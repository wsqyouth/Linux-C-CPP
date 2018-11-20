这里我将原文的代码修改了下，更方便阅读。
主要的思想：MemBlock的pNextMem方便纵向扩展,横向指向一片连续的内存（该内存包括blocksize大小的内存单元nBlockCount个,主要是方便销毁delete[]）
	   其中每个blocksize大小的内存单元的前几个字节复用为pNext域，当分离出去后整合为一个blocksize大小的空闲内存单元.
因此，我在修改为传统c++代码时，借鉴了连续内存的思想：
m_pLargeBlock 指向一大块内存，该内存包括blocksize大小的内存单元nBlockCount个,主要是方便销毁(直接free m_pLargeBlock即可)
	
原代码：
 g++ CMymem.cpp -std=c++11 -o CMymem
 提供了安全的Alloc和Free函数(c++11的锁保护)
----
#include <iostream>
#include<ctime>
#include <vector>
#include <mutex>
using namespace std;
clock_t startTime,endTime;

template<int ObjectSize, int NumofObjects = 20>
class MemPool
{
private:
	//空闲节点结构体
	struct FreeNode
	{
		FreeNode* pNext;
		char data[ObjectSize];
	};

	//内存块结构体
	struct MemBlock
	{
		MemBlock* pNextMem;
		FreeNode dataNode[NumofObjects];
	};

	FreeNode* freeNodeHeader;
	MemBlock* memBlockHeader;

public:
	MemPool()
	{
		freeNodeHeader = NULL;
		memBlockHeader = NULL;
	}

	~MemPool()
	{
		MemBlock* ptr;
		while (memBlockHeader)
		{
			ptr = memBlockHeader->pNextMem;
			delete memBlockHeader;
			memBlockHeader = ptr;
		}
	}
	void* malloc();
	void free(void*);
};

//分配空闲的节点
template<int ObjectSize, int NumofObjects>
void* MemPool<ObjectSize, NumofObjects>::malloc()
{
	//无空闲节点，申请新内存块
	if (freeNodeHeader == NULL)
	{
		MemBlock* newBlock = new MemBlock;
		newBlock->pNextMem = NULL;

		freeNodeHeader=&newBlock->dataNode[0];	 //设置内存块的第一个节点为空闲节点链表的首节点
		//将内存块的其它节点串起来
		for (int i = 1; i < NumofObjects; ++i)
		{
			newBlock->dataNode[i - 1].pNext = &newBlock->dataNode[i];
		}
		newBlock->dataNode[NumofObjects - 1].pNext=NULL;

		//首次申请内存块
		if (memBlockHeader == NULL)
		{
			memBlockHeader = newBlock;
		}
		else
		{
			//将新内存块加入到内存块链表
			newBlock->pNextMem = memBlockHeader;
			memBlockHeader = newBlock;
		}
	}
	//返回空节点闲链表的第一个节点
	void* freeNode = freeNodeHeader;
	freeNodeHeader = freeNodeHeader->pNext;
	return freeNode;
}

//释放已经分配的节点
template<int ObjectSize, int NumofObjects>
void MemPool<ObjectSize, NumofObjects>::free(void* p)
{
	FreeNode* pNode = (FreeNode*)p;
	pNode->pNext = freeNodeHeader;	//将释放的节点插入空闲节点头部
	freeNodeHeader = pNode;
}
 
template<int ObjectSize, int NumofObjects>
class CMemoryPoolSafty : public MemPool<ObjectSize,NumofObjects>  {
private:
    std::mutex	m_utex;
    
public:
	 //创建ObjectSize* nNumOfElements大小的空间
     //从内存池中获取一个对象空间
	 void* malloc()
     {
		std::lock_guard<std::mutex> guard(m_utex);
        return MemPool<ObjectSize, NumofObjects>::malloc();
     }
     
     //内存池回收一个对象空间
     void free(void* p)
     {
        std::lock_guard<std::mutex> guard(m_utex);
        MemPool<ObjectSize, NumofObjects>::free(p);
     }
};


#define DATA_BLOCK_LEN 1024*1024
 
int main()
{
	std::vector<void*> array;
	array.reserve(1000);

	startTime=clock();		//程序开始计时
	for(int j=0;j<500;++j) {
		for(int i=0;i<1000;++i) {
			array[i] =(void*) new char[DATA_BLOCK_LEN];
		}
		for(int i=0;i<1000;++i) {
			delete [] (char*)array[i];
		}
	}

	endTime=clock();		//程序结束用时
	double endtime=(double)(endTime-startTime)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位
	

	//分配1000块内存,每个块DATA_BLOCK_LEN= 1024*10 bytes
	CMemoryPoolSafty <DATA_BLOCK_LEN, 1000> myPool2;  //MemPool
	startTime=clock();		//程序开始计时
	for(int j=0;j<500;++j) {
		for(int i=0;i<1000;++i) {
			array[i] =(void*) myPool2.malloc();
		}
		for(int i=0;i<1000;++i) {
			//delete (char*);
			myPool2.free(array[i]);
		}
	}

	endTime=clock();		//程序结束用时
    endtime=(double)(endTime-startTime)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位
	return 0;
}

------
我的代码：
g++ mempool_my.cpp -std=c++11 -o mempool_my
/*
参考：https://blog.csdn.net/K346K346/article/details/49538975
https://blog.csdn.net/nanjunxiao/article/details/8970396
初始想法：每个内存单元使用链表链接，但是由于free时内存单元的next指针修改，导致原来内存单元链接被断掉
本次修改：因此参考博客中的想法：每一个内存链实质上是一大块内存(共nBlockSize×nBlockCount)。物理的连接方便析构销毁
         其中每个内存单元有复用的功能：空闲时内存单元的前几个字节作为next指针链接空闲内存单元，当完全分配出去时不需要考虑内容，大小刚好为nBlockSize
		 也就是说FreeNode内存块功能和FreeNode*指针功能复用，此时一个前提是：FreeNode内存块大小 >= FreeNode*指针大小
*/
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <ctime>
#include <vector>
#include <mutex>
using namespace std;
clock_t startTime,endTime;


class MemPool
{
private:
	//空闲节点结构体
	struct FreeNode
	{
		FreeNode* pNext;
	};

	//内存块结构体
	struct MemBlock
	{
		MemBlock* pNextMem; //pNextMem指向下一个MemBlock
		void* m_pLargeBlock; //一大块内存起始地址
	};

	FreeNode* freeNodeHeader;
	MemBlock* memBlockHeader;
	

	uint32_t   nBlockSize;  //大小
	uint32_t   nBlockCount; //个数
public:
	MemPool(uint32_t   blockSize, uint32_t   blockCount)
	{
		freeNodeHeader = NULL;
		memBlockHeader = NULL;
		//nBlockSize  = blockSize;
		nBlockCount = blockCount;
		/*
		每个nUnitSize最少是一个指针的大小,而指针大小与平台有关
		余下的是raw memory
		*/
		nBlockSize = blockSize > sizeof(FreeNode*) ? blockSize : sizeof(FreeNode*);

	}
	~MemPool() {
		MemBlock* pNextMemBlock = NULL;
		while (memBlockHeader)
		{
			pNextMemBlock = memBlockHeader->pNextMem;

			if (memBlockHeader->m_pLargeBlock) {
				free(memBlockHeader->m_pLargeBlock);
				memBlockHeader->m_pLargeBlock = NULL;
			}
			free(memBlockHeader);

			memBlockHeader = pNextMemBlock;
		}
	}

	void* Alloc();
	void Free(void*);
};

void* MemPool::Alloc()
{
	//无空闲节点，申请新内存块
	if (freeNodeHeader == NULL)
	{
		//第一个MemBlock
		MemBlock* newBlock = (MemBlock*)malloc(sizeof(MemBlock));
		newBlock->m_pLargeBlock =(void*)malloc(nBlockCount * (nBlockSize)); //sizeof(FreeNode*) + 
		if (NULL == newBlock->m_pLargeBlock)
		{
			cout<< "CMemPool::Init malloc large mem error 1\n";
			return NULL;
		}
		newBlock->pNextMem = NULL;

		freeNodeHeader = (FreeNode*)newBlock->m_pLargeBlock;	 //设置内存块的第一个节点为空闲节点链表的首节点

		//将内存块的其它节点串起来(第一个空闲节点之后相邻内存块之间进行链接：初始空闲链表的链接)
		char *pPrev = NULL;
		FreeNode* pCurNode = NULL;
		FreeNode* pPrevNode = NULL;
		for (uint32_t  i = 1; i < nBlockCount; ++i)
		{
			//pPrevNode = (FreeNode*)((char*)newBlock->m_pLargeBlock + (i - 1)*nBlockSize);
			//pCurNode = (FreeNode*)((char*)newBlock->m_pLargeBlock + i*nBlockSize);
			pPrev = (char*)((char*)newBlock->m_pLargeBlock + (i - 1)*nBlockSize);
			pPrevNode = (FreeNode*)(pPrev);
			pCurNode = (FreeNode*)(pPrev+nBlockSize);
			
			pPrevNode->pNext = pCurNode;
		}
 		//最后一个内存块的pNext设置为NULL
		FreeNode* pLastNode = (FreeNode*)((char*)newBlock->m_pLargeBlock + (nBlockCount - 1)*nBlockSize);
		pLastNode->pNext = NULL;
		
		//首次申请内存块
		if (memBlockHeader == NULL)
		{
			memBlockHeader = newBlock;
		}
		else
		{
			//将新内存块加入到内存块链表
			newBlock->pNextMem = memBlockHeader;
			memBlockHeader = newBlock;
		}
	
	}

	//返回空节点闲链表的第一个节点
	void* freeNode = freeNodeHeader;
	freeNodeHeader = freeNodeHeader->pNext;
	return freeNode;
}

//释放已经分配的节点
void MemPool::Free(void* p)
{
	FreeNode* pNode = (FreeNode*)p;
	pNode->pNext = freeNodeHeader;//将释放的节点插入空闲节点头部
	freeNodeHeader = pNode;
	return;
}

class CMemoryPoolSafty: public MemPool{
private:
    std::mutex	m_utex;
    
public:
	 //基类构造函数带参数的继承方式
	 CMemoryPoolSafty(uint32_t blockSize, uint32_t blockCount):MemPool(blockSize,blockCount)
	 {
	 }
	 //析构函数则不必显式定义

	 //创建ObjectSize* nNumOfElements大小的空间
     //从内存池中获取一个对象空间
	 void* Alloc()
     {
		std::lock_guard<std::mutex> guard(m_utex);
        return MemPool::Alloc();
     }
     
     //内存池回收一个对象空间
     void Free(void* p)
     {
        std::lock_guard<std::mutex> guard(m_utex);
        MemPool::Free(p);
     }
};
#if 0
//测试1：检测内存池的分配
int main()
{
	MemPool mp(24,2);
	void *ptr = mp.Alloc();
	printf("%p\n", ptr);
	void *ptr1 = mp.Alloc();
	printf("%p\n", ptr1);
	void *ptr2 = mp.Alloc();
	printf("%p\n", ptr2);
	mp.Free(ptr);
	void* ptr3 = mp.Alloc();
	printf("%p\n", ptr3);
	printf("hello\n");
	return 0;
}
#endif

#if 0
//测试2：检测对类对象的分配
class ActualClass
{
	static int count;
	int No;

public:
	ActualClass()
	{
		No = count;
		count++;
	}

	void print()
	{
		cout << this << ": ";
		cout << "the " << No << "th object" << endl;
	}

	void* operator new(size_t size);
	void operator delete(void* p);
};

//定义内存池对象
MemPool mp(sizeof(ActualClass), 3);

void* ActualClass::operator new(size_t size)
{
	return mp.Alloc();
}

void ActualClass::operator delete(void* p)
{
	mp.Free(p);
}

int ActualClass::count = 0;


int main()
{
while(1)
{	
	ActualClass* p1 = new ActualClass;
	p1->print();

	ActualClass* p2 = new ActualClass;
	p2->print();
	delete p1;

	p1 = new ActualClass;
	p1->print();

	ActualClass* p3 = new ActualClass;
	p3->print();

	delete p1;
	delete p2;
	delete p3;
}

}

#endif

#if 1
//测试3:500次，内存块大小为1024*1024bytes 共1000个，计时对比
#define DATA_BLOCK_LEN 1024*1024
 
int main()
{
	std::vector<void*> array;
	array.reserve(1000);

	startTime=clock();		//程序开始计时
	for(int j=0;j<500;++j) {
		for(int i=0;i<1000;++i) {
			array[i] =(void*) new char[DATA_BLOCK_LEN];
		}
		for(int i=0;i<1000;++i) {
			delete [] (char*)array[i];
		}
	}

	endTime=clock();		//程序结束用时
	double endtime=(double)(endTime-startTime)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位
	

	//分配1000块内存,每个块DATA_BLOCK_LEN= 1024*10 bytes
	CMemoryPoolSafty myPool2(DATA_BLOCK_LEN, 1000);  //MemPool
	startTime=clock();		//程序开始计时
	for(int j=0;j<500;++j) {
		for(int i=0;i<1000;++i) {
			array[i] =(void*) myPool2.Alloc();
		}
		for(int i=0;i<1000;++i) {
			myPool2.Free(array[i]);
		}
	}

	endTime=clock();		//程序结束用时
    endtime=(double)(endTime-startTime)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位

	return 0;
}
#endif

