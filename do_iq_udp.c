
//udp发送数据

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAXDATASIZE  5000


int  udp_server(uint16_t port)
{
    int32_t server_sockfd;
    
	//初始化地址结构体
	struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr)); //数据初始化--清零
    my_addr.sin_family = AF_INET; //设置为IP通信
    my_addr.sin_addr.s_addr = INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
    my_addr.sin_port = htons(port); //服务器端口号

    /*创建服务器端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if((server_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket error");
        return -1;
    }
    /*将套接字绑定到服务器的网络地址上*/
    if (bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0)
    {
        perror("bind error");
        return  -1;
    }
	
	return server_sockfd;
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


void decomposeBuf(int8_t  recvbuf[], int num)
{
	int32_t content_buf[4000];
	int count = 0;	
	for(int i=0;i<num;i+=4)
	{
		content_buf[count]=*(int32_t*)(&recvbuf[i]);
		//printf("0x%.8X \n",content_buf[count]);
		count++;
	
	}
	//printf("count=%d\n",count);	
	decompose(content_buf,count);
}


int main()
{

	
    uint8_t buf[MAXDATASIZE];  //数据传送的缓冲区
	int server_sockfd = udp_server(4001);
	if(server_sockfd < 0)
		printf("udp server init failed\n");

	ssize_t len;
	struct sockaddr_in remote_addr; //客户端网络地址结构体
	uint32_t  sin_size = sizeof(struct sockaddr_in);
	/*接收客户端的数据并将其发送给客户端--recvfrom是无连接的*/
    while (((len = recvfrom(server_sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&remote_addr, &sin_size)) > 0)) {
		
		printf("len=%d\n",len);	
		decomposeBuf(buf,len);
		#if 0
        len = sendto(server_sockfd, buf, len, 0, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr));
        if (len > 0) {
            LOG(INFO)<<" send out len:"<<len<<endl;
        }
		#endif
	}
	return 0;
}


----
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


#define SIZE 256  


int UDPPORT = 4001;  
const char *serveraddr = "127.0.0.1";  


/* 
 * @brief       initialize UDP server 
 * @port        port number for socket 
 * @serverfd    server socket fd 
 * return server socked fd for success, on error return error code 
 * */  
int UDPServerInit(int port, int *serverfd) {  
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
 * @brief   initialize UDP client 
 * @clientfd    client socket fd 
 * return client socked fd for success, on error return error code 
 */  
int UDPClientInit(int *clientfd) {  
    *clientfd = socket(AF_INET, SOCK_DGRAM, 0);  
  
    return *clientfd;  
}  
/* 
 * @brief       UDPRecv from UDP socket 
 * @clientfd    socket fd 
 * @buf         receving buffer 
 * @size        buffer size 
 * @from_ip     the client ip address of requested from UDP 
 * return       the length of receive data 
 */  
int UDPRecv(int sockfd, void *buf, size_t size, char *from_ip, int *port) {  
    struct sockaddr_in client;  
    size_t addrlen = 0;  
    int rc;  
    memset(&client, '\0', sizeof(client));  
    addrlen = sizeof(struct sockaddr_in);
    rc = recvfrom(sockfd, buf, size, 0, (struct sockaddr *)&client, &addrlen);  
    strcpy(from_ip, (const char *)inet_ntoa(client.sin_addr));  
    *port = htons(client.sin_port);  
  
    return rc;  
      
}  
/* 
 * @brief       UDPSend from UDP socket 
 * @clientfd    socket fd 
 * @buf         sending buffer 
 * @size        buffer size 
 * @to_ip       the ip address of target server 
 * return       the length of sending data 
 */  
int UDPSend(int sockfd, const void *buf, size_t size, const char *to_ip, const int *port) {  
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


int main()
{
	int16_t ret;
    /**TCP 客户端初始化**/
	int clientfd = -1;  
    char buf[SIZE];  
    size_t  num;  
    UDPClientInit(&clientfd);  
    if(clientfd < 0) {  
        perror("UDP client init failed...!\n");  
        exit(EXIT_FAILURE);  
    }  

    printf("start send\n");
	int32_t send_buf[] = {0x00007FFF,0x278D79BB,0x4B3B678D,0x678D4B3B,0x79BB278D,0x7FFF0000,0x79BBD873,
                     	  0x678DB4C5,0x4B3B9873,0x278D8645,0x00008001,0xD8738645,0xB4C59873,0x8645D873,
                          0x80010000,0x8645278D,0x98734B3B,0xB4C5678D,0xD87379BB};

	UDPSend(clientfd, send_buf, sizeof(send_buf), serveraddr, &UDPPORT);  
	printf("sent\n");
	
	return 0;
}
