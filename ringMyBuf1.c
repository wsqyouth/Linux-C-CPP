
如果需要参考标准生产者消费者模型，建议参考csapp中并发模型。(生产者信号量为缓冲区元素个数，每次仅操作1个元素)

这里测试紧紧结合工程的模型，只是将真正的数据以一个非常小的数据buf代替。
生产者消费者对共享内存操作后，即打印该共享缓冲区，以此查看每次操作后的情况。

这里由于考虑到一个标准生产者消费者的实现，因此生产者消费者之间通过全局变量进行交互：

生产者: 自定义数据pSendBuf+该数据实际长度----->填充共享缓冲区(临界区)
消费者：共享缓冲区(临界区)---->取出本次操作数据，消费完清空。

该程序需要考虑的问题是:若某次操作的数据并不是一个操作区域（单位1）的大小，该怎么办
-----

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

//(可修改)这里为了测试方便，设置content_buf为1*4个字节，实际运行时修改为1024*4即可
//(可修改)每次操作数据为12字节，共享buf开辟空间为12*4字节，也就是说读写指针一次循环为4次
//(可修改)这里生产者信号量设置为1,那麽此时生产1个，消费1个，后面可以修改为4  N/M
//此时，生产者可以多生产几次，直到缓冲区被填满，消费者留出缓冲区再生产

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;


typedef struct {
    u8 *buf;    // Buffer array
    int n;       // Maximum number of slots
    int front;   // buf[(front+1)%n] is first item
    int rear;    // buf[rear%n] is the last item
    sem_t mutex; // Protects accesses to buf
    sem_t slots; // Counts available slots
    sem_t items; // Counts available items
} sbuf_t;


sbuf_t sp;
void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, u8  *buf,int n);
void sbuf_remove(sbuf_t *sp);


u8 *pSendBuf;
int iLen;//每次从零开始计数

void sbuf_init(sbuf_t *sp, int n) {
    sp->buf = (u8*)calloc(n,sizeof(u8)); //unsafe
	if(sp->buf==NULL)
	{
		printf("null\n");
		return ;
	}
    sp->n = n;                  // Buffer holds max of n items
    sp->front = sp->rear = 0;   // Empty buffer iff front == rear
    sem_init(&sp->mutex, 0, 1); // Binary semaphore for locking
    sem_init(&sp->slots, 0, 1); // Initially, buf has 1 empty slots
    sem_init(&sp->items, 0, 0); // Initially, buf has 0 items
}

// Clean up buffer sp
void sbuf_deinit(sbuf_t *sp){
    free(sp->buf);
}

void printData(u8  *buf,int n)
{
	for(int i=0;i<n;i++)
		printf("%x ",buf[i]);
	printf("\n");
}
// Insert item onto the rear of shared buffer sp
void sbuf_insert(sbuf_t *sp, u8  *buf,int n) {
    sem_wait(&sp->slots);                        // Wait for available slot
    sem_wait(&sp->mutex);                       // Lock the buffer		
	// Insert the item
	if(buf != NULL)
	{
		printf("+++++ insert at %d \n",sp->rear);		
		memcpy(&(sp->buf[sp->rear]),buf,n); //每次拷贝至初始位置,n个字节
		printData(sp->buf,sp->n);
		sp->rear = (sp->rear+n) %(sp->n);
		
	}
    sem_post(&sp->mutex);                        // Unlock the buffer
    sem_post(&sp->items);                        // Announce available item
}

// Remove and return the first tiem from the buffer sp
void sbuf_remove(sbuf_t *sp) {
    
    sem_wait(&sp->items);                         // Wait for available item
    sem_wait(&sp->mutex);                         // Lock the buffer
    if(sp->buf != NULL)
	{
		printf("--- remove at %d\n",sp->front);
		//send nbytes;
		int sentBytes = 12;
		//清空已发送区域
		memset(&(sp->buf[sp->front]),0,sentBytes*sizeof(u8));
		printData(sp->buf,sp->n);
		sp->front = (sp->front+sentBytes)%(sp->n);
		
	}
    sem_post(&sp->mutex);                         // Unlock the buffer
    sem_post(&sp->slots);                         // Announce available slot
    
}

void *thread_write(){
	int value=0;
while(1){  
		value++;
		sbuf_insert(&sp,pSendBuf,iLen);
		//printf("+++insert %d\n",value);
		//sleep(1);
	}
}

void *thread_read(){
	int result=0;
while(1){  
		result++;
		sbuf_remove(&sp); 
		//printf("---remove %d\n",result);
		sleep(1);
	}
}

int main()
{
	
	pthread_t wtid, rtid;  
	sbuf_init(&sp,12*4);
	
	pSendBuf = (u8 *)malloc(8000);  //开辟SendBuf
	iLen = 0;//每次从零开始计数
    //实际开辟1*4
	int8_t content_buf[1*4] = {0};
	//7E7E  len  content_buf 0D0A
	
	pSendBuf[0] = 0x7e; pSendBuf[1] = 0x7e;   //7E 7E 
	//整包长度，４字节 pSendBuf[2]  pSendBuf[3] pSendBuf[4] pSendBuf[5]
	*(u32*)(&pSendBuf[2]) = 12; 

	iLen += 6; //head + len
	memcpy(pSendBuf+iLen,content_buf, sizeof(content_buf));
	iLen +=sizeof(content_buf);
	*(u16*)(&pSendBuf[iLen]) = 0x0a0d; //帧尾 0D 0A
	iLen += 2;
	
	pthread_create(&wtid, NULL, thread_write, NULL);  
    pthread_create(&rtid, NULL, thread_read, NULL);  
    pthread_exit(NULL);  
	
	sbuf_deinit(&sp);

	return 0;
}

---
输出：
pp@pp:~/mycode/ringbuf$  gcc ringMy1.c -o ringMy1 -lpthread
pp@pp:~/mycode/ringbuf$ ./ringMy1 
+++++ insert at 0 
7e 7e c 0 0 0 0 0 0 0 d a 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
--- remove at 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
+++++ insert at 12 
0 0 0 0 0 0 0 0 0 0 0 0 7e 7e c 0 0 0 0 0 0 0 d a 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
--- remove at 12
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
+++++ insert at 24 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 7e 7e c 0 0 0 0 0 0 0 d a 0 0 0 0 0 0 0 0 0 0 0 0 
--- remove at 24
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
+++++ insert at 36 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 7e 7e c 0 0 0 0 0 0 0 d a 
--- remove at 36
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
+++++ insert at 0 
7e 7e c 0 0 0 0 0 0 0 d a 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
--- remove at 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
+++++ insert at 12 
0 0 0 0 0 0 0 0 0 0 0 0 7e 7e c 0 0 0 0 0 0 0 d a 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
--- remove at 12
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
