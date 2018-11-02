
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
