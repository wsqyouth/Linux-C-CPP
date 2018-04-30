参考CSAPP代码，先调试产生第一个版本，其中生产者信号量槽数=buffer元素个数。
-----
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int *buf;    // Buffer array
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
void sbuf_insert(sbuf_t *sp, int item);
int  sbuf_remove(sbuf_t *sp);

// Create an empty, bounded, shared FIFO buffer with n slots
void sbuf_init(sbuf_t *sp, int n) {
    sp->buf = (int*)calloc(n,sizeof(int)); //unsafe
	if(sp->buf==NULL)
	{
		printf("null\n");
		return ;
	}
    sp->n = n;                  // Buffer holds max of n items
    sp->front = sp->rear = 0;   // Empty buffer iff front == rear
    sem_init(&sp->mutex, 0, 1); // Binary semaphore for locking
    sem_init(&sp->slots, 0, 2); // Initially, buf has n empty slots
    sem_init(&sp->items, 0, 0); // Initially, buf has 0 items
}
// Clean up buffer sp
void sbuf_deinit(sbuf_t *sp){
    free(sp->buf);
}
// Insert item onto the rear of shared buffer sp
void sbuf_insert(sbuf_t *sp, int item) {
    sem_wait(&sp->slots);                        // Wait for available slot
    sem_wait(&sp->mutex);                       // Lock the buffer
    sp->buf[(++sp->rear)%(sp->n)] = item; 		// Insert the item
    sem_post(&sp->mutex);                        // Unlock the buffer
    sem_post(&sp->items);                        // Announce available item
}
// Remove and return the first tiem from the buffer sp
int sbuf_remove(sbuf_t *sp) {
    int item;
    sem_wait(&sp->items);                         // Wait for available item
    sem_wait(&sp->mutex);                         // Lock the buffer
    item = sp->buf[(++sp->front)%(sp->n)]; 		 // Remove the item
    sem_post(&sp->mutex);                         // Unlock the buffer
    sem_post(&sp->slots);                         // Announce available slot
    return item;
}


void *thread_write(){
	int value=0;
while(1){  
		value++;
		sbuf_insert(&sp,value);
		printf("+++insert %d\n",value);
	}
}


void *thread_read(){
	int result;
while(1){  
		result = sbuf_remove(&sp); 
		printf("---remove %d\n",result);
	}
}
int main()
{
	
	pthread_t wtid, rtid;  
	sbuf_init(&sp,8);
	pthread_create(&wtid, NULL, thread_write, NULL);  
    pthread_create(&rtid, NULL, thread_read, NULL);  
    pthread_exit(NULL);  
	
	sbuf_deinit(&sp);
}


该版本添加一个成员变量，m=4,表示共4次可用信号量操作，每次操作size大小为2
-----
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int *buf;    // Buffer array
    int n;       // buffer  size
	int m;       // Maximum number of slots
    int front;   // buf[(front+1)%n] is first item
    int rear;    // buf[rear%n] is the last item
    sem_t mutex; // Protects accesses to buf
    sem_t slots; // Counts available slots
    sem_t items; // Counts available items
} sbuf_t;


sbuf_t sp;
void sbuf_init(sbuf_t *sp, int n,int m);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, int item);
int  sbuf_remove(sbuf_t *sp);

// Create an empty, bounded, shared FIFO buffer with n slots
void sbuf_init(sbuf_t *sp, int n,int m) {
    sp->buf = (int*)calloc(n,sizeof(int)); //unsafe
	if(sp->buf==NULL)
	{
		printf("null\n");
		return ;
	}
    sp->n = n;                  // Buffer holds max of n items
	sp->m = m;   				// Buffer every time operate size
    sp->front = sp->rear = 0;   // Empty buffer iff front == rear
    sem_init(&sp->mutex, 0, 1); // Binary semaphore for locking
    sem_init(&sp->slots, 0, (sp->n)/(sp->m)); // Initially, buf has n empty slots,every time operate m size
    sem_init(&sp->items, 0, 0); // Initially, buf has 0 items
}
// Clean up buffer sp
void sbuf_deinit(sbuf_t *sp){
    free(sp->buf);
}
// Insert item onto the rear of shared buffer sp
void sbuf_insert(sbuf_t *sp, int item) {
    sem_wait(&sp->slots);                        // Wait for available slot
    sem_wait(&sp->mutex);                       // Lock the buffer
	for(int i=0;i<(sp->m);++i)
   	   sp->buf[(++sp->rear)%(sp->n)] = item; 		// Insert the item
    sem_post(&sp->mutex);                        // Unlock the buffer
    sem_post(&sp->items);                        // Announce available item
}
// Remove and return the first tiem from the buffer sp
int sbuf_remove(sbuf_t *sp) {
    int item;
    sem_wait(&sp->items);                         // Wait for available item
    sem_wait(&sp->mutex);                         // Lock the buffer
	for(int i=0;i<(sp->m);++i)
    	item = sp->buf[(++sp->front)%(sp->n)]; 		 // Remove the item
    sem_post(&sp->mutex);                         // Unlock the buffer
    sem_post(&sp->slots);                         // Announce available slot
    return item;
}


void *thread_write(){
	int value=0;
while(1){  
		value++;
		sbuf_insert(&sp,value);
		printf("+++insert %d\n",value);
	}
}


void *thread_read(){
	int result;
while(1){  
		result = sbuf_remove(&sp); 
		printf("---remove %d\n",result);
	}
}
int main()
{
	
	pthread_t wtid, rtid;  
	sbuf_init(&sp,8,4); //4 avaiable slots
	pthread_create(&wtid, NULL, thread_write, NULL);  
        pthread_create(&rtid, NULL, thread_read, NULL);  
        pthread_exit(NULL);  
	
	sbuf_deinit(&sp);
}
