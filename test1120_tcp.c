使用一个标准的TCP通信，来测试两个错误的产生：EPIPE和EAGAIN

send中的标志位：
MSG_DONTWAIT:进行非阻塞的操作
MSG_NOSIGNAL:当连接中断时，不要发送SIGPIPE信号(操作仍会返回错误，并且errno变量值为EPIPE)

setsockopt可设置地址可重用、缓冲区大小、设置超时。

server.c(避免重用)
-----
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HELLO_WORLD_SERVER_PORT 8888
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
  struct sockaddr_in server_addr;
  bzero(&server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htons(INADDR_ANY);
  server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);

  int server_socket = socket(AF_INET,SOCK_STREAM,0);
  if( server_socket < 0)
  {
    printf("Create Socket Failed!");
    exit(1);
  }
//设置地址可重用
#if 1
    int nUse=1;
    setsockopt(server_socket,SOL_SOCKET,SO_SNDBUF,(char *)&nUse,sizeof(nUse)); //避免对服务器重启造成影响
#endif         

  if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
  {
    printf("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT);
    exit(1);
  }

  if ( listen(server_socket, LENGTH_OF_LISTEN_QUEUE) )
  {
    printf("Server Listen Failed!");
    exit(1);
  }

  while (1)
  {
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    int new_server_socket = accept(server_socket,(struct sockaddr*)&client_addr,&length);
    if ( new_server_socket < 0)
    {
      printf("Server Accept Failed!\n");
      break;
    }

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer,"Hello,World from server!");
    strcat(buffer,"\n");
    send(new_server_socket,buffer,BUFFER_SIZE,0);

    bzero(buffer,BUFFER_SIZE);
        while(1){
      length = recv(new_server_socket,buffer,BUFFER_SIZE,0);
      if (length < 0)
      {
        printf("Server Recieve Data Failed!\n");
        exit(1);
      }
      printf("\n%s",buffer);
        }
    close(new_server_socket);
  }
  close(server_socket);
  return 0;
}

client.c
------
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

#define HELLO_WORLD_SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: ./%s ServerIPAddress\n",argv[0]);
    exit(1);
  }
  //////////////////////////////////////////
  
	//sigset_t set;
	//sigemptyset(&set);
	//sigaddset(&set, SIGPIPE);
	//sigprocmask(SIG_BLOCK, &set, NULL);
  //////////////////////////////////////////
  struct sockaddr_in client_addr;
  bzero(&client_addr,sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = htons(INADDR_ANY);
  client_addr.sin_port = htons(0);

  int client_socket = socket(AF_INET,SOCK_STREAM,0);

  if( client_socket < 0)
  {
    printf("Create Socket Failed!\n");
    exit(1);
  }
  
//缓冲区测试
#if 1
    int nZero=10;
    setsockopt(client_socket,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero)); //内核中2倍：10*2字节
#endif                              
       

  if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
  {
    printf("Client Bind Port Failed!\n");
    exit(1);
  }

  struct sockaddr_in server_addr;
  bzero(&server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  if(inet_aton(argv[1],&server_addr.sin_addr) == 0)
  {
    printf("Server IP Address Error!\n");
    exit(1);
  }
  server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
  socklen_t server_addr_length = sizeof(server_addr);
  if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
  {
    printf("Can Not Connect To %s!\n",argv[1]);
    exit(1);
  }

  char buffer[BUFFER_SIZE];
  bzero(buffer,BUFFER_SIZE);
  int length = recv(client_socket,buffer,BUFFER_SIZE,0);
  if(length < 0)
  {
    printf("Recieve Data From Server %s Failed!\n", argv[1]);
    exit(1);
  }
  printf("From Server %s :\t%s",argv[1],buffer);

  bzero(buffer,BUFFER_SIZE);
  strcpy(buffer,"Hello, World! From Client\n");


  while(1){
    sleep(1);
//MSG_NOSIGNAL:Don't generate a SIGPIPE signal
//MSG_DONTWAIT: Enables nonblocking operation; if the operation would block, EAGAIN or EWOULDBLOCK is returned.
    int ret = send(client_socket,buffer,BUFFER_SIZE,MSG_NOSIGNAL | MSG_DONTWAIT);//0  
    if(ret <= 0)
    {
		printf("send error%d  %d\n",ret,errno);
     
        if (ret == -1 && errno == EPIPE){ //对方socket中断，发送端write会返回-1,errno号为EPIPE(32)    
      		printf("receive sigpipe  has catched %d\n",errno);
    	}
		if  (ret == -1 && errno == EAGAIN){ 
      		printf("receive  nonblocking %d\n",errno);
    	}
	}
	
  }

  close(client_socket);
  return 0;
}
