#include <stdio.h>
#include <string.h>  //strlen
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>  //inet_add
#include <unistd.h>     //write


#define  SERVER_PORT 8000
#define  SERVER_IP_ADDRESS "127.0.0.1"
#define  BUF_LEN     1024
int main(int argc, char *argv[])
{
    int skfd;
	struct sockaddr_in server_addr;
	char sendMsg[100]={"Hello,Server!"};    
	char recvbuf[BUF_LEN] = {0};
	int nbytes;
    
  	
    
    //create socket
    if(-1 == (skfd=socket(AF_INET,SOCK_STREAM,0)))
    {
         perror("Socket Error:\n");
         exit(1);
    }

    //prepare the sockaddr_in structure of want to connect
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SERVER_PORT);
    server_addr.sin_addr.s_addr=inet_addr(SERVER_IP_ADDRESS);

    // client connect server 
    if(-1 == connect(skfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr)))
    {
         perror("Connect Error:\n");
         exit(1);
    }
    //client send msg to server
    if(-1 == write(skfd, sendMsg, strlen(sendMsg)) )
	{
	  	 perror("send msg Error:\n");
         exit(1);
	}
    //cilent read message from server
    if(-1 == (nbytes = read(skfd,recvbuf,BUF_LEN)))
	{
         perror("Recv Error:");
	  	 exit(1);
    }
    printf("Server arrived:%s    ,recv len=%d\n",recvbuf,nbytes);

    //close 
    close(skfd);
    exit(0);
}

