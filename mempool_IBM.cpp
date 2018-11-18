参考书籍：《C++应用程序性能优化》 冯洪华
参考代码：https://www.xuebuyuan.com/1443916.html
         https://www.ibm.com/developerworks/cn/linux/l-cn-ppp/index6.html
         和https://www.cnblogs.com/cumtli/p/5773188.html
总结：IBM这种思路主要是一个内存管理单元+多个分配的内存单元的思想。根据内存对齐后的unit计算索引。

当然最好是.cpp和.h分离，我这里暂时合起来了，方便调试。

MemoryPool.h
------
/*
内存管理单元MemoryBlock后面跟N个内存单元unit，该内存单元即为所分
*/

#ifndef __MEMORY_POOL_H_
#define __MEMORY_POOL_H_
#include <stdlib.h>
#include <iostream>

//平台内存对齐长度
const int MEMPOOL_ALIGNMENT = 8;
//内存块首部，描述该内存块的结构体，unit_num为内存单元数目，unit_size为内存单元大小
struct MemoryBlock {
public:
    MemoryBlock(int unit_num, int unit_size);
    static void* operator new(size_t,int unit_num, int unit_size);
    static void operator delete(void *pblock);

    int nTotalSize;//内存块总大小（不包括memblock头部空间）
    MemoryBlock* pNextBlock;//指向下一个内存块
    int nFreeNum;//空闲内存块数目
    int nFirstFreeIndex;//下一个可用内存单元的index
    char aData[1];//重要：内存单元的起始地址
};

class MemoryPool {
public:
    MemoryPool(int unit_num, int unit_size,int grow_size);
    ~MemoryPool();

    void *Alloc();
    void Free(void *);

    void Static();
private:
    MemoryBlock* pBlock;
    int nUnitNumPerBlock;
    int nUnitSize;
    int nGrowSize;
};

MemoryBlock::MemoryBlock(int unit_num, int unit_size) {
    nTotalSize = unit_num * unit_size;
    nFreeNum = unit_num - 1;
    nFirstFreeIndex = 1;
    pNextBlock = NULL;

    //给每个空闲unit头2个字节分配编号
    char* data = aData;
    for(int i=1; i< unit_num; ++i){
        (*(unsigned short*)data) = i;
        data += unit_size;
    }
}
void*  MemoryBlock::operator new(size_t,int unit_num, int unit_size){
    return ::operator new(sizeof(MemoryBlock) + unit_num * unit_size);
}
void  MemoryBlock::operator delete(void*  pblock) {
    ::operator delete(pblock);
}

MemoryPool::MemoryPool(int unit_num, int unit_size,int grow_size){
    pBlock = NULL;
    nUnitNumPerBlock = unit_num;
    nUnitSize = unit_size;
    nGrowSize = grow_size;
    /*
    內存对齐：若内存对齐不是8的整数倍，则向上取整为8的倍数
    举例：若unit_size=12(7),则nUnitSize=16(8)
    */
    if(unit_size > 4) {
        nUnitSize = (unit_size + (MEMPOOL_ALIGNMENT - 1)) & ~(MEMPOOL_ALIGNMENT-1);
    }else if(unit_size < 2)
    {
        //每个unit在空闲状态时，头两个字节存放“下一个unit的编号”，最少为2
        nUnitSize = 2;
    }else {
        nUnitSize = 4;
    }
    
}
MemoryPool::~MemoryPool() {
    //根据pBlock和pNextBlock依次delete
    MemoryBlock* pblockTmp = pBlock;

    while(pblockTmp != NULL) {
        pblockTmp = pblockTmp->pNextBlock;

        if(pblockTmp)
            delete(pblockTmp);
    }
}
void* MemoryPool::Alloc() {
    //第一次内存申请
    if(NULL == pBlock)
    {
            pBlock = (MemoryBlock*)new(nUnitNumPerBlock,nUnitSize) MemoryBlock(nUnitNumPerBlock,nUnitSize);

            if(pBlock) {
                    return (void*)pBlock->aData;
            }
    }
    //临时变量存储pBlock头指针 
    MemoryBlock *tmp_block_ptr = pBlock;
    //不是第一次分配，寻找空闲unit的block
    while(tmp_block_ptr !=NULL && tmp_block_ptr->nFreeNum == 0){
       tmp_block_ptr = tmp_block_ptr ->pNextBlock;
    }
    //若找到含有空闲unit的block
    if(tmp_block_ptr)
    {
        //根据空闲索引找到free，空闲unit数目减1，更新空闲索引
        char *free = tmp_block_ptr->aData + tmp_block_ptr->nFirstFreeIndex * nUnitSize;
        tmp_block_ptr->nFreeNum --;
        tmp_block_ptr->nFirstFreeIndex = *((unsigned short*)free);
        return (void*)free;
    }
    else
    {
        //若没有含有空闲unit的block,则重新new一个block
        MemoryBlock *block = (MemoryBlock*)new(nGrowSize,nUnitSize) MemoryBlock(nGrowSize,nUnitSize);
        if(block)
        {
            //将重新new的block放到最开始（靠前）的位置
            block->pNextBlock = pBlock;
            pBlock = block;
            return (void*) (block->aData);
        }
    }
}
void MemoryPool::Free(void *free) {
    //先找到free所在的block
    MemoryBlock* tmp_block_ptr = pBlock;
    MemoryBlock* pre_block_ptr = NULL;
    //根据free指针地址比较合适block
    while(tmp_block_ptr && ((unsigned long)tmp_block_ptr->aData + tmp_block_ptr->nTotalSize <= (unsigned long)free || (unsigned long)free < (unsigned long)tmp_block_ptr->aData))
    {
            pre_block_ptr = tmp_block_ptr;
            tmp_block_ptr = tmp_block_ptr->pNextBlock;
    }
    //找到了 
    if(tmp_block_ptr)
    {
        //头插至该block处(给free填写原空闲索引，同时更新空闲索引)
        tmp_block_ptr->nFreeNum++;
        *((unsigned short*)free) = tmp_block_ptr->nFirstFreeIndex;
        tmp_block_ptr->nFirstFreeIndex = (unsigned short)((unsigned long)free - (unsigned long)(tmp_block_ptr->aData)) / nUnitSize;//计算索引

        //判断该block是否全部处于空闲状态
        if(tmp_block_ptr->nFreeNum * nUnitSize == tmp_block_ptr->nTotalSize)
        {
                if(NULL != pre_block_ptr)
                {
                    //若全部空闲且前驱非空，则删除该block(A->B->C A-C)
                    pre_block_ptr->pNextBlock = tmp_block_ptr->pNextBlock;
                }
                else {
                    pBlock = tmp_block_ptr->pNextBlock;
                }
                delete (tmp_block_ptr);
     }
     else
     {
        //若并非全部空闲，则将该block插入到最前头
        //若已时最前头，则不必移动，防止当前只有一个block，重复释放内存
        if(tmp_block_ptr != pBlock)
        {
            pre_block_ptr->pNextBlock = tmp_block_ptr->pNextBlock;
            //头插至pBlock之前，同时更新pBlock
            tmp_block_ptr->pNextBlock = pBlock;
            pBlock = tmp_block_ptr;
        }
     }
   }
    else
   {
       std:: cout << "not find the block...\n";
       return;
   }
}

void MemoryPool::Static(){
    int count = 1;
    
    MemoryBlock *tmp_block_ptr = pBlock;
    //检查pBlock是否已空 
    if(NULL == tmp_block_ptr) {
        std::cout << "all memory block is free!\n";
        return;
    }
    else {
    //若非空，则计算block个数和空闲索引
            while(tmp_block_ptr->pNextBlock != NULL) {
                count++;
                tmp_block_ptr = tmp_block_ptr->pNextBlock;
            }
           std:: cout << "still left block: " << count <<"\n";
           
           tmp_block_ptr = pBlock;
           while(tmp_block_ptr) {
                   std::cout << "[static]current block free num: "<<tmp_block_ptr->nFreeNum <<"\n";
                   std::cout << "[static]current free num: "<<tmp_block_ptr->nFirstFreeIndex<<"\n";
                   tmp_block_ptr = tmp_block_ptr->pNextBlock;
           }
    }
}
#endif


TestClass.h
------
#ifndef _TESTCLASS_H_
#define _TESTCLASS_H_

#include "MemoryPool.h"

class TestClass {
public:
    TestClass(int para);
    ~TestClass();

    int GetTotal(){
        return total;
    }

    void* operator new (std::size_t);
    void operator delete(void* free);

    static MemoryPool mem_pool;

private:
    int total;
};


TestClass::TestClass(int para){
    total = para;
}
TestClass::~TestClass() {
    
}

void* TestClass::operator new (std::size_t){
    std::cout << "testclass operator new is called.\n";
    return mem_pool.Alloc();
}
void TestClass::operator delete(void* free) {
    std::cout << "testclass operator delete is called.\n";
    mem_pool.Free(free);
}

#endif

Main.cpp
#include <iostream>
#include "MemoryPool.h"
#include "TestClass.h"
using namespace std;

MemoryPool TestClass::mem_pool(sizeof(TestClass),1000,50);

int main()
{
    TestClass* tmp = (TestClass*) new TestClass(100);
    cout << "total value: " << tmp->GetTotal() <<endl;
    delete tmp;


    TestClass::mem_pool.Static();
    cout << "hello "<<endl;
    return 0;
    
}
