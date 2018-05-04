本代码参考https://blog.csdn.net/wangzhicheng1983/article/details/40261087
实现了基于UDP通信的一种框架，能够实现简单的通信交互。

Makefile:
-----
all:udpServer udpClient

udpServer:udpServer.o socket.o
	gcc -g -o udpServer socket.o udpServer.o  
udpClient:udpClient.o socket.o
	gcc -g -o udpClient socket.o udpClient.o 
udpServer.o:udpServer.c  
	gcc -g -c udpServer.c 
udpClient.o:udpClient.c 
	gcc -g -c udpClient.c 
socket.o : socket.c
	gcc -g -c socket.c
clean:all
	rm  *.o udpServer  udpClient
  
  
 udpServer.c
 -----
 
/*************************************************************************
    > File Name: server.c
	> Author: ma6174
 ************************************************************************/

#include "socket.h"

int UDPPORT = 4001;
#define SIZE 256

int main() {
	int serverfd = -1;
	char buf[SIZE];
	char from_ip[SIZE];
	int port, num;
	if(UDPServerInit(UDPPORT, &serverfd) < 0) {
		perror("can init UDP server...!\n");
		exit(EXIT_FAILURE);
	}
	while(1) {
		num = UDPRecv(serverfd, buf, SIZE, from_ip, &port);
		if(num < 0) {
			perror("receving error...!\n");
			exit(EXIT_FAILURE);
		}
		buf[num] = '\0';
		printf("from client message: %s\n Its ip is %s, port is %d\n", buf, from_ip, port);
		if(!strcmp(buf, "bye")) {
			break;
		}
//		sleep(1);
	}
	UDPClose(serverfd);

	return 0;
}

udpClient.c
-----

/*************************************************************************
    > File Name: client.c
    > Author: ma6174
 ************************************************************************/

#include "socket.h"

#define SIZE 256
int UDPPORT = 4001;
const char *serveraddr = "127.0.0.1";

int main() {
	int clientfd = -1;
	char buf[SIZE];
	size_t  num;
	UDPClientInit(&clientfd);
	if(clientfd < 0) {
		perror("UDP client init failed...!\n");
		exit(EXIT_FAILURE);
	}
	while(1) {
		puts("sending message...:");
		scanf("%s", buf);
		int sendBytes = UDPSend(clientfd, buf, strlen(buf), serveraddr, &UDPPORT);
		printf("sent %d\n",sendBytes);
		sleep(1);
	}
	return 0;
}


所依赖的文件
socket.c
-----

#include "socket.h"
/*
 * @brief		initialize UDP server
 * @port		port number for socket
 * @serverfd	server socket fd
 * return server socked fd for success, on error return error code
 * */
int	UDPServerInit(int port, int *serverfd) {
	struct sockaddr_in server;
	// create socket , same as client
	*serverfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(*serverfd < 0) return -1;
	/// initialize structure dest
	memset((void*)&server, '\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;
	// Assign a port number to socket
	bind(*serverfd, (struct sockaddr*)&server, sizeof(server));

	return *serverfd;
}
/*
 * @brief	initialize UDP client
 * @clientfd	client socket fd
 * return client socked fd for success, on error return error code
 */
int	UDPClientInit(int *clientfd) {
	*clientfd = socket(AF_INET, SOCK_DGRAM, 0);

	return *clientfd;
}
/*
 * @brief		UDPRecv from UDP socket
 * @clientfd	socket fd
 * @buf	     	receving buffer
 * @size		buffer size
 * @from_ip		the client ip address of requested from UDP
 * return	    the length of receive data
 */
int	UDPRecv(int sockfd, void *buf, size_t size, char *from_ip, int *port) {
	struct sockaddr_in client;
	size_t addrlen = 0;
	int rc;
	memset(&client, '\0', sizeof(client));
	addrlen = sizeof(client);
	rc = recvfrom(sockfd, buf, size, 0, (struct sockaddr *)&client, (socklen_t *)&addrlen);
	strcpy(from_ip, (const char *)inet_ntoa(client.sin_addr));
	*port = htons(client.sin_port);

	return rc;
	
}
/*
 * @brief		UDPSend from UDP socket
 * @clientfd	socket fd
 * @buf	     	sending buffer
 * @size		buffer size
 * @to_ip		the ip address of target server
 * return	    the length of sending data
 */
int	UDPSend(int sockfd, const void *buf, size_t size, const char *to_ip, const int *port) {
	struct sockaddr_in server;
	memset(&server, '\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(*port);
	inet_aton(to_ip, &server.sin_addr);

	return sendto(sockfd, buf, size, 0, (struct sockaddr *)&server, sizeof(server));
}
/*
 * close the socket
 * */
int UDPClose(int sockfd) {
	return close(sockfd);
}

-----
socket.h


/**
  @file		socket.h
  @brief	Socket API header file

  UDP socket utility functions, it provides simple functions that helps
  to build UDP client/server.
 */
#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int	UDPServerInit(int port, int *serverfd);
int	UDPClientInit(int *clientfd);
int	UDPRecv(int sockfd, void *buf, size_t size, char *from_ip, int *port);
int	UDPSend(int sockfd, const void *buf, size_t size, const char *to_ip, const int *port);
int UDPClose(int sockfd);

#endif



