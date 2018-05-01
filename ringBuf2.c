
//本代码参考https://www.cnblogs.com/xybaby/p/6516387.html#_label_7，实现两种方法的生产者消费者模型

------
基于资源信号量+互斥信号量
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFF_SIZE  3
#define PRODUCE_THREAD_SIZE 1
int g_buff[BUFF_SIZE];
int g_write_index = 0;
int g_read_index = 0;

sem_t lock;
sem_t consume_sem, produce_sem;


void* produce(void *ptr){
    int idx = *(int*)ptr;
    
    while(1){
        sem_wait(&produce_sem);// 限制了生产者并发的数目

		sem_wait(&lock); // 对临界区的访问要加锁
        g_buff[g_write_index] = idx;
        g_write_index = (g_write_index + 1) % BUFF_SIZE;
		printf("in produce %d ,write_index=%d read_index=%d\n",idx, g_write_index, g_read_index);
        sem_post(&lock);

        sem_post(&consume_sem);
		sleep(1);
    }
    return NULL;
}

void* consume(void *ptr){
    while(1){
        sem_wait(&consume_sem);
        sem_wait(&lock);
        int data = g_buff[g_read_index];
        g_buff[g_read_index] = -1;
        g_read_index = (g_read_index + 1) % BUFF_SIZE;
        printf("consume %d read_index=%d write_index=%d\n", data, g_read_index, g_write_index);
        sem_post(&lock);
        sem_post(&produce_sem);
		sleep(1);
    }
    return NULL;
}

int main(int argc, char * argv[]){
    pthread_t con;
    pthread_t pros[PRODUCE_THREAD_SIZE];
    sem_init(&lock, 0, 1);
    sem_init(&consume_sem,0, 0);
    sem_init(&produce_sem,0, BUFF_SIZE);

    pthread_create(&con, NULL, consume, NULL);
    int thread_args[PRODUCE_THREAD_SIZE];
    for(int i = 0; i < PRODUCE_THREAD_SIZE; i++){
        thread_args[i] = i + 1;
        pthread_create(&pros[i], NULL, produce, (thread_args + i));
    }

    pthread_join(con,0);
    for(int i = 0; i < PRODUCE_THREAD_SIZE; i++)
        pthread_join(pros[i],0);

    sem_destroy(&lock);
    sem_destroy(&consume_sem);
    sem_destroy(&produce_sem);

    return 0;
}



------
基于管程（互斥锁+条件变量）来实现
https://blog.csdn.net/u010424605/article/details/42872189
http://anzhsoft.iteye.com/blog/2031354


永远在循环（loop）里调用 wait 和 notify，不是在 If 语句
现在你知道wait应该永远在被synchronized的背景下和那个被多线程共享的对象上调用，
下一个一定要记住的问题就是，你应该永远在while循环，而不是if语句中调用wait。
因为线程是在某些条件下等待的——在我们的例子里，即“如果缓冲区队列是满的话，那么生产者线程应该等待”，
你可能直觉就会写一个if语句。但if语句存在一些微妙的小问题，导致即使条件没被满足，你的线程你也有可能被错误地唤醒。
所以如果你不在线程被唤醒后再次使用while循环检查唤醒条件是否被满足，你的程序就有可能会出错——
例如在缓冲区为满的时候生产者继续生成数据，或者缓冲区为空的时候消费者开始小号数据。所以记住，永远在while循环而不是if语句中使用wait
https://blog.csdn.net/u011863767/article/details/59731447
-----
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


#define BUFF_SIZE  3
#define PRODUCE_THREAD_SIZE 1
int g_buff[BUFF_SIZE];
int g_write_index = 0;
int g_read_index = 0;

pthread_mutex_t lock;
pthread_cond_t consume_cond, produce_cond;


void* produce(void *ptr){
    int idx = *(int*)ptr;
    
    while(1){
        pthread_mutex_lock(&lock);
        while((g_write_index + 1)% BUFF_SIZE  == g_read_index)  //
            pthread_cond_wait(&produce_cond, &lock);

        g_buff[g_write_index] = idx;
        g_write_index = (g_write_index + 1) % BUFF_SIZE;
		printf("in produce %d ,write_index=%d read_index=%d\n",idx, g_write_index, g_read_index);
        pthread_cond_signal(&consume_cond);
        pthread_mutex_unlock(&lock);

		sleep(1);

    }
    return NULL;
}

void* consume(void *ptr){
    while(1){
        pthread_mutex_lock(&lock);
        while(g_read_index == g_write_index)
             pthread_cond_wait(&consume_cond, &lock);

        int data = g_buff[g_read_index];
        g_buff[g_read_index] = -1;
        g_read_index = (g_read_index + 1) % BUFF_SIZE;
        printf("consume %d read_index=%d write_index=%d\n", data, g_read_index, g_write_index);

        pthread_cond_signal(&produce_cond);
        pthread_mutex_unlock(&lock);

		sleep(1);
    }
    return NULL;
}

int main(int argc, char * argv[]){
    pthread_t con;
    pthread_t pros[PRODUCE_THREAD_SIZE];

    srand((unsigned)time(NULL));
    pthread_mutex_init(&lock, 0);
    pthread_cond_init(&consume_cond,0);
    pthread_cond_init(&produce_cond,0);

    pthread_create(&con, NULL, consume, NULL);
    int thread_args[PRODUCE_THREAD_SIZE];
    for(int i = 0; i < PRODUCE_THREAD_SIZE; i++){
        thread_args[i] = i + 1;
        pthread_create(&pros[i], NULL, produce, (thread_args + i));
    }

    pthread_join(con,0);
    for(int i = 0; i < PRODUCE_THREAD_SIZE; i++)
        pthread_join(pros[i],0);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&consume_cond);
    pthread_cond_destroy(&produce_cond);

    return 0;
}
