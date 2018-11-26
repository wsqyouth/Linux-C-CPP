test.cpp
----
#include <iostream>
#include <cassert>
#include <list>
#include <vector>
#include "Mempool.h"
/*
 * 参考：https://blog.csdn.net/nanjunxiao/article/details/8970396
 * 为了测试自己对该代码的理解：管理节点数组中每个节点指向一片连续内存中的一块，stFree数组头插头删进行Alloc和Free。
 * 目前能够完成对已有空闲内存块的分配，但是无法扩展，比如已有3个内存块，用完之后，下一个申请者为NULL，不是很明白
 * 原作者stToFree是怎么使用的，后面稳固复习时再研究下。
 *
 * 测试结果：
 *  0x1330c50
 *  0x1330c38
 *  0x1330c20
 *  (nil)
 *  每个内存块大小为24byte，满足题意
 */
using namespace std;

int main() 
{
	CMemPool mp;
	mp.Init(3,24);
	void *ptr = mp.AllocBlock();
	printf("%p\n", ptr);
	void *ptr1 = mp.AllocBlock();
	printf("%p\n", ptr1);
	void *ptr2 = mp.AllocBlock();
	printf("%p\n", ptr2);
	mp.FreeBlock(ptr);
	void* ptr3 = mp.AllocBlock();
	printf("%p\n", ptr3);
	mp.Exit();
	printf("hello\n");

	return 0;
}


Mempool.h
-----
#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_
/*Readme:
 * 1.内存池为多个相同大小的内存block集合，block大小和个数在初始化是设定。
 * 2.用两个链表管理内存池的block集合，一个为空闲块链表(可申请)，另一个为
 *  待释放块链表，这两个链表均为带头结点的单向非循环链表。实现时只提供空闲
 *  链表头和待释放链表头，具体链表节点为block管理节点(和block一一对应),每个
 *  block管理节点内部包含block内存地址，大小和指向下一个管理节点的next指针。
 * 3.申请时从空闲块链表上申请，若空闲块链表上已经没有空闲块，则将空闲块链表
 *  和待释放块链表交换(加锁)；若申请成功，则从空闲块链表上删除该节点，给相应线程
 *  使用（删除后不管它了）
 * 4.释放时，将线程释放的内存块挂入待释放链表（加锁）
 * 5.空闲链表头部加入，头部取出，类似于栈,待释放链表只头部插入
 * 6.为保证多线程安全，空闲块链表和待释放块链表各有一个互斥锁
 */
 
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/*是否一次性申请一大块内存，然后切分成小block；
 *否则一小块一小块申请
 *目前机器内存不是问题，默认使用第一种方式
 */
#define USE_LARGEMEMORY
 
class CMemPool
{
public:
    CMemPool()
    {
#ifdef USE_LARGEMEMORY
        m_pLargeBlock = NULL;
#endif
        m_uiBlockSize = 0;
        m_uiBlockCount = 0;
        m_uiBlockCountAlloc = 0;
        m_uiBlockCountFree = 0;
        m_pListNodeArray = NULL;
        m_stFreeListHead.next = NULL;
        m_stTofreeListHead.next = NULL;
    }
    virtual ~CMemPool()
    {
    }
 
     /*
     * 内存池初始化
     * uiBlockSize为分配的内存块block大小byte,相同大小块
     * uiBlockCount为分配的内存块数目
     * */
    bool Init(uint32_t uiBlockCount, uint32_t uiBlockSize)
    {
        if(uiBlockCount==0 || 0==uiBlockSize)
            return false;
#ifdef USE_LARGEMEMORY
        m_pLargeBlock = malloc(uiBlockCount*uiBlockSize);
        if(NULL == m_pLargeBlock)
        {
            printf("CMemPool::Init malloc large mem error 1\n");
            return false;
        }
#endif
        m_pListNodeArray = (MEMLISTNODE*)malloc(uiBlockCount*sizeof(MEMLISTNODE) );
        if(NULL == m_pListNodeArray)
        {
            printf("CMemPool::Init malloc MEMLISTNODE array error 2\n");
            return false;;
        }
        memset(m_pListNodeArray,0,uiBlockCount*sizeof(MEMLISTNODE) );
 
        if(uiBlockCount>0 && uiBlockSize>0)//check again
        {
            /*申请uiBlockCount个大小为uiBlockSize byte的内存块*/
            for(uint32_t i=0; i<uiBlockCount; ++i)
            {
                void* pNewBlock = NULL; //raw mem ptr
#ifndef USE_LARGEMEMORY
                /*一个block一个block申请，基本不采用此方式*/
                pNewBlock = malloc(uiBlockSize);
                if(NULL == pNewBlock)
                {
                    printf("CMemPool::Init malloc failed 3\n");
                    return false;
                }
#else
                pNewBlock = (void*)((char*)m_pLargeBlock + i*uiBlockSize);
#endif
                /*新建一个链表节点，即block管理节点*/
                MEMLISTNODE* pNewNode = &m_pListNodeArray[i];
                pNewNode->pvMemBlock = pNewBlock;
				pNewNode->uiBlockSize = uiBlockSize;
                /*加入到空闲块链表*/
                pNewNode->next = m_stFreeListHead.next;
                m_stFreeListHead.next = pNewNode;
            }
        }
        m_uiBlockCount = uiBlockCount;
        m_uiBlockSize = uiBlockSize;
 
        return true;
    }
 
    void Exit()
    {
#ifdef USE_LARGEMEMORY
        if(m_pLargeBlock)
        {
            free(m_pLargeBlock);
            m_pLargeBlock = NULL;
        }
#endif
        if(NULL != m_pListNodeArray)
        {
#ifndef USE_LARGEMEMORY
            for(uint32_t i=0; i<m_uiBlockSize; ++i)
            {
                MEMLISTNODE* p = &m_pListNodeArray[i];
                if(NULL != p->pvMemBlock)
                {
                    free(p->pvMemBlock);
                    p->pvMemBlock = NULL;
                }
            }
#endif
            free(m_pListNodeArray);
            m_pListNodeArray = NULL;
        }
    }
 
    /*
     * 从内存池中申请一个空闲block
     * pBlock内存块block首地址，raw mem ptr
     * uiBlockSize 内存块block大小
     * 成功返回空闲块ID,管理节点地址，否则NULL
     */
    void* AllocBlock0(void* &pBlock, uint32_t& uiBlockSize)
    {
        void* nBlockId = NULL;
        MEMLISTNODE* pAvlNode = NULL;
        if(NULL == m_stFreeListHead.next)
        {
            if(NULL == m_stTofreeListHead.next)
            {
                return NULL;
            }
            MEMLISTNODE* ptofreeHead;
            {
                /*待释放链表实际头结点赋给空闲链表，自己为NULL*/
                ptofreeHead = m_stTofreeListHead.next;
                m_stTofreeListHead.next = NULL;
            }
            //这时待释放链表其他线程可以头部加入了
            //交换
            m_stFreeListHead.next = ptofreeHead;
        }
        pAvlNode = m_stFreeListHead.next;
        if(pAvlNode)//申请成功
        {
            m_stFreeListHead.next = m_stFreeListHead.next;
            m_uiBlockCountAlloc++;//已申请内存块个数

            pBlock      = pAvlNode->pvMemBlock;//raw
            uiBlockSize = pAvlNode->uiBlockSize;
            nBlockId     = (void*)pAvlNode;
        }
        return nBlockId;
    }
 
 
    /*
     * 从内存池中申请一个空闲block
     * 成功空闲块对应指向的raw mem地址，否则NULL
     */
    void* AllocBlock()
    {
        void* nBlockId = NULL;
        MEMLISTNODE* pAvlNode = NULL;
        if(NULL == m_stFreeListHead.next)
        {
            if(NULL == m_stTofreeListHead.next)
            {
                return NULL;
            }
            MEMLISTNODE* ptofreeHead;
            {
                /*待释放链表实际头结点赋给空闲链表，自己为NULL*/
                ptofreeHead = m_stTofreeListHead.next;
                m_stTofreeListHead.next = NULL;
            }
            //这时待释放链表其他线程可以头部加入了
            //交换
            m_stFreeListHead.next = ptofreeHead;
        }
        pAvlNode = m_stFreeListHead.next;
        if(pAvlNode)//申请成功
        {
            m_stFreeListHead.next = m_stFreeListHead.next->next;
            m_uiBlockCountAlloc++;//已申请内存块个数

            //pBlock      = pAvlNode->pvMemBlock;//raw
            //uiBlockSize = pAvlNode->uiBlockSize;
            nBlockId     = (void*)pAvlNode->pvMemBlock;//raw mem
        }
        return nBlockId;
    }
    void FreeBlock(void* blockID)
    {
        MEMLISTNODE* pNode = (MEMLISTNODE*)blockID;
        if(pNode)
        {
            pNode->next = m_stTofreeListHead.next;
            m_stTofreeListHead.next = pNode;
 
            m_uiBlockCountFree ++;//已释放内存块个数
        }
    }
 
    uint32_t GetBlockSize()
    {
        return m_uiBlockSize;
    }
 
    uint32_t GetBlockCount()
    {
        return m_uiBlockCount;
    }
	/*正在被线程使用的内存block个数*/
    uint32_t GetBusyBlockCount()
    {
        return m_uiBlockCountAlloc - m_uiBlockCountFree;
    }
private:
    /*block管理结构，管理数组，空闲链表、待释放链表节点*/
    typedef struct _stMemListNode
    {
		void* pvMemBlock; //指向内存块block起始地址,raw mem ptr
        uint32_t uiBlockSize; //内存块block大小
        struct _stMemListNode* next; //next指针
    }MEMLISTNODE;
private:
#ifdef USE_LARGEMEMORY
    void* m_pLargeBlock; //一大块内存起始地址
#endif
    uint32_t m_uiBlockSize; //block内存块的大小(byte)
    uint32_t m_uiBlockCount; //内存块数目
 
    uint32_t m_uiBlockCountAlloc; //所有线程已申请的内存块数目
    uint32_t m_uiBlockCountFree; //所有线程已释放的内存块数目

	MEMLISTNODE m_stTofreeListHead; //待释放链表头结点
	MEMLISTNODE m_stFreeListHead; //空闲块链表头结点
private:
    MEMLISTNODE* m_pListNodeArray; //链表节点数组,block集合管理节点数组,和block集合一一对应
};
#endif
