#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>   
#include <strings.h>  
#include<unistd.h>  
#include<sys/types.h>  
#include<arpa/inet.h>  
#include<net/if.h>   
#include<signal.h>  
#include<sys/wait.h>  
#include<sys/ioctl.h>  

/************************************************************* 
* 函数功能： 手动设置IP地址 
* 参数类型： 要设置的IP地址 
* 返回类型： 成功返回0，失败返回-1 
**************************************************************/  
int set_hand_ip(const char *ifname, char *ipaddr)  
{  
    int sock_set_ip;       
    struct sockaddr_in sin_set_ip;       
    struct ifreq ifr_set_ip;       
  
    memset( &ifr_set_ip,0,sizeof(ifr_set_ip));       
    if( ipaddr == NULL )       
    {  
        return -1;       
    }  
     
    sock_set_ip = socket( AF_INET, SOCK_STREAM, 0 );  
    //printf("sock_set_ip=====%d\n",sock_set_ip);  
    if(sock_set_ip<0)    
    {       
        perror("socket create failse...SetLocalIp!");       
        return -1;       
    }       
  
    memset( &sin_set_ip, 0, sizeof(sin_set_ip));       
    strncpy(ifr_set_ip.ifr_name, ifname, sizeof(ifr_set_ip.ifr_name)-1);          
  
    sin_set_ip.sin_family = AF_INET;       
    sin_set_ip.sin_addr.s_addr = inet_addr(ipaddr);       
    memcpy( &ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));       
    printf("ipaddr===%s\n",ipaddr);  
    if( ioctl( sock_set_ip, SIOCSIFADDR, &ifr_set_ip) < 0 )       
    {       
        perror( "Not setup interface");       
        return -1;       
    }       
  
    //设置激活标志        
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;        
  
    //get the status of the device        
    if( ioctl( sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip ) < 0 )       
    {       
         perror("SIOCSIFFLAGS");       
         return -1;       
    }       
      
    close( sock_set_ip );       
    return 0;  
}  
//根据网卡名设置IP
int SetIfAddr(char *ifname, char *Ipaddr)
{
    int fd;
    int rc=0;
    struct ifreq ifr;
    struct sockaddr_in *sin;


    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
            perror("socket   error");
            return -1;
    }
    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name,ifname);
    sin = (struct sockaddr_in*)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    //IP地址
    if(inet_aton(Ipaddr,&(sin->sin_addr)) < 0)
    {
        perror("inet_aton   error");
        return -2;
    }

    if(ioctl(fd,SIOCSIFADDR,&ifr) < 0)
    {
        perror("ioctl   SIOCSIFADDR   error");
        return -3;
    }
    close(fd);
    return rc;
}


int main()
{
  SetIfAddr("eth0","192.168.1.33"); 
  set_hand_ip("eth1","192.168.2.33"); 
  return 0;
 }
