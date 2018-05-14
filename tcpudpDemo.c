
//网上的一个标准代码，其中udp就是很标准的代码，没什么特别的，tcp实现了文件传输，后面可以仔细分析下是如何传输大数据的
//同时能够对二者的学习进行区分。

Linux网络编程之UDP Socket程序示例
本文所述示例程序是基于Linux平台的socket网络编程，实现文件传输功能。该示例是基于TCP流协议实现的socket网络文件传输程序。
采用C语言编写。最终能够实现传输任何格式文件的文件传输程序。

Server端代码如下：
---
/************************************************************************* 
  > File Name: Server.c 
  > Author: SongLee 
 ************************************************************************/ 
 
#include<netinet/in.h> // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h> // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>   // exit 
#include<string.h>   // bzero 
 
#define SERVER_PORT 8000 
#define LENGTH_OF_LISTEN_QUEUE 20 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 
 
int main(void) 
{ 
  // 声明并初始化一个服务器端的socket地址结构 
  struct sockaddr_in server_addr; 
  bzero(&server_addr, sizeof(server_addr)); 
  server_addr.sin_family = AF_INET; 
  server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  server_addr.sin_port = htons(SERVER_PORT); 
 
  // 创建socket，若成功，返回socket描述符 
  int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0); 
  if(server_socket_fd < 0) 
  { 
    perror("Create Socket Failed:"); 
    exit(1); 
  } 
  int opt = 1; 
  setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
 
  // 绑定socket和socket地址结构 
  if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))) 
  { 
    perror("Server Bind Failed:"); 
    exit(1); 
  } 
   
  // socket监听 
  if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE))) 
  { 
    perror("Server Listen Failed:"); 
    exit(1); 
  } 
 
  while(1) 
  { 
    // 定义客户端的socket地址结构 
    struct sockaddr_in client_addr; 
    socklen_t client_addr_length = sizeof(client_addr); 
 
    // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信 
    // accept函数会把连接到的客户端信息写到client_addr中 
    int new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length); 
    if(new_server_socket_fd < 0) 
    { 
      perror("Server Accept Failed:"); 
      break; 
    } 
 
    // recv函数接收数据到缓冲区buffer中 
    char buffer[BUFFER_SIZE]; 
    bzero(buffer, BUFFER_SIZE); 
    if(recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0) < 0) 
    { 
      perror("Server Recieve Data Failed:"); 
      break; 
    } 
 
    // 然后从buffer(缓冲区)拷贝到file_name中 
    char file_name[FILE_NAME_MAX_SIZE+1]; 
    bzero(file_name, FILE_NAME_MAX_SIZE+1); 
    strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer)); 
    printf("%s\n", file_name); 
 
    // 打开文件并读取文件数据 
    FILE *fp = fopen(file_name, "r"); 
    if(NULL == fp) 
    { 
      printf("File:%s Not Found\n", file_name); 
    } 
    else 
    { 
      bzero(buffer, BUFFER_SIZE); 
      int length = 0; 
      // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
      while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
      { 
        if(send(new_server_socket_fd, buffer, length, 0) < 0) 
        { 
          printf("Send File:%s Failed./n", file_name); 
          break; 
        } 
        bzero(buffer, BUFFER_SIZE); 
      } 
 
      // 关闭文件 
      fclose(fp); 
      printf("File:%s Transfer Successful!\n", file_name); 
    } 
    // 关闭与客户端的连接 
    close(new_server_socket_fd); 
  } 
  // 关闭监听用的socket 
  close(server_socket_fd); 
  return 0; 
} 

Client端代码如下：
---
/************************************************************************* 
  > File Name: Client.c 
  > Author: SongLee 
 ************************************************************************/ 
 
#include<netinet/in.h>  // sockaddr_in 
#include<sys/types.h>  // socket 
#include<sys/socket.h>  // socket 
#include<stdio.h>    // printf 
#include<stdlib.h>    // exit 
#include<string.h>    // bzero 
 
#define SERVER_PORT 8000 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 
 
int main() 
{ 
  // 声明并初始化一个客户端的socket地址结构 
  struct sockaddr_in client_addr; 
  bzero(&client_addr, sizeof(client_addr)); 
  client_addr.sin_family = AF_INET; 
  client_addr.sin_addr.s_addr = htons(INADDR_ANY); 
  client_addr.sin_port = htons(0); 
 
  // 创建socket，若成功，返回socket描述符 
  int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
  if(client_socket_fd < 0) 
  { 
    perror("Create Socket Failed:"); 
    exit(1); 
  } 
 
  // 绑定客户端的socket和客户端的socket地址结构 非必需 
  if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)))) 
  { 
    perror("Client Bind Failed:"); 
    exit(1); 
  } 
 
  // 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接 
  struct sockaddr_in server_addr; 
  bzero(&server_addr, sizeof(server_addr)); 
  server_addr.sin_family = AF_INET; 
  if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == 0) 
  { 
    perror("Server IP Address Error:"); 
    exit(1); 
  } 
  server_addr.sin_port = htons(SERVER_PORT); 
  socklen_t server_addr_length = sizeof(server_addr); 
 
  // 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接 
  if(connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0) 
  { 
    perror("Can Not Connect To Server IP:"); 
    exit(0); 
  } 
 
  // 输入文件名 并放到缓冲区buffer中等待发送 
  char file_name[FILE_NAME_MAX_SIZE+1]; 
  bzero(file_name, FILE_NAME_MAX_SIZE+1); 
  printf("Please Input File Name On Server:\t"); 
  scanf("%s", file_name); 
 
  char buffer[BUFFER_SIZE]; 
  bzero(buffer, BUFFER_SIZE); 
  strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name)); 
   
  // 向服务器发送buffer中的数据 
  if(send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0) 
  { 
    perror("Send File Name Failed:"); 
    exit(1); 
  } 
 
  // 打开文件，准备写入 
  FILE *fp = fopen(file_name, "w"); 
  if(NULL == fp) 
  { 
    printf("File:\t%s Can Not Open To Write\n", file_name); 
    exit(1); 
  } 
 
  // 从服务器接收数据到buffer中 
  // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
  bzero(buffer, BUFFER_SIZE); 
  int length = 0; 
  while((length = recv(client_socket_fd, buffer, BUFFER_SIZE, 0)) > 0) 
  { 
    if(fwrite(buffer, sizeof(char), length, fp) < length) 
    { 
      printf("File:\t%s Write Failed\n", file_name); 
      break; 
    } 
    bzero(buffer, BUFFER_SIZE); 
  } 
 
  // 接收成功后，关闭文件，关闭socket 
  printf("Receive File:\t%s From Server IP Successful!\n", file_name); 
  close(fp); 
  close(client_socket_fd); 
  return 0; 
} 

该程序备有较为详尽的注释，相信不难理解。感兴趣的朋友可以在此基础上尝试一些功能的扩展，使其功能更加强大。


--------------------------------------------------
Linux网络编程之UDP Socket程序示例
在网络传输协议中，TCP协议提供的是一种可靠的，复杂的，面向连接的数据流（SOCK_STREAM）传输服务，它通过三段式握手过程建立连接。
TCP有一种“重传确认”机制，即接收端收到数据后要发出一个肯定确认的信号，发送端如果收到接收端肯定确认的信号，就会继续发送其他的数据，
如果没有，它就会重新发送。
相对而言，UDP协议则是一种无连接的，不可靠的数据报（SOCK_DGRAM）传输服务。
使用UDP套接口不用建立连接，服务端在调用socket()生成一个套接字并调用bind()绑定端口后就可以进行通信（recvfrom函数和sendto函数）了；
客户端在用socket()生成一个套接字后就可以向服务端地址发送和接收数据了。

server端代码如下：
/************************************************************************* 
 > File Name: server.c 
 > Author: SongLee 
 ************************************************************************/ 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<unistd.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<stdio.h> 
#include<stdlib.h> 
#include<errno.h> 
#include<netdb.h> 
#include<stdarg.h> 
#include<string.h> 
 
#define SERVER_PORT 8000 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 
 
int main() 
{ 
 /* 创建UDP套接口 */ 
 struct sockaddr_in server_addr; 
 bzero(&server_addr, sizeof(server_addr)); 
 server_addr.sin_family = AF_INET; 
 server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
 server_addr.sin_port = htons(SERVER_PORT); 
 
 /* 创建socket */ 
 int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0); 
 if(server_socket_fd == -1) 
 { 
  perror("Create Socket Failed:"); 
  exit(1); 
 } 
 
 /* 绑定套接口 */ 
 if(-1 == (bind(server_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)))) 
 { 
  perror("Server Bind Failed:"); 
  exit(1); 
 } 
 
 /* 数据传输 */ 
 while(1) 
 {  
  /* 定义一个地址，用于捕获客户端地址 */ 
  struct sockaddr_in client_addr; 
  socklen_t client_addr_length = sizeof(client_addr); 
 
  /* 接收数据 */ 
  char buffer[BUFFER_SIZE]; 
  bzero(buffer, BUFFER_SIZE); 
  if(recvfrom(server_socket_fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&client_addr, &client_addr_length) == -1) 
  { 
   perror("Receive Data Failed:"); 
   exit(1); 
  } 
 
  /* 从buffer中拷贝出file_name */ 
  char file_name[FILE_NAME_MAX_SIZE+1]; 
  bzero(file_name,FILE_NAME_MAX_SIZE+1); 
  strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer)); 
  printf("%s\n", file_name); 
 } 
 close(server_socket_fd); 
 return 0; 
} 

client端代码如下：

/************************************************************************* 
 > File Name: client.c 
 > Author: SongLee 
 ************************************************************************/ 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<unistd.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<stdio.h> 
#include<stdlib.h> 
#include<errno.h> 
#include<netdb.h> 
#include<stdarg.h> 
#include<string.h> 
 
#define SERVER_PORT 8000 
#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 
 
int main() 
{ 
 /* 服务端地址 */ 
 struct sockaddr_in server_addr; 
 bzero(&server_addr, sizeof(server_addr)); 
 server_addr.sin_family = AF_INET; 
 server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
 server_addr.sin_port = htons(SERVER_PORT); 
 
 /* 创建socket */ 
 int client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0); 
 if(client_socket_fd < 0) 
 { 
  perror("Create Socket Failed:"); 
  exit(1); 
 } 
 
 /* 输入文件名到缓冲区 */ 
 char file_name[FILE_NAME_MAX_SIZE+1]; 
 bzero(file_name, FILE_NAME_MAX_SIZE+1); 
 printf("Please Input File Name On Server:\t"); 
 scanf("%s", file_name); 
 
 char buffer[BUFFER_SIZE]; 
 bzero(buffer, BUFFER_SIZE); 
 strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name)); 
 
 /* 发送文件名 */ 
 if(sendto(client_socket_fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0) 
 { 
  perror("Send File Name Failed:"); 
  exit(1); 
 } 
 
 close(client_socket_fd); 
 return 0; 
} 
