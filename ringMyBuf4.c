//参考《嵌入式Linux软件开发从入门到精通》
可以使用全局结构体变量，使用了互斥锁+条件变量+非正常终止情况
----
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;


//12*4
#define SINGLESIZE 12 
#define SEMSIZE 4
#define RINGSIZE 48


struct thread_param_struct *thread_param;  

struct loop_buf_struct
{
	char* buf;                //环形缓冲区内存
	unsigned int front;       //缓冲区读位置（消费者使用）
	unsigned int rear;        //缓冲区写位置（生产者使用）
	unsigned int size;     	  //缓冲区总大小
	unsigned int free;        //缓冲区空余大小
	pthread_mutex_t mutex;    //互斥锁，支持多线程安全
};

/*
*初始化缓冲区
*/
struct loop_buf_struct* loopbuf_init(unsigned int size)
{
	struct loop_buf_struct* loopbuf = malloc(sizeof(struct loop_buf_struct));
	if(loopbuf)
	{
		loopbuf->front=0;
		loopbuf->rear=0;
		loopbuf->size=size;
		loopbuf->buf=malloc(size);
		loopbuf->free=loopbuf->size;
		pthread_mutex_init(&loopbuf->mutex,NULL);
	}
	return loopbuf;
}

/*
*销毁缓冲区
*/
void loopbuf_destroy(struct loop_buf_struct *loopbuf)
{
	if(loopbuf->buf)
	{
		free(loopbuf->buf);
		loopbuf->buf=NULL;
	}

	pthread_mutex_destroy(&loopbuf->mutex);

	if(loopbuf)
	{
		free(loopbuf);
		loopbuf=NULL;
	}
}

/*
*把生产者生产的数据放入缓冲区中
*返回值：
*成功返回放入数据的长度，否则返回-1
*/
int loopbuf_produce(struct loop_buf_struct *loopbuf,char* data,unsigned int len)
{
	if (loopbuf ==NULL)
		return -1;
	if (loopbuf->free >= len)
	{
		if (loopbuf->rear + len > loopbuf->size)
		{
			unsigned int tofrontopy = loopbuf->size - loopbuf->rear;
			memcpy(loopbuf->buf+loopbuf->rear,data,tofrontopy);

			loopbuf->rear = 0;
			memcpy(loopbuf->buf,data+tofrontopy,len - tofrontopy);
			loopbuf->rear = len - tofrontopy;
		}
		else
		{
			memcpy(loopbuf->buf+loopbuf->rear,data,len);
			loopbuf->rear = loopbuf->rear + len;
		}

		pthread_mutex_lock(&loopbuf->mutex);
		loopbuf->free = loopbuf->free - len;
		pthread_mutex_unlock(&loopbuf->mutex);
	
		return len;
	}
	else
	{ return -1; }
}

/*
*消费者从缓冲区中消费数据
*返回值：
*成功返回0，否则返回-1
*/
int loopbuf_custom(struct loop_buf_struct *loopbuf,char* data,unsigned int len)
{
	if (loopbuf ==NULL||data == NULL || len<0)
		return -1;

	int length = len;

	if (loopbuf->size-loopbuf->free >= length )
	{
		if (loopbuf->front + length  > loopbuf->size)
		{//two times
			unsigned int tofrontopy = loopbuf->size - loopbuf->front;
			memcpy(data,loopbuf->buf+loopbuf->front,tofrontopy);
			loopbuf->front = 0;
			memcpy(data+tofrontopy,loopbuf->buf,length  - tofrontopy);
			loopbuf->front = length  - tofrontopy;
		}
		else
		{//one times
			memcpy(data,loopbuf->buf+loopbuf->front,length );
			loopbuf->front = loopbuf->front + length ;
		}

		pthread_mutex_lock(&loopbuf->mutex);
		loopbuf->free = loopbuf->free + length ;
		pthread_mutex_unlock(&loopbuf->mutex);

		return length ;
	}
	else
	{
		return -1;
	}
}
struct thread_param_struct//线程参数结构体
{
	struct loop_buf_struct *loop_buf;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
};


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

/******************************************************************************
Description.: this functions cleans up allocated resources
Input Value.: arg is unused
Return Value: -
******************************************************************************/
void worker_cleanup(void *arg)
{
    static unsigned char first_run = 1;

    if(!first_run) {
        printf("already cleaned up resources\n");
        return;
    }

    first_run = 0;
    printf("cleaning up resources allocated by input thread\n");

    if(thread_param->loop_buf->buf != NULL) free(thread_param->loop_buf->buf);
}

//生产者，只要环形队列有空位,便不断生产
void* productor(void*arg){
    
	/* set cleanup handler to cleanup allocated resources */
    pthread_cleanup_push(worker_cleanup, NULL);

	struct thread_param_struct *t_param =(struct thread_param_struct *)arg;
	struct loop_buf_struct *loopbuf = t_param->loop_buf;//环形缓冲区

    
    u8 pSendBuf[5000]={0};
    while(1){
	//AD9361拼装数据
	int len = InsertBuf(pSendBuf);
	//判断是否为12字节，若不是continue丢弃
	if(SINGLESIZE != len)
	{
	  continue;
	}
		printf("++start product at %d,",loopbuf->rear);        
		int res =loopbuf_produce(loopbuf,pSendBuf,len);//把接收的数据加入环形缓冲区	
		printf("res %d\n",res);
		//memcpy(&(arr[proIndex]),pSendBuf,len); //每次拷贝至写指针当前位置,len个字节
		printData(loopbuf->buf,RINGSIZE);
        pthread_cond_signal(&t_param->cond);//激活条件变量  
	    //sleep(1);
    }
   
    printf("leaving input thread, calling cleanup function now\n");
    pthread_cleanup_pop(1);

	 return NULL;
}




//消费者,只要环形队列中有数据,就不断消费
void* consumer(void*arg){
    /* set cleanup handler to cleanup allocated resources */
    pthread_cleanup_push(worker_cleanup, NULL);

	struct thread_param_struct *t_param =(struct thread_param_struct *)arg;
	struct loop_buf_struct *loopbuf = t_param->loop_buf;//环形缓冲区
  
	u8 consumBuf[SINGLESIZE];

    while(1){
        //存在数据则消费,不存在数据则阻塞,直到有数据为止
		pthread_mutex_lock(&t_param->mutex);  
    	pthread_cond_wait(&t_param->cond,&t_param->mutex);//等待条件变量
		

     	printf("--start consume  at %d\n",loopbuf->front);
		loopbuf_custom(loopbuf,consumBuf,SINGLESIZE);
		printData(consumBuf,SINGLESIZE);
		//判断是否为12字节，若不是则丢弃(默认继续消费)
		int sentBytes = SINGLESIZE;
	
		if(sentBytes <0){
		   break;
		}
		//memset(&(arr[conIndex]),0,SINGLESIZE*sizeof(u8));//直接清空该区域
		//printData(arr,RINGSIZE);
		pthread_mutex_unlock(&t_param->mutex); 
		//sleep(1);
    }
    /* cleanup now */
    pthread_cleanup_pop(1);

    return NULL;
}


int main()
{
	
    pthread_t thread_pro,thread_con;
    
    thread_param = malloc(sizeof(struct thread_param_struct));
    thread_param->loop_buf = loopbuf_init(RINGSIZE);//初始化环形缓冲区;

    pthread_mutex_init(&thread_param->mutex,NULL);
  	pthread_cond_init(&thread_param->cond,NULL);

	if ( pthread_create(&thread_pro,NULL,productor,thread_param) == -1 ) 
    {
    		perror("Error creating thread.");
    		exit(EXIT_FAILURE);
  	}	
  	//该子线程的状态设置为分离的
    pthread_detach(thread_pro); //pthread_self()
  	if ( pthread_create(&thread_con,NULL,consumer,thread_param) == -1 ) 
    {
    		perror("Error creating thread.");
    		exit(EXIT_FAILURE);
  	}	
  	//该子线程的状态设置为分离的
    pthread_detach(thread_con); //pthread_self()

    
    while(1) {sleep(5);}
#if 0
	if ( pthread_join(thread_pro, NULL) ) 
  	{
    	perror("Error joining thread\n");
    	exit(EXIT_FAILURE);
  	}
  	
  	if ( pthread_join(thread_con, NULL) ) 
  	{
    	perror("Error joining thread\n");
    	exit(EXIT_FAILURE);
  	}
#endif

  	//关闭Socket描述符
	loopbuf_destroy(thread_param->loop_buf);
	pthread_mutex_destroy(&thread_param->mutex);
  	pthread_cond_destroy(&thread_param->cond);		
	free(thread_param);
    return 0;
}

