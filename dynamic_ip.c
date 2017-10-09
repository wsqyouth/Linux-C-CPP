//代码能够简单的判断IP地址格式为x.x.x.x
//能够在root权限下实时修改IP，重启无效
//成功返回0

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<net/if.h>
#include<string.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/ioctl.h>

#include<regex.h>

//#define ETH_NAME "eth0"	//
#define ETH_NAME "ens33"	//

/*************************************************************
* 函数功能：	通过正则表达式检测是否为IP地址
* 参数类型：	需要检测的IP地址
* 返回类型：	成功返回0，失败返回-1
**************************************************************/
int check_right_ip(const char *ip)
{
    int status = 0;
	int cflags = REG_EXTENDED;
	regmatch_t pmatch[1];
	const size_t nmatch = 1;
	regex_t reg;
	char str_ip[30] = "";
	const char *pattern = "[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}";//存在局限

        strcpy(str_ip,ip);
	regcomp(&reg, pattern, cflags);
	status = regexec(&reg,str_ip,nmatch,pmatch,0);

	if(status==REG_NOMATCH)
	{
        printf("No match\n");
		return -1;
	}
	else if(status==0)
	{
        return 0;
	}
	regfree(&reg);

	return 0;
}

/*************************************************************
* 函数功能：	获取IP地址
* 参数类型：	IP地址存放位置
* 返回类型：	
**************************************************************/
char *get_ip(char *ip)
{
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
	char *temp_ip = NULL;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket");
        return NULL;                
    }
    strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl");
        return NULL;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	temp_ip = inet_ntoa(sin.sin_addr);
	strcpy(ip,temp_ip);
    fprintf(stdout, "eth0: %s\n", temp_ip);

	return ip;
}

/*************************************************************
* 函数功能：	手动设置IP地址
* 参数类型：	要设置的IP地址
* 返回类型：	成功返回0，失败返回-1
**************************************************************/
int set_hand_ip(const char *ipaddr)
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
    strncpy(ifr_set_ip.ifr_name, ETH_NAME, sizeof(ifr_set_ip.ifr_name)-1);        

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

int main()
{
   char ip[] = "192.168.123.12";
   printf("ip valid:%d\n",check_right_ip(ip));
   printf("modify valid:%d\n",set_hand_ip(ip));
   return 0;
}
