
本文参考自：

原代码：基于环形队列的生产者消费者模型
----
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 4
//环形队列
int arr[SIZE] = {0};
sem_t sem_pro;      //描述环形队列中的空位置
sem_t sem_con;      //描述唤醒队列中的数据
//生产者，只要环形队列有空位,便不断生产
void*productor(void*arg){
    int data = 0;
    int proIndex = 0;
    while(1){
        //有空位便生产,没空位便阻塞等消费者消费
        sem_wait(&sem_pro);
        data = rand()%1234;
        arr[proIndex] = data;
        printf("product done %d at %d\n",data,proIndex);
        proIndex = (proIndex+1)%SIZE;
        //供消费者消费的数据加1
        sem_post(&sem_con);
    }
}
//消费者,只要环形队列中有数据,就不断消费
void*consumer(void*arg){
    int data = 0;
    int conIndex = 0;
    while(1){
        //环形队列中存在数据则消费,不存在数据则阻塞,直到有数据为止
        sem_wait(&sem_con);
        data = arr[conIndex];
        printf("consume done %d at %d\n",data,conIndex);
        conIndex = (conIndex+1)%SIZE;
        //最后,消费了一个数据,空位加1
        sem_post(&sem_pro);

	sleep(1);
    }
}
 
int main(){
    pthread_t pro,con;
    sem_init(&sem_pro,0,SIZE);        //一开始有很多空位置
    sem_init(&sem_con,0,0);         //但并没有数据
 
    pthread_create(&pro,NULL,productor,NULL);
    pthread_create(&con,NULL,consumer,NULL);
    pthread_join(pro,NULL);
    pthread_join(con,NULL);
 
    sem_destroy(&sem_pro);
    sem_destroy(&sem_con);
    return 0;
}



-----
我修改的工程代码（每个块大小为12，共4块）：
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

//12*4
#define SINGLESIZE 12 
#define SEMSIZE 4
#define RINGSIZE 48
//环形队列
u8 arr[RINGSIZE] = {0};
sem_t sem_pro;      //描述环形队列中的空位置
sem_t sem_con;      //描述唤醒队列中的数据


void printData(u8  *buf,int n)
{
	for(int i=0;i<n;i++)
		printf("%x ",buf[i]);
	printf("------\n");
}


int InsertBuf(u8 *pSendBuf) {
  	int iLen = 0;//每次从零开始计数
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
	return iLen;			
}

//生产者，只要环形队列有空位,便不断生产
void* productor(void*arg){
    //该子线程的状态设置为分离的
    pthread_detach(pthread_self());

    int proIndex = 0;
    //u8 *pSendBuf = (u8 *)malloc(5000); //开辟SendBuf
    u8 pSendBuf[5000]={0};
    while(1){
	//AD9361拼装数据
	int len = InsertBuf(pSendBuf);
	//判断是否为12字节，若不是continue丢弃
	if(SINGLESIZE != len)
	{
	  continue;
	}
        //有空位便生产,没空位便阻塞等消费者消费
        sem_wait(&sem_pro);
	memcpy(&(arr[proIndex]),pSendBuf,len); //每次拷贝至写指针当前位置,len个字节
        printf("product at %d\n",proIndex);
	//printData(arr,RINGSIZE);
	//步进长度SINGLESIZE=12
        proIndex = (proIndex+SINGLESIZE)%RINGSIZE;
        //供消费者消费的数据加1
        sem_post(&sem_con);
	//sleep(1);
    }
    //free(pSendBuf);//free
    pthread_exit(NULL);
}


//消费者,只要环形队列中有数据,就不断消费
void* consumer(void*arg){
    //该子线程的状态设置为分离的
    pthread_detach(pthread_self());
    int conIndex = 0;
    while(1){
        //环形队列中存在数据则消费,不存在数据则阻塞,直到有数据为止
        sem_wait(&sem_con);
    
        printf("consume  at %d\n",conIndex);
	//判断是否为12字节，若不是则丢弃(默认继续消费)
	int sentBytes = SINGLESIZE;
	
	if(sentBytes <0){
	   break;
	}
	//清零操作
	memset(&(arr[conIndex]),0,SINGLESIZE*sizeof(u8));//直接清空该区域
	//printData(arr,RINGSIZE);
	
        conIndex = (conIndex+SINGLESIZE)%RINGSIZE;
        //最后,消费了一个数据,空位加1
        sem_post(&sem_pro);

	//sleep(1);
    }
    pthread_exit(NULL);
}

int main()
{
	
    pthread_t pro,con;
    sem_init(&sem_pro,0,SEMSIZE);      //一开始有很多空位置
    sem_init(&sem_con,0,0);         //但并没有数据
    pthread_create(&pro,NULL,productor,NULL);
    pthread_create(&con,NULL,consumer,NULL);
    
    while(1);
    sem_destroy(&sem_pro);
    sem_destroy(&sem_con);
    return 0;
}


----
测试对一维数组操作代码：
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;


void printData(u8  *buf,int n)
{
	for(int i=0;i<n;i++)
		printf("%x ",buf[i]);
	printf("\n");
}


int InsertBuf(u8 *pSendBuf) {
  	int iLen = 0;//每次从零开始计数
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
	return iLen;			
}
int main()
{

   u8 *pSendBuf = (u8 *)malloc(8000); //开辟SendBuf
   int len = InsertBuf(pSendBuf);
   printData(pSendBuf,len);
   free(pSendBuf);
   return 0;
}

valgrind测试代码内存泄露：
/home/smile/software/valgrind/bin/valgrind  --track-fds=yes --leak-check=full --show-reachable=yes -v
--read-var-info=yes --num-callers=10 --leak-resolution=high   --trace-children=yes ./可执行应用程序
