本次代码主要测试一下几个功能：
1，服务器端使用地址可重用方法，这样在服务器断开后迅速启用时，不会出现bind has use错误。
2，客户端通过使用#if 0方法测试超时机制和支持重连的功能，超时机制将阻塞套接字转换为非阻塞模式，即超过设定时间后立即返回；支持重连机制在服务器负载很重时，
   客户端尝试多处连接，使用了指数补偿的算法。
   
-----server.c---
#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 40

int main() 
{       
	char buf[BUFFER_SIZE];
	int server_sockfd, client_sockfd; 
        int sin_size=sizeof(struct sockaddr_in); 
	struct sockaddr_in server_address; 
	struct sockaddr_in client_address; 
	memset(&server_address,0,sizeof(server_address));
	server_address.sin_family = AF_INET; 
	server_address.sin_addr.s_addr = INADDR_ANY; 
	server_address.sin_port = htons(12000); 
	// 建立服务器端socket 
	if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		perror("server_sockfd creation failed");
		exit(EXIT_FAILURE);
	}
	// 设置套接字选项避免地址使用错误,即设置地址可重用
#if 1
	int on=1;
	if((setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
#endif 
	// 将套接字绑定到服务器的网络地址上 
	if((bind(server_sockfd,(struct sockaddr *)&server_address,sizeof(struct sockaddr)))<0)
	{
		perror("server socket bind failed");
		exit(EXIT_FAILURE);
	}
	// 建立监听队列
	listen(server_sockfd,5);
	// 等待客户端连接请求到达
	client_sockfd=accept(server_sockfd,(struct sockaddr *)&client_address,(socklen_t*)&sin_size);
	if(client_sockfd<0)
	{
		perror("accept client socket failed");
		exit(EXIT_FAILURE);
	}
     while(1)
    {
		// 接收客户端数据
		if(recv(client_sockfd,buf,BUFFER_SIZE,0)<0)
		{
			perror("recv client data failed");
			exit(EXIT_FAILURE);
		}
		printf("receive from client:%s\n",buf);
		sleep(2);
		// 发送数据到客户端
		if(send(client_sockfd,"I have received your message.",BUFFER_SIZE,0)<0)
		{
			perror("send failed");
			exit(EXIT_FAILURE);
		}
    }
	//close(client_sockfd);
	//close(server_sockfd);
	exit(EXIT_SUCCESS);
}


-------cleint.c----
#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h>                                                
#include <arpa/inet.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>
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
int main() 
{ 
    char sendBuf[BUFFER_SIZE]={"hello world"};
    char recvBuf[BUFFER_SIZE];

    int client_sockfd; 
    int len; 
    struct sockaddr_in address;// 服务器端网络地址结构体                                           
    int result; 
    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);// 建立客户端socket  
//超时机制测试
#if 0
	struct timeval tv;
	tv.tv_sec = 10; //超时的秒数
	tv.tv_usec = 0;//超时的微秒数
	setsockopt(client_sockfd,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(tv));  
    setsockopt(client_sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv)); 
#endif                              
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = inet_addr("192.168.123.111");             
    address.sin_port = htons(12000); 
    len = sizeof(address);
//支持重试链接
#if 1
	result = connect_retry(client_sockfd, (struct sockaddr *)&address, len); //connect
#endif   
    // 与远程服务器建立连接
    //result = connect(client_sockfd, (struct sockaddr *)&address, len); 
    if(result<0) 
    { 
         perror("connect failed"); 
         exit(EXIT_FAILURE); 
    } 
    printf("Please input the message:\n");
    while(1)
    {
		int snd = send(client_sockfd,sendBuf,BUFFER_SIZE,0);
	    if( snd<= 0)
		{
			printf("send error%d  %d\n",snd,errno);
		}
		
		sleep(2);

		int ret = recv(client_sockfd,recvBuf,BUFFER_SIZE,0); //非阻塞,
		if( ret <= 0)// 实际上,等于0表示服务端主动关闭通信socket  
 		{
			if (ret == -1) {
				//#define EAGAIN  11 /* Try again */ 
        		if( errno == EAGAIN || errno == EWOULDBLOCK) {
					printf("recv error:%d  %d\n",ret,errno);
					continue;
				}
			}
        }
		printf("receive data from server: %s\n",recvBuf);
    }
    close(client_sockfd); 
    return 0; 
}

