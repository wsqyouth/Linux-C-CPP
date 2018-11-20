这里提供两份代码：都是正确的，只是思想不一样。

一份代码是：https://blog.csdn.net/D_Guco/article/details/76312469
该代码使用的new一个对象数组（连续内存）,因为他这里直接对一个T类型的对象分配内存，不必考虑传入指定数量的内存大小。
优点：纵向扩展、内存单元使用双向链表连接、安全分配时扩展类、传统锁类封装。

Lock.h
----
#ifndef _Lock_H
#define _Lock_H
 
#include <pthread.h>
 
//锁接口类
class ILock
{
public:
	virtual ~ILock() {}
 
	virtual void Lock() const = 0;
	virtual void Unlock() const = 0;
};
 
//互斥锁类
class CMutex : public ILock
{
public:
	CMutex();
	~CMutex();
 
	virtual void Lock() const;
	virtual void Unlock() const;
 
private:
	mutable pthread_mutex_t m_mutex;
};
 
//锁
class CMyLock
{
public:
	CMyLock(const ILock&);
	~CMyLock();
 
private:
	const ILock& m_lock;
};
 
 
#endif

Lock.cpp
----
#include "Lock.h"
 
 
//动态方式初始化互斥锁
CMutex::CMutex()
{
	pthread_mutex_init(&m_mutex, NULL);
}
 
//注销互斥锁
CMutex::~CMutex()
{
	pthread_mutex_destroy(&m_mutex);
}
 
//确保拥有互斥锁的线程对被保护资源的独自访问
void CMutex::Lock() const
{
	pthread_mutex_lock(&m_mutex);
}
 
//释放当前线程拥有的锁，以使其它线程可以拥有互斥锁，对被保护资源进行访问
void CMutex::Unlock() const
{
	pthread_mutex_unlock(&m_mutex);
}
 
//利用C++特性，进行自动加锁
CMyLock::CMyLock(const ILock& m) : m_lock(m)
{
	m_lock.Lock();
}
 
//利用C++特性，进行自动解锁
CMyLock::~CMyLock()
{
	m_lock.Unlock();
}

对该类锁的测试：
// pthread_mutex.cpp : 定义控制台应用程序的入口点。
//
 
#include <iostream>
#include <unistd.h>
#include "Lock.h"
 
using namespace std;
 
//创建一个互斥锁
CMutex g_Lock;
 
 
//线程函数
void * StartThread(void *pParam)
{
	char *pMsg = (char *)pParam;
	if (!pMsg)
	{
		return (void *)1;
	}
 
	//对被保护资源（以下打印语句）自动加锁
	//线程函数结束前，自动解锁
	CMyLock lock(g_Lock);
 
	for( int i = 0; i < 5; i++ )
	{
		cout << pMsg << endl;
		sleep( 1 );
	}
 
	return (void *)0;
}
 
int main(int argc, char* argv[])
{
	pthread_t thread1,thread2;
	pthread_attr_t attr1,attr2;
 
	char *pMsg1 = "First print thread.";
	char *pMsg2 = "Second print thread.";
 
	//创建两个工作线程，分别打印不同的消息
	pthread_attr_init(&attr1);
	pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);
	if (pthread_create(&thread1,&attr1, StartThread,pMsg1) == -1)
	{
		cout<<"Thread 1: create failed"<<endl;
	}
	pthread_attr_init(&attr2);
	pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_JOINABLE);
	if (pthread_create(&thread2,&attr2, StartThread,pMsg2) == -1)
	{
		cout<<"Thread 2: create failed"<<endl;
	}
 
	//等待线程结束
	void *result;
	pthread_join(thread1,&result);
	pthread_join(thread2,&result);
 
	//关闭线程，释放资源
	pthread_attr_destroy(&attr1);
	pthread_attr_destroy(&attr2);
 
 
	return 0;
}



Mempool.h
----
#include "MemPool.h"
 
CMemPool::CMemPool(std::size_t blockSize, int preAlloc, int maxAlloc):
m_blockSize(blockSize),
m_maxAlloc(maxAlloc),
m_allocated(preAlloc)
{
	if ( preAlloc < 0 || maxAlloc == 0 || maxAlloc < preAlloc )
	{
		cout<<"CMemPool::CMemPool parameter error."<<endl;
	}
 
	int r = BLOCK_RESERVE;
	if (preAlloc > r)
		r = preAlloc;
	if (maxAlloc > 0 && maxAlloc < r)
		r = maxAlloc;
	//test
	cout << "CMemPool reserve:" << r << endl;
	m_blocks.reserve(r);
	for (int i = 0; i < preAlloc; ++i)
	{
		m_blocks.push_back(new char[m_blockSize]);
	}
}
 
 
CMemPool::~CMemPool()
{
	for (BlockVec::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		delete [] *it;
	}
}
 
 
void* CMemPool::Get()
{
	//多线程环境下保证线程安全
	CMyLock lock(m_mutex);
 
	if (m_blocks.empty())
	{
		if (m_maxAlloc == 0 || m_allocated < m_maxAlloc)
		{
			++m_allocated;
			return new char[m_blockSize];
		}
		else
		{
			cout<<"CMemPool::get CMemPool exhausted."<<endl;
			return (void *)NULL;
		}
	}
	else
	{
		char* ptr = m_blocks.back();
		m_blocks.pop_back();
		return ptr;
	}
}
 
 
void CMemPool::Release(void* ptr)
{   
	//多线程环境下保证线程安全
	CMyLock lock(m_mutex);
 
	m_blocks.push_back(reinterpret_cast<char*>(ptr));
}


MemPool.cpp
-----
#include "MemPool.h"
 
CMemPool::CMemPool(std::size_t blockSize, int preAlloc, int maxAlloc):
m_blockSize(blockSize),
m_maxAlloc(maxAlloc),
m_allocated(preAlloc)
{
	if ( preAlloc < 0 || maxAlloc == 0 || maxAlloc < preAlloc )
	{
		cout<<"CMemPool::CMemPool parameter error."<<endl;
	}
 
	int r = BLOCK_RESERVE;
	if (preAlloc > r)
		r = preAlloc;
	if (maxAlloc > 0 && maxAlloc < r)
		r = maxAlloc;
	//test
	cout << "CMemPool reserve:" << r << endl;
	m_blocks.reserve(r);
	for (int i = 0; i < preAlloc; ++i)
	{
		m_blocks.push_back(new char[m_blockSize]);
	}
}
 
 
CMemPool::~CMemPool()
{
	for (BlockVec::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		delete [] *it;
	}
}
 
 
void* CMemPool::Get()
{
	//多线程环境下保证线程安全
	CMyLock lock(m_mutex);
 
	if (m_blocks.empty())
	{
		if (m_maxAlloc == 0 || m_allocated < m_maxAlloc)
		{
			++m_allocated;
			return new char[m_blockSize];
		}
		else
		{
			cout<<"CMemPool::get CMemPool exhausted."<<endl;
			return (void *)NULL;
		}
	}
	else
	{
		char* ptr = m_blocks.back();
		m_blocks.pop_back();
		return ptr;
	}
}
 
 
void CMemPool::Release(void* ptr)
{   
	//多线程环境下保证线程安全
	CMyLock lock(m_mutex);
 
	m_blocks.push_back(reinterpret_cast<char*>(ptr));
}


CMyMemPool.cpp
----
// CMyMemPool.cpp : 定义控制台应用程序的入口点。
//
 
#include <iostream>
#include<ctime>
#include "MemPool.h"
using namespace std;
clock_t start,end;

#define DATA_BLOCK_LEN 1024*1024
 
int main()
{
#if 0
	CMemPool myPool1(DATA_BLOCK_LEN, 0, 10);
 
	cout<<"myPool1 block size = "<<myPool1.BlockSize()<<endl;
	cout<<"myPool1 allocated block num = "<<myPool1.Allocated()<<endl;
	cout<<"myPool1 available block num = "<<myPool1.Available()<<endl<<endl;
 
	std::vector<void*> ptrs;
	for (int i = 0; i < 10; ++i)
	{
		ptrs.push_back(myPool1.Get());
	}
 
	myPool1.Get();
 
	for (std::vector<void*>::iterator it = ptrs.begin(); it != ptrs.end(); ++it)
	{
		myPool1.Release(*it);
		cout<<"myPool1 available block num = "<<myPool1.Available()<<endl;
	}
 
	CMemPool myPool2(DATA_BLOCK_LEN, 5, 10);
	cout<<endl<<"myPool2 block size = "<<myPool2.BlockSize()<<endl;
	cout<<"myPool2 allocated block num = "<<myPool2.Allocated()<<endl;
	cout<<"myPool2 available block num = "<<myPool2.Available()<<endl;
 
#endif 
	std::vector<void*> array;
	array.reserve(1000);

	start=clock();		//程序开始计时
	for(int j=0;j<500;++j) {
		for(int i=0;i<1000;++i) {
			array[i] =(void*) new char[DATA_BLOCK_LEN];
		}
		for(int i=0;i<1000;++i) {
			delete [] (char*)array[i];
		}
	}

	end=clock();		//程序结束用时
	double endtime=(double)(end-start)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位
	

	//分配1000块内存,每个块DATA_BLOCK_LEN= 1024*10 bytes
	CMemPool myPool2(DATA_BLOCK_LEN, 1000, 1000);
	start=clock();		//程序开始计时
	for(int j=0;j<500;++j) {
		for(int i=0;i<1000;++i) {
			array[i] =(void*) myPool2.Get();
		}
		for(int i=0;i<1000;++i) {
			//delete (char*);
			myPool2.Release(array[i]);
		}
	}

	end=clock();		//程序结束用时
    endtime=(double)(end-start)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位

	return 0;
}
