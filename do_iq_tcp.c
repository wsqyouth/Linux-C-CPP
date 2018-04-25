#tcp通信接收到的是单位为1个字节的数据，我们需要将其整合单位为4字节的数据，之后对其处理。
-------
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include "includes.h"
#include <pthread.h>

int32_t listenfd, connectfd;     //socket描述符

void  tcp_server(uint16_t port)
{
  
    //调用socket，创建监听客户端的socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Creating socket failed.");
        exit(1);
    }
    
    //设置socket属性，端口可以重用
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    //初始化服务器地址结构体
	struct sockaddr_in my_addr;
    bzero(&my_addr,sizeof(my_addr));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(port);
    my_addr.sin_addr.s_addr = htonl (INADDR_ANY);

    //调用bind，绑定地址和端口
    if (bind(listenfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("Bind error.");
        exit(1);
    }

    //调用listen，开始监听
    if(listen(listenfd, 5) == -1){
        perror("listen() error\n");
        exit(1);
    }

}

void decompose(int32_t  buf[], int n)
{
    printf("32-bit Word: I : Q\n");
    for(int j=0;j<n;j++)
    {
         int32_t val = buf[j];
         char *p_dat=(char *)&val;
         const int16_t i = ((int16_t*)p_dat)[0]; // Real (I)
	 const int16_t q = ((int16_t*)p_dat)[1]; // Imag (Q)
	 printf("0x%.8X : %8d : %8d\n",val,i,q);
    }
}

int main()
{
	tcp_server(8000);
    
    struct sockaddr_in client;     //客户端地址信息结构体
    //调用accept，返回与服务器连接的客户端描述符
	uint32_t sin_size = sizeof(struct sockaddr_in);
    char buf[1024]; 
	if ((connectfd = accept(listenfd,(struct sockaddr *)&client, (socklen_t *)&sin_size)) == -1) {
            perror("accept() error\n");
    }else {  
            printf("connected with ip: %s  and port: %d\n", inet_ntop(AF_INET,&client.sin_addr, buf, 1024), ntohs(client.sin_port));  
    }  
	int num;
    char recvbuf[3000];
	int32_t content_buf[3000];
	int count = 0;

    //MSG_WAITALL
 	//while (1) {
		memset(recvbuf, '\0', sizeof(recvbuf));  
		num = recv(connectfd, recvbuf, sizeof(recvbuf),0);
		
		if(num > 0)
		{
			printf("num=%d\n",num);
			//decompose(recvbuf,num);
			for(int i=0;i<num;i+=4)
			{
				content_buf[count]=*(int32_t*)(&recvbuf[i]);
				//printf("0x%.8X \n",content_buf[count]);
				count++;
			}
			printf("count=%d\n",count);	
			decompose(content_buf,count);
		}
		
 	//}
	return 0;
}



------
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include "includes.h"
#include <pthread.h>

int16_t sockfd;
int16_t err_log;

int Tcp_Client_Init(uint8_t* ip,uint16_t port)
{
	uint16_t server_port = port;   // 服务器的端口号6001
	uint8_t  *server_ip =  ip; // 服务器ip地址"192.168.33.33"
	
	//printf("connect %s ...\n",server_ip);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);      // 创建通信端点：套接字
	if (sockfd < 0) {
		perror("socket failed");
		//exit(-1);
		return -1;
	}
	
    //初始化服务器地址结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));    // 初始化服务器地址
	server_addr.sin_family = AF_INET;   		 // IPv4
	server_addr.sin_port = htons(server_port); // 端口 6001
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);    // ip
	// 主动连接服务器
	err_log = connect(sockfd, (struct sockaddr*) &server_addr,sizeof(server_addr));
	if (err_log != 0) {
		printf("connect %s failed!\n",server_ip);
		//close(sockfd);
		//exit(-1);
		return -1;
	}else
		printf("connected %s \n",server_ip);

	return 0;
}



int main()
{
	int16_t ret;
    /**TCP 客户端初始化**/
	ret = Tcp_Client_Init("127.0.0.1",8000);
	if (ret != 0) {
		printf("127.0.0.1,tcp client init failed!\n");
	}
    printf("start send\n");
	int32_t send_buf[] = {0x00007FFF,0x278D79BB,0x4B3B678D,0x678D4B3B,0x79BB278D,0x7FFF0000,0x79BBD873,
                     	  0x678DB4C5,0x4B3B9873,0x278D8645,0x00008001,0xD8738645,0xB4C59873,0x8645D873,
                          0x80010000,0x8645278D,0x98734B3B,0xB4C5678D,0xD87379BB};
	if(err_log == 0)
	{
		 send(sockfd, send_buf, sizeof(send_buf), 0);
		 printf("sent\n");
	}

	return 0;
}
