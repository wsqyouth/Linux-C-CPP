#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

/*
* fork 被调用一次，返回值却有两次：
*在父进程中，fork返回新创建子进程的进程ID。
*在子进程中，fork返回0。
*
*gcc test_fork.c -o test_fork -lpthread
*/
int  main(void)
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        printf("error in fork!");
    }
    else if(pid == 0)
    {
        printf("I am the child process, my process ID is %d,my parent ID is %d\n",getpid(),getppid());//子进程
    }
    else
    {
        printf("I am the parent process,the value of pid is %d\n",pid); //在父进程中，返回新创建子进程ID
        printf("I am the parent process,my process ID is %d\n",getpid());
    }

    return 0;
}

------------------------------------
output：
I am the parent process,the value of pid is 11629
I am the parent process,my process ID is 11628
I am the child process, my process ID is 11629,my parent ID is 11628
