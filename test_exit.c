#include <stdlib.h>  //exit
#include <unistd.h>  //_exit
#include <stdio.h>

/*
* _exit()函数直接将进程关闭，缓冲区数据会丢失
* exit()函数进程关闭前会检查文件打开情况，把文件缓冲区内容冲洗到磁盘
*/
int  main(void)
{
    printf("output begin");
    printf("content in buffer"); //缓冲区遇到换行符或者EOF会冲洗，这里不使用

    //exit(0); //内容都显示
    _exit(0); //内容不显示,因为缓冲区内容未冲洗
    return 0;
}
