/*
参考：https://blog.csdn.net/K346K346/article/details/49538975
https://blog.csdn.net/nanjunxiao/article/details/8970396
MemBlock 指向一大块内存，该内存包括blocksize大小的内存单元nBlockCount个
其中每个blocksize大小的内存单元的前几个字节复用为pNext域，当分离出去后整合为一个blocksize大小的空闲内存单元
*/
#include <iostream>
#include <cstdint>
using namespace std;

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
		MemBlock* pNextMem;
		//FreeNode* pFirstFreeNode;
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
			perror("CMemPool::Init malloc large mem error 1\n");
			_exit(-1);
		}
		newBlock->pNextMem = NULL;

		freeNodeHeader = (FreeNode*)newBlock->m_pLargeBlock;	 //设置内存块的第一个节点为空闲节点链表的首节点

		//将内存块的其它节点串起来
		FreeNode* pCurNode = NULL;
		FreeNode* pPrevNode = NULL;
		for (uint32_t  i = 1; i < nBlockCount; ++i)
		{
			pPrevNode = (FreeNode*)((char*)newBlock->m_pLargeBlock + (i - 1)*nBlockSize);
			pCurNode = (FreeNode*)((char*)newBlock->m_pLargeBlock + i*nBlockSize);
			pPrevNode->pNext = pCurNode;
		}
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
#if 0
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
