这个主要是用来测试TCP有关信号部分的：
测试指令：
step 1）编译
        gcc -o server server.c
        gcc -o -g client client.c （通过gdb直接看到异常退出）
step 2）启动服务器端：./server
step 3) 启动客户端：（这里假设客户端和服务器部署在同一台服务器） gdb ./client 
        (gdb) r 127.0.0.1
step 4) 观察正常运行结果：首先是客户端收到服务器端的消息：From Server 127.0.0.1 : Hello,World from server!
        然后是服务器端每隔1s收到客户端的消息： Hello, World! From Client
step 5）通过ctrl+c关闭服务器端
step 6）观察客户端结果
Program received signal SIGPIPE, Broken pipe.
重现了，原因是：
网络断开连接后，内核发出信号SIGPIPE，含义是管道破裂，默认操作是终止进程。因此应用程序“崩溃”。

server.c
-----
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HELLO_WORLD_SERVER_PORT 6666
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
-----
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

#define HELLO_WORLD_SERVER_PORT 6666
#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: ./%s ServerIPAddress\n",argv[0]);
    exit(1);
  }
  //////////////////////////////////////////
  
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	sigprocmask(SIG_BLOCK, &set, NULL);
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
    int ret = send(client_socket,buffer,BUFFER_SIZE,0);
        if (ret == -1 && errno == EPIPE){
      printf("receive sigpipe\n");
    }
  }

  close(client_socket);
  return 0;
}
