#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h>                                                
#include <arpa/inet.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
    
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

/*
添加函数connect_retry,函数使用了指数补偿算法，如果调用connect失败，进程就休眠一小段时间然后再尝试，每循环一次增加每次尝试的延迟，
直到最大延迟为1分钟：1+2+4+8+16+32=1min.
*/
#define BUFFER_SIZE 40


#define MAXSLEEP 64
int connect_retry(int sockfd,const struct sockaddr *addr, socklen_t alen)
{
	int nsec;
	//try to connect with exponential backoff
	for(nsec = 1;nsec <= MAXSLEEP; nsec <<=1){
		if( connect(sockfd,addr,alen) == 0){
		 	 //connection accepted
			 return 0;
		}
		
		//delay before trying again
		if(nsec <= MAXSLEEP/2)
			 sleep(nsec);
    }
}
    
	sem_t semConnect;
	char sendBuf[BUFFER_SIZE]={"hello world"};
    char recvBuf[BUFFER_SIZE];

    int client_sockfd; 
    
    struct sockaddr_in address;// 服务器端网络地址结构体                                           
   
   


int main() 
{ 
	int len; 
    int result; 
	
	int ret = sem_init(&semConnect, 0, 0);
    if(ret == -1)
    {
    	perror("semaphore semConnect intitialization failed\n");
        //exit(EXIT_FAILURE);
    }  
	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);// 建立客户端socket  
//超时机制测试
#if 1
	struct timeval tv;
	int nZero=10;
	tv.tv_sec = 3; //超时的秒数
	tv.tv_usec = 5;//超时的微秒数
	setsockopt(client_sockfd,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(tv));  
    
    setsockopt(client_sockfd,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero));

    //setsockopt(client_sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv)); 
#endif                              
    const int chOpt=1;   
    int nErr=setsockopt(client_sockfd,IPPROTO_TCP,TCP_NODELAY,&chOpt,sizeof(chOpt));   
    if(nErr == -1)   
    {   
     perror("setsockopt() error\n");   
     
    }  

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = inet_addr("192.168.33.35");             
    address.sin_port = htons(12000); 
    len = sizeof(address);
//支持重试链接
#if 0
	result = connect_retry(client_sockfd, (struct sockaddr *)&address, len); //connect
#endif   
    // 与远程服务器建立连接
    result = connect(client_sockfd, (struct sockaddr *)&address, len); 
    if(result<0) 
    { 
         perror("connect failed"); 
         exit(EXIT_FAILURE); 
    } 
    printf("Heart Hop message:\n");
    while(1)
    {
		int snd = send(client_sockfd,sendBuf,BUFFER_SIZE,MSG_NOSIGNAL); //MSG_NOSIGNAL
	    if( snd<= 0)
		{
			printf("send error%d  %d\n",snd,errno);
			if( (snd == -1) && (errno != EAGAIN))
			{
				printf("network disconnection\n");				
		        //call reconnect function
			}
				
			
		}
		else
		{
			printf("send success-----\n");
		}
		//successs
		
		//sleep(1);
        /*
		int ret = recv(client_sockfd,recvBuf,BUFFER_SIZE,0); //非阻塞,
		if( ret <= 0)// 实际上,等于0表示服务端主动关闭通信socket  
 		{
			if (ret == -1) {
				//#define EAGAIN  11 // Try again  
        		if( errno == EAGAIN || errno == EWOULDBLOCK) {
					printf("recv error:%d  %d\n",ret,errno);
					continue;
				}
			}
        }
		printf("receive data from server: %s\n",recvBuf);
		*/
    }
	printf("exit ---------\n");
    close(client_sockfd); 
    return 0; 
}
