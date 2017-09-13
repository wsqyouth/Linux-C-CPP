
#include <stdio.h>
#include <string.h>  //strlen
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>  //inet_add
#include <unistd.h>     //write

int main(int argc , char *argv[])
{
	int socket_fd, child_fd, ret;
	struct sockaddr_in server_addr,client_addr;

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
	server_addr.sin_port = htons( 8000 );
	
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
		
		close(child_fd);
	}

	close(socket_fd);
	return 0;
}
