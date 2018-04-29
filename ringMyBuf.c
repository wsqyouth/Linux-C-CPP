将一个回合所操作的数据看成一对。
这里每次操作2个数据，space信号量为4，那么生产者可以生产4次，此时满则等待
消费者每次消费1次，同时给生产者留出1次的操作。
也就是项目中每次1k触发，环形缓冲区设为4k的意思。

同时这里面利用in,out的与特性解决了缓冲区索引溢出问题，只是操作必须为2的倍数。
我们可以在消费者中添加sleep验证二者速率不匹配时的数据操作情况：
极端情况：生产者生产速率大于消费者，此时生产者填充缓冲区满，只能等待消费者消费一次，他才填充一次
比如下面：生产者填充值3495，然后只有等待消费者消费了之前的数据3492，他才能填充他的下一个数3496
++++3495 at end 6
3492 at 0
++++3496 at end 0
3493 at 2
++++3497 at end 2
3494 at 4
++++3498 at end 4
3495 at 6
++++3499 at end 6
3496 at 0
++++3500 at end 0
---

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

// N must be 2^i
#define N (8)

int b[N];

int in = 0;
int out = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t countsem,spacesem;

void init() {
  sem_init(&countsem,0,0);
  sem_init(&spacesem,0,N/2);
}

void* thread_write(){
    int value=0;
while(1)
    { 
	
	 
	 // wait if there is no space left:
	 sem_wait(&spacesem);
	 pthread_mutex_lock(&lock);
     value++;
	 for(int i=0;i<2;i++)
	 	b[(in++)&(N-1)] = value;
	 pthread_mutex_unlock(&lock);
	 printf("++++%d at end %d\n",value,(in)&(N-1)); 
	 // increment the count of the number of items
	 sem_post(&countsem);
	 //sleep(1); 
}
}

void *thread_read(){
	int result;
while(1)
    {  

  // Wait if there are no items in the buffer
  sem_wait(&countsem);

  pthread_mutex_lock(&lock);
	for(int i=0;i<2;i++)
  		 result = b[(out++) & (N-1)];
  pthread_mutex_unlock(&lock);
  printf("%d at %d\n",result,(out) & (N-1));
  // Increment the count of the number of spaces
  sem_post(&spacesem);
  sleep(0.1); 
}
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
