#include <stdlib.h>  
#include <stdio.h>   //ffush

/*
* int fflush(FILE *stream);
* If  the  stream  argument  is  NULL,  fflush()  flushes all open output streams.
*
* system("shutdown -h now"); 
* system("reboot");
*/
int  main(void)
{
    printf("output begin");
    printf("shutdown"); //缓冲区遇到换行符或者EOF会冲洗，这里不使用

   
    fflush(NULL); //内容都显示
    system("shutdown -h now");  //关机后关闭电源〔halt〕。 root权限
    return 0;
}
