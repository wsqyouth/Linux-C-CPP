sem_t sem; 
定义一个信号量变量。使用时需首先使用sem_init()函数初始化。 
在多线程编程中，想让某个线程阻塞等待，减少cpu占用，在该需要运行时才运行。
使用信号量一个A线程sem_wait(),阻塞等待；
一个B线程在需要运行A线程时sem_post(),解除A线程阻塞。
----
#include <stdint.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

sem_t sem;

void *testfunc(void *arg)
{
    while(1)
    {
        sem_wait(&sem);
        //do something....
        printf("hello world...\n");
    }
}

int main()
{
    pthread_t ps;
    sem_init(&sem, 0, 0);
    pthread_create(&ps,NULL,testfunc,NULL);
    while(1)
    {
        //每隔一秒sem_post 信号量sem加1 子线程sem_wait解除等待 打印hello world
        sem_post(&sem);
        sleep(1);
    }
    return 0;
}
