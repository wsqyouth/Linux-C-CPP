#include <stdio.h>  
#include <time.h>  
#include <string.h>  
#include <stdlib.h> 
/*
-c: 表示次数，1 为1次 -w: 表示deadline, time out的时间，单位为秒
-i<间隔秒数> 指定收发信息的间隔时间。 

子进程的结束状态返回后存于status，底下有几个宏可判别结束情况  
WIFEXITED(status)如果子进程正常结束则为非0值。  
WEXITSTATUS(status)取得子进程exit()返回的结束代码，一般会先用WIFEXITED 来判断是否正常结束才能使用此宏。  
WIFSIGNALED(status)如果子进程是因为信号而结束则此宏值为真 

WTERMSIG(status)取得子进程因信号而中止的信号代码，一般会先用WIFSIGNALED 来判断后才使用此宏。  
WIFSTOPPED(status)如果子进程处于暂停执行情况则此宏值为真。一般只有使用WUNTRACED 时才会有此情况。  
WSTOPSIG(status)取得引发子进程暂停的信号代码，一般会先用WIFSTOPPED 来判断后才使用此宏。
*/ 
int Ping(char* host) {
        int ret;
        char cmd[64];

        /* Safer than strcat: snprintf*/
        snprintf(cmd, sizeof(cmd), "ping -c 3 -i 1 %s >/dev/null", host);
        printf("%s\n",cmd); 
		ret = system(cmd);
        if(WIFEXITED(ret))
                return WEXITSTATUS(ret); //0 normal
        else
                return -1;
}

int main()
{
/*
   int status = system("ping -c3 -w 1 192.168.123.207");
    printf("%d\n",status);
    if(WIFEXITED(status))
    	printf("%d\n",WEXITSTATUS(status)); //0 normal
*/
    

	if(0==Ping("192.168.123.137"))
    {
		printf("%s\n","is UP");
	}else
	{
		printf("%s\n","is Down");
	}
    return 0;
}
