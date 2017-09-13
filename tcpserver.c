
#include <stdio.h>
#include <string.h>  //strlen
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>  //inet_add
#include <unistd.h>     //write

#define  SERVER_PORT 8000
#define  BUF_LEN     1024
int main(int argc , char *argv[])
{
	int socket_fd, child_fd, ret;
	struct sockaddr_in server_addr,client_addr;
    
	char Msg[100]={"Ok,i hear you!"};    
	char buf[BUF_LEN] = {0};
	int nbytes;

	//create socket
	socket_fd = socket(AF_INET , SOCK_STREAM ,0);
	if(socket_fd == -1)
	{
		perror("Could not create socket\r\n");
		exit(1);
	}
	
	//prepare the sockaddr_in structure
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons( SERVER_PORT );
	
	//bind
	if( bind(socket_fd,(struct sockaddr *)&server_addr ,sizeof(server_addr) ) <0)
	{
		//print error message
		perror("Bind faild. error\r\n");
		exit(1);
	}

	//listen
	if( listen(socket_fd,3) == -1)
	{
		perror("Listen error.\r\n");
	}
	
	while(1)
	{
		int addr_len=sizeof(struct sockaddr_in);
		if(-1 ==(child_fd = accept(socket_fd, (struct sockaddr *)(&client_addr),&addr_len) ))
		{
			perror("Accept error.\r\n");
			exit(1);
		}
        printf("Connect from %s:%u ...!\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		
		//read message
		if(-1 == (nbytes = read(child_fd,buf,BUF_LEN)))
		{
        	 perror("Recv Error:");
		     exit(1);
    	}
    	printf("Client arrived:%s    ,recv len=%d\n",buf,nbytes);

		//server reply message to client
    	if(-1 == write(child_fd, Msg, strlen(Msg)) )
		{
	  	    perror("send msg Error:\n");
      	    exit(1);
		}
		//close(child_fd);
	}
	//close
	//close(socket_fd);
	return 0;
}
