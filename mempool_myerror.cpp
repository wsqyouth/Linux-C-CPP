/*
参考网址：https://blog.csdn.net/K346K346/article/details/49538975
https://blog.csdn.net/nanjunxiao/article/details/8970396
https://blog.csdn.net/D_Guco/article/details/76312469
https://blog.csdn.net/chexlong/article/details/7071922

待完善：内存对齐问题，若mp是全局对象，内存合适被析构？ 是否采用大块内存问题
代码出错：问题在维护的block表地址被修改了，free了非法地址
初始想法：每个内存单元使用链表链接，但是由于free时内存单元的next指针修改，导致原来内存单元链接被断掉
*/
#include <iostream>
#include <stdint.h>
using namespace std;
/*
思路：两个结构体
使用new出count个blocksize
*/

class MemPool
{
private:
	
	//空闲节点结构体
	//每个放在一个blocksize的头部
	struct FreeNode
	{
		FreeNode* pNextBlock;
	};

	//内存块结构体
	//pNext指向下一个MemBlock
	//pFirstMemBlock指向每一串中的第一个FreeNode,方便销毁
	struct MemBlock
	{
		MemBlock* pNext;
		FreeNode* pFirstFreeNode;
	};

	FreeNode* freeNodeHeader;//指向空闲FreeNode,头插头删
	MemBlock* memBlockHeader;//指向当前MemBlock，方便扩展

	uint32_t nBlockSize;  //大小
	uint32_t nBlockCount; //个数
public:
	MemPool(uint32_t blockSize, uint32_t blockCount )
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
	~MemPool();

	void* Alloc();
	void Free(void*);
};
void* MemPool::Alloc()
{
	//无空闲节点，申请新内存块
	if (freeNodeHeader == NULL)
	{
		//第一个FreeNode
		freeNodeHeader = (FreeNode*)malloc(nBlockSize);
		freeNodeHeader->pNextBlock = NULL;

		for (uint32_t i = 1; i < nBlockCount; ++i)
		{
			FreeNode* pNewFreeNode = NULL; //raw mem ptr
			/*一个block一个block申请，每个block大小为blocksize,首部用作FreeNode，当分配出去时无需考虑内容，只考虑大小*/
			pNewFreeNode = (FreeNode*)malloc(nBlockSize);
			pNewFreeNode->pNextBlock = NULL; //初始化(后面的内存为raw mem)
			if (NULL == pNewFreeNode)
			{
				perror("CMemPool::Init malloc failed\n");
				return NULL;
			}
			//头插法插入空闲链表
			pNewFreeNode->pNextBlock = freeNodeHeader->pNextBlock;
			freeNodeHeader->pNextBlock = pNewFreeNode;
		}
		//第一个MemBlock
		MemBlock* newBlock = (MemBlock*)malloc(sizeof(MemBlock));
		newBlock->pFirstFreeNode = freeNodeHeader;
		newBlock->pNext = NULL;

		//首次申请内存块
		if (memBlockHeader == NULL)
		{
			memBlockHeader = newBlock;
		}
		else
		{
			//将新内存块加入到内存块链表
			newBlock->pNext = memBlockHeader;
			memBlockHeader = newBlock;
		}
		
	}
	//返回空节点闲链表的第一个节点
	void* freeNode = freeNodeHeader;
	freeNodeHeader = freeNodeHeader->pNextBlock;
	return freeNode;
}
//释放已经分配的节点
void MemPool::Free(void* p)
{
	FreeNode* pNode = (FreeNode*)p;
	pNode->pNextBlock = freeNodeHeader;//将释放的节点插入空闲节点头部
	freeNodeHeader = pNode;
}
 MemPool::~MemPool()
{
	MemBlock* pNextMemBlock=NULL;
	while (memBlockHeader)
	{
		pNextMemBlock = memBlockHeader->pNext;
		//根据pFirstFreeNode对所有连接的block进行free
		FreeNode* pFreeNode = memBlockHeader->pFirstFreeNode;
		FreeNode* pTmp;
		while (pFreeNode)
		{
			pTmp = pFreeNode->pNextBlock;
			if (pFreeNode) {
				free(pFreeNode);
				pFreeNode = NULL;
			}
				
			pFreeNode = pTmp;
		}
		//查找下一个memBlock
		memBlockHeader = pNextMemBlock;
	}
}


 //测试
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
 MemPool mp(24, 2);

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
	/*
	{
	
		void *ptr = mp.Alloc();
		cout << ptr << endl;
		mp.Free(ptr);
		void *ptr1 = mp.Alloc();
		cout << ptr1 << endl;
		
		
	}
	*/
	{
		ActualClass* p1 = new ActualClass;
		p1->print();

		ActualClass* p2 = new ActualClass;
		p2->print();
		
		ActualClass* p3 = new ActualClass;
		p3->print();

		delete p1;
		delete p2;
		delete p3;
		
	}
	cout << "hello" << endl;
//	getchar();
	return 0;
}
