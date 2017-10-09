root权限下：
vim /etc/network/interfaces
sudo /etc/init.d/networking restart
-------
# interfaces(5) file used by ifup(8) and ifdown(8)
auto lo
iface lo inet loopback
#开机自动连接网络eth0
auto ens33
#static表示使用固定ip，dhcp表述使用动态ip eth0
iface ens33 inet static
address 192.168.123.111
netmask 255.255.255.0
gateway 192.168.123.1

ubuntu从15开始网卡名修改为ens33,要根据自己网卡名进行修改。
该代码在嵌入式平台下能够修改开发板IP（通过修改配置文件，并立即重启的方式）
优点：避免了通过系统调用修改IP死机、
缺点是：1，立即重启，重启网卡不起作用 2，修改IP必须通过fseek跳过指定的字节数，这要求配置文件前面部分是固定的

-------------------------------------------------------------------
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
    char * FILE_NAME = "/etc/network/interfaces";  
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
    char * FILE_NAME = "/etc/network/interfaces";  
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
    printf("rebooting!!!\n");  
  
    return 0;  
}  

