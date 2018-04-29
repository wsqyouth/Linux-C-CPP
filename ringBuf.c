
仅用了信号量实现同步，并没有实现互斥。
每次仅仅进行一次操作，blank_sem定义为SIZE大小
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 8
int ring[SIZE];//创建数组，存放数据


 sem_t blank_sem;//定义信号量，表示空格子量
 sem_t data_sem;//定义信号量，表示格子中含有数据的个数

void *product( void *arg)//生产者
{
    int data=0;
    int step=0;
    while(1)
    {
        int data =rand( )%1234;
        sem_wait(&blank_sem);//空格子减1
        ring[step]=data;// 放入数据
        sem_post(&data_sem);//格子中含有数据的个数加1
        printf("The product done:%d at %d\n",data,step);
        step++;
        step%=SIZE;
       sleep(1); 
   }
}
void *consume( void *arg)
{
    int step=0;
    while(1)
    { 
       int data=-1;//消费失败就会输出-1
       sem_wait(&data_sem);//格子中含有数据的个数减1
       data=ring[step];//拿出数据
       sem_post(&blank_sem);//空格子数加1
       printf("The consume done:%d\n",data);

       step++;
       step%=SIZE;

    }

}
int main( )
{
     pthread_t p;
     pthread_t c;

    pthread_create(&p,NULL,product,NULL);
    pthread_create(&c,NULL,consume,NULL);

    sem_init(&blank_sem,0,SIZE);//初始化信号量，刚开始的空格子数为SIZE大小
    sem_init(&data_sem,0,0);//初始化信号量，刚开始的格子中含有数据的个数为0

    pthread_join(p,NULL);
    pthread_join(c,NULL);

    sem_destroy(&blank_sem);//销毁信号量
    sem_destroy(&data_sem);


   return 0;    
}

输出：
The product done:1219 at 0
The consume done:1219
The product done:900 at 1
The consume done:900
The product done:45 at 2
The consume done:45
The product done:85 at 3
The consume done:85
The product done:623 at 4
The consume done:623
The product done:241 at 5
The consume done:241
The product done:636 at 6
The consume done:636
The product done:1212 at 7
The consume done:1212
The product done:1049 at 0
The consume done:1049
The product done:19 at 1
The consume done:19
The product done:98 at 2
The consume done:98


-----------------------------------

 该代码使用了&操作而不是%操作，并且后面可考虑溢出问题(unsigned int in.out)
 使用了信号量和互斥锁的方法，当有互斥锁时，写入和读取不能同时操作，这样互斥很安全，但符合功能需要吗
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
//https://www.cnblogs.com/rechen/p/5143841.html
//https://github.com/angrave/SystemProgramming/wiki/Synchronization%2C-Part-8%3A-Ring-Buffer-Example


// N must be 2^i
#define N (8)

int b[N];

int in = 0;
int out = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t countsem,spacesem;

void init() {
  sem_init(&countsem,0,0);
  sem_init(&spacesem,0,N);
}

void* thread_write(){
    int value=0;
while(1)
    { 
	 value++;
	 if(value == N)
		value = 0;
	 // wait if there is no space left:
	 sem_wait(&spacesem);
	 
	
	 pthread_mutex_lock(&lock);
	 b[(in++)&(N-1)] = value;
	 pthread_mutex_unlock(&lock);
	 printf("++++%d\n",value); 
	 // increment the count of the number of items
	 sem_post(&countsem);
}
}

void *thread_read(){
while(1)
    {  

  // Wait if there are no items in the buffer
  sem_wait(&countsem);

  pthread_mutex_lock(&lock);
  int result = b[(out++) & (N-1)];
  pthread_mutex_unlock(&lock);
  printf("%d\n",result);
  // Increment the count of the number of spaces
  sem_post(&spacesem);
}
  //return result;
}

int main()
{	
init();
	pthread_t wtid, rtid;  
	pthread_create(&wtid, NULL, thread_write, NULL);  
    pthread_create(&rtid, NULL, thread_read, NULL);  
    pthread_exit(NULL);  
	return 0;

}


