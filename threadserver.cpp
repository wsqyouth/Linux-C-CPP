
//这是使用pthread进行并发服务器的第一个版本，优点是各个功能分离
//使用pthread线程库
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#define PORT 6666           //服务器端口
#define BACKLOG 5         //listen队列等待的连接数
#define MAXDATASIZE  1024    //缓冲区大小
 
void process_cli(int connectfd, struct sockaddr_in client);    //客户端请求处理函数
void* start_routine(void* arg);        //线程函数
 
typedef struct  _ARG  {
    int connfd;
    struct sockaddr_in client; 
}ARG;                    		  //客户端结构体


void sys_err(const char * ptr_err)
{
    perror(ptr_err);
    exit(EXIT_FAILURE);
}

//处理客户端链接的接收工作*
void accept_conn(int listenfd)
{
	 int  connectfd;       		   //socket描述符
     pthread_t  connectthread;	   //线程体变量
     ARG *arg;                      //客户端结构体变量
	 struct sockaddr_in client;     //客户端地址信息结构体
     int sin_size = sizeof(struct sockaddr_in);
	 
	 while(1) {
        //调用accept，返回与服务器连接的客户端描述符
        if ((connectfd = accept(listenfd,(struct sockaddr *)&client,(socklen_t *)&sin_size))==-1) {                    
            sys_err("accept() error\n");
        }
        
        arg = new  ARG;
        arg->connfd = connectfd;
        memcpy(&arg->client, &client, sizeof(client));
        
		//创建线程，以客户端连接为参数，start_routine为线程执行函数
        if (pthread_create(&connectthread, NULL, start_routine, (void*)arg)) {        
            sys_err("Pthread_create() error");
        }
    }
}
//
void tcp_server(uint16_t port)
{
	int listenfd;				   //socket描述符
    struct sockaddr_in server;     //服务器地址信息结构体
   
	
    //调用socket，创建监听客户端的socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        sys_err("Creating socket failed.");
    }
	
    //设置socket属性，端口可以重用
    int opt = SO_REUSEADDR;        
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));    
	
    //初始化服务器地址结构体
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr = htonl (INADDR_ANY);
	
    //调用bind，绑定地址和端口
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {   
        sys_err("Bind error.");
    }   
	
    //调用listen，开始监听
    if(listen(listenfd,BACKLOG) == -1){      
        sys_err("listen() error\n");
    }
    
	//处理客户端链接的接收工作
    accept_conn(listenfd);

	//关闭监听socket
    close(listenfd);        
}
int main()
{
    
	uint16_t port  = 6666;
	tcp_server(port);
    
}
 
void process_cli(int connectfd, sockaddr_in client)
{
    int num;
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cli_name[MAXDATASIZE];
    
    printf("You got a connection from %s.  ",inet_ntoa(client.sin_addr) );
	 
    while ((num = recv(connectfd, recvbuf, MAXDATASIZE,MSG_WAITALL)) > 0) {
 		recvbuf[num] = '\0';
        printf("Received size( %d ) message: %s",num, recvbuf);	
	}
   
    /* 
 	num = recv(connectfd, cli_name, MAXDATASIZE,0);
    if (num == 0) {
        close(connectfd);
        
        return;
    }
    cli_name[num - 1] = '\0';
    printf("Client's content is: %s\n",cli_name);

    while (num = recv(connectfd, recvbuf, MAXDATASIZE,0)) {
        recvbuf[num] = '\0';
        printf("Received client( %s ) message: %s",cli_name, recvbuf);
        for (int i = 0; i < num - 1; i++) {
            sendbuf[i] = recvbuf[num - i -2];
        }
        sendbuf[num - 1] = '\0';
        send(connectfd,sendbuf,strlen(sendbuf),0);
    }
	*/
	printf("Client disconnected.\n");
    close(connectfd); 
}
void* start_routine(void* arg)
{
    ARG *info;
    info = (ARG *)arg;
    
    process_cli(info->connfd, info->client);
    delete info;
    pthread_exit(NULL);
} 

-----------------------
参考：https://blog.csdn.net/Apollon_krj/article/details/58628733
第二个版本：设置线程分离属性，并修改传参
 g++ tserver.cpp -std=c++11 -o tserver -lpthread
//使用pthread线程库
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<fcntl.h>
#include<errno.h>

#define PORT 6666           //服务器端口
#define BACKLOG 5           //listen队列等待的连接数
#define MAX_THREAD_NUM 3    //所能创建的最大连接线程数
#define MAXDATASIZE  1024    //缓冲区大小
  
void accept_conn(const int);    //客户端请求处理函数
void * deal_conn(void *);       //connfd子连接线程函数
 
typedef struct  _connfd_threadinfo  {
    int connfd;
    struct sockaddr_in cliaddr; 
}CONNFD_THREADINFO;                  //客户端结构体


void sys_err(const char * ptr_err)
{
    perror(ptr_err);
    exit(EXIT_FAILURE);
}

//处理客户端链接的接收工作
void accept_conn(int listenfd)
{
	 int  connectfd;       		   //socket描述符
     pthread_t  connectthread;	   //线程体变量
	 struct sockaddr_in cliaddr;     //客户端地址信息结构体

	 pthread_attr_t attr;//线程状态信息
     CONNFD_THREADINFO thread[MAX_THREAD_NUM];//存储线程信息，用于函数指针传参
	 pthread_attr_init(&attr);//初始化线程属性
     /*设置为分离状态,也可以在线程处理函数中，用pthread_detach()进行分离状态设置，但是效率稍差*/
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
     int sin_size = sizeof(struct sockaddr_in);
	 int thread_index = 0; //线程池索引
	 
	 while(1) {
        //调用accept，返回与服务器连接的客户端描述符
        if ((connectfd = accept(listenfd,(struct sockaddr *)&cliaddr,(socklen_t *)&sin_size)) < 0) {    
			if(errno == EINTR || errno == ECONNABORTED) {
				usleep(1000);
		        continue;                
            } else {
				sys_err("accept() error\n");
			}
        }
        /*保存连接到的客户端信息到结构体中以便传参*/
        thread[thread_index].connfd = connectfd;
        thread[thread_index].cliaddr = cliaddr;
        
		//创建线程，以客户端连接为参数，deal_conn为线程执行函数);
        if (pthread_create(&connectthread, NULL, deal_conn, (void*)&thread[thread_index])) {        
            sys_err("Pthread_create() error");
        }
		pthread_attr_destroy(&attr);//线程资源使用完后释放资源

		thread_index++;
        if(thread_index == MAX_THREAD_NUM){//超过线程总数限制，服务器主线程结束
            printf("Too many connect!\n");
            pthread_exit((void *)(-1));
        }
    }
}

void * deal_conn(void *arg)
{
	CONNFD_THREADINFO * info = (CONNFD_THREADINFO *)(arg);//无类型指针转换为自定义客户端信息类型指针
	int connectfd      = info->connfd;
	sockaddr_in cliaddr = info->cliaddr;
	
	int num;
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cli_name[MAXDATASIZE];
    
    printf("You got a connection from %s:%d.\n",inet_ntoa(cliaddr.sin_addr) ,ntohs(cliaddr.sin_port));//客户端端口
	 
    while ((num = recv(connectfd, recvbuf, MAXDATASIZE,MSG_WAITALL)) > 0) {
 		recvbuf[num] = '\0';
        printf("Received size( %d ) message: %s\n",num, recvbuf);	
	}

	printf("Client disconnected.\n");
    close(connectfd); 
	pthread_exit(NULL);

	/* 
 	num = recv(connectfd, cli_name, MAXDATASIZE,0);
    if (num == 0) {
        close(connectfd);
        
        return;
    }
    cli_name[num - 1] = '\0';
    printf("Client's content is: %s\n",cli_name);

    while (num = recv(connectfd, recvbuf, MAXDATASIZE,0)) {
        recvbuf[num] = '\0';
        printf("Received client( %s ) message: %s",cli_name, recvbuf);
        for (int i = 0; i < num - 1; i++) {
            sendbuf[i] = recvbuf[num - i -2];
        }
        sendbuf[num - 1] = '\0';
        send(connectfd,sendbuf,strlen(sendbuf),0);
    }
	*/
}


//
void tcp_server(uint16_t port)
{
	int listenfd;				   //socket描述符
    struct sockaddr_in server;     //服务器地址信息结构体
   
	
    //调用socket，创建监听客户端的socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        sys_err("Creating socket failed.");
    }
	
    //设置socket属性，端口可以重用
    int opt = SO_REUSEADDR;        
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));    
	
    //初始化服务器地址结构体
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr = htonl (INADDR_ANY);
	
    //调用bind，绑定地址和端口
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {   
        sys_err("Bind error.");
    }   
	
    //调用listen，开始监听
    if(listen(listenfd,BACKLOG) == -1){      
        sys_err("listen() error\n");
    }
    
	//处理客户端链接的接收工作
    accept_conn(listenfd);

	//关闭监听socket
    close(listenfd);        
}
int main()
{
    
	uint16_t port  = 6666;
	tcp_server(port);
    
}


------
client.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char**argv)
{
    int sockfd,n;
    char recvline[4096],sendline[4096];
    struct sockaddr_in servaddr;

    if(argc !=2)
    {
        printf("usage: ./client <ipaddress>\n");
        return 0;
    }
    //创建socket
    if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        printf(" create socket error: %s (errno :%d)\n",strerror(errno),errno);
        return 0;
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    //IP地址从“点分十进制”转换到“二进制整肃”
    if( inet_pton(AF_INET,argv[1], &servaddr.sin_addr) <=0 ) {
        printf("inet_pton error for %s\n",argv[1]);
        return 0;
    }
    //连接
    if( connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) <0) {
        printf(" connect socket error: %s(errno :%d)\n",strerror(errno),errno);
        return 0;
    }

    printf("send msg to server:\n");
    while(fgets(sendline,4096,stdin))
	{
		//send发送
		if ( send(sockfd,sendline,strlen(sendline),0) <0) {
		    printf("send msg error: %s(errno :%d)\n",strerror(errno),errno);
		    return 0;
		}
		bzero(sendline, strlen(sendline));
	}
    close(sockfd);
    return 0;
}
