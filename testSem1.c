无名信号量在多线程间的同步
无名信号量的常见用法是将要保护的变量放在 sem_wait 和 sem_post 中间所形成的临界区内，这样该变量就会被保护起来

---
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int number; //被保护的全局变量

sem_t sem_id;

void * thread_one_fun(void *arg)
{
	sem_wait(&sem_id);
	printf("thread_one have the semaphore\n");
	number++;
	printf("number = %d\n",number);
	sem_post(&sem_id);
}

void * thread_two_fun(void *arg)
{
	sem_wait(&sem_id);
	printf("thread_two have the semaphore\n");
	number--;
	printf("number = %d\n",number);
	sem_post(&sem_id);
} 

int main(int argc,char *argv[])
{
	number = 1;
	pthread_t id1,id2;
	sem_init(&sem_id,0,1);
	pthread_create(&id1,NULL,thread_one_fun,NULL);
	pthread_create(&id2,NULL,thread_two_fun,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	printf("main...\n");
	return 0;
}
