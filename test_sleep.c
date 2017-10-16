
/*
* 命令版：
* date +%S.%N; sleep 0.001; date +%S.%N
* sleep函数参数可以是浮点数
* sleep  0.001 和usleep 1000 都是延迟1ms
*/
#include <stdio.h>  
#include <unistd.h> //sleep
#include<stdlib.h>

int main(int argc, char *argv[])  
{  
     printf("hello\n");
     system("date +%S.%N");

  	 sleep(0.001);    //sleep  0.001s
     usleep(1000);    //usleep 1000us

	 system("date +%S.%N");
     printf("world"); 
    return 0;  
}  
