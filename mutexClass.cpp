//用c++简单的封装线程中互斥锁
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h> 
 
//用c++简单的封装互斥锁
class MutexLock {
public:
	MutexLock() { 
		pthread_mutex_init(&mutex, NULL);   //在构造函数中直接初始化互斥锁
	}
	~MutexLock() {
		pthread_mutex_destroy(&mutex); 
	}
	inline void lock() {
		pthread_mutex_lock(&mutex);
	}
	inline void unlock() {
		pthread_mutex_unlock(&mutex);
	}
private:
	pthread_mutex_t mutex;
};
 
//全局变量
int a = 0;
//创建锁对象
MutexLock lock;
 
//线程回调函数
void * thread1(void * arg)
{
	while(1)
	{
		lock.lock();
		a++;
		printf("thread1-a == %d\n",a);
		lock.unlock();
		sleep(1);
	}
}
 
void * thread2(void *arg)
{
	while(1)
	{
		lock.lock();
		a++;
		printf("thread2-a == %d\n",a);
		lock.unlock();
		sleep(2);
	}
}
int main(int argc,char * argv[])
{
	//创建两个线程
	pthread_t test1;
    pthread_t test2;
	
 
	pthread_create(&test1,NULL,thread1,NULL);
	pthread_create(&test2,NULL,thread2,NULL);
	
	pthread_join(test1,NULL);
	pthread_join(test2,NULL);
	return 0;
}
