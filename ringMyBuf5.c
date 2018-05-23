//参考ubp卷2对ringMyBuf2.c进行修改，添加结构体和互斥变量。

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//12*4
#define SINGLESIZE 12 
#define SEMSIZE 4
#define RINGSIZE 48

struct ringbuf_struct
{
	u8 buf[RINGSIZE];   //data shared by producer and consumer 
	sem_t sem_pro;      //producer semaphores
  sem_t sem_con;      //consumer semaphores
	sem_t mutex;        //mutex
}ringshared;


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
        sem_wait(&ringshared.sem_pro);
		sem_wait(&ringshared.mutex);

	    memcpy(&(ringshared.buf[proIndex]),pSendBuf,len); //每次拷贝至写指针当前位置,len个字节
        printf("product at %d\n",proIndex);
		//printData(buf,RINGSIZE);
		//步进长度SINGLESIZE=12
        proIndex = (proIndex+SINGLESIZE)%RINGSIZE;
      
		sem_post(&ringshared.mutex);
        sem_post(&ringshared.sem_con);
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
        //存在数据则消费,不存在数据则阻塞,直到有数据为止
        sem_wait(&ringshared.sem_con);
    	sem_wait(&ringshared.mutex);
        printf("consume  at %d\n",conIndex);
		//判断是否为12字节，若不是则丢弃(默认继续消费)
		int sentBytes = SINGLESIZE;
	
		if(sentBytes <0){
		   break;
		}
		//清零操作
		memset(&(ringshared.buf[conIndex]),0,SINGLESIZE*sizeof(u8));//直接清空该区域
		//printData(buf,RINGSIZE);
	
        conIndex = (conIndex+SINGLESIZE)%RINGSIZE;
        //消费了一个数据,空位加1
		sem_post(&ringshared.mutex);
        sem_post(&ringshared.sem_pro);

	 	//sleep(1);
    }
    pthread_exit(NULL);
}

int main()
{
	
    pthread_t pro,con;
    //initialize three semaphores 
	sem_init(&ringshared.mutex,0,1);    
    sem_init(&ringshared.sem_pro,0,SEMSIZE);     
    sem_init(&ringshared.sem_con,0,0);          

    pthread_create(&pro,NULL,productor,NULL);
    pthread_create(&con,NULL,consumer,NULL);
    
    while(1)
	{
		sleep(3);
	}
	sem_destroy(&ringshared.mutex);
    sem_destroy(&ringshared.sem_pro);
    sem_destroy(&ringshared.sem_con);
    return 0;
}
