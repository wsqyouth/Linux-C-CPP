#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<sys/ioctl.h>  
#include<sys/socket.h>  
#include<arpa/inet.h>  
#include<netinet/in.h>  
#include<net/if.h>  
  
//设置IP  
void setip(char *buffer)  
{  
//    char * FILE_NAME = "/etc/network/interfaces";  
    char * FILE_NAME = "interfaces";  
    FILE * file_fd;  
    int len = 0;  
    file_fd = fopen(FILE_NAME,"r+");  
    if(file_fd == NULL)  
    {  
        perror("errno");  
    }  
    //跳过开头的116个字节  
    fseek(file_fd,116,SEEK_CUR);  
    fwrite(buffer,strlen(buffer),1,file_fd);  
    fclose(file_fd);  
}  
  
  
//获取IP  
void getip(char *buffer)  
{  
 //   char * FILE_NAME = "/etc/network/interfaces";  
    char * FILE_NAME = "interfaces";  
    FILE * file_fd;  
    int len = 0;  
    file_fd = fopen(FILE_NAME,"rb");  
    if(file_fd == NULL)  
    {  
        perror("errno");  
    }  
    //跳过开头的116个字节  
    fseek(file_fd,116,SEEK_CUR);  
    len = fread(buffer, 1, 13, file_fd);  
    if(len == -1)  
    {  
        printf("File read error!\n");  
        perror("errno");  
    }  
    fclose(file_fd);  
}  
  
  
int main(int argc, char * argv[])  
{  
    char ipbuffer[20]="192.168.123.112";  
    setip(ipbuffer);  
    getip(ipbuffer);  
    printf("ipbuffer is:%s\n",ipbuffer);  
//    system("reboot");  
//    printf("rebooting!!!\n");  
  
    return 0;  
}  

/////////////////////
auto eth0
iface eth0 inet static
address 192.168.6.10
