本代码提供三个示例：popen的read、write和popen返回值具体分析。

读取外部程序的输出：
---
#include <sys/types.h>    
#include <unistd.h>    
#include <stdlib.h>    
#include <stdio.h>    
#include <string.h>  
  
int main()    
{    
    FILE   *read_fp;    
    char   buf[1024];   
    int    chars_read;
  
    memset(buf, '\0', sizeof(buf) );//初始化buf,以免后面写如乱码到文件中  
    read_fp = popen( "uname -a", "r" ); //将“uname -a”命令的输出 通过管道读取（“r”参数）到FILE* stream  
    if(read_fp != NULL)
	{
		chars_read = fread( buf, sizeof(char), sizeof(buf),  read_fp);  //将刚刚FILE* read_fp的数据流读取到buf中 
		if(chars_read >0)
			printf("my output:\n%s\n",buf);
	}

    pclose( read_fp );    
   
    return 0;  
}   

将输出送往外部程序：
--------
#include <sys/types.h>    
#include <unistd.h>    
#include <stdlib.h>    
#include <stdio.h>    
#include <string.h>  
  
int main()    
{    
    FILE   *write_fp;    
    char   buf[1024];   
    int    chars_write;
  
    memset(buf, '\0', sizeof(buf) );//初始化buf,以免后面写如乱码到文件中
	sprintf(buf,"hello world...\n");  
    write_fp = popen( "od -c", "w" ); //通过管道（“w”参数）到FILE* stream  
    if(write_fp != NULL)
	{
		fwrite( buf, sizeof(char), sizeof(buf),  write_fp);  //将刚刚FILE* write_fp的数据流读取到buf中 
		pclose( write_fp );    
	}

   
   
    return 0;  
}   

popen返回值具体分析
-------
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(int argc, char* argv[])
{
    char cmd[1024];
    char line[1024];
    FILE* pipe;
    int rv;

    if (argc != 2)
    {
        printf("Usage: %s <path>\n", argv[0]);
        return -1;
    }

    // pclose fail: No child processes
    //signal(SIGCHLD, SIG_IGN);
    
    snprintf(cmd, sizeof(cmd), "ls -l %s 2>/dev/null", argv[1]);

    pipe = popen(cmd, "r");
    if(NULL == pipe)
    {
        printf("popen() failed: %s\n", cmd);
        return -1;
    }
    
    while(fgets(line, sizeof(line),pipe) != NULL)
    {
        printf("%s", line);
    }

    rv = pclose(pipe);
 
    if (-1 == rv)
    {
        printf("pclose() failed: %s\n", strerror(errno));
        return -1;
    }
    
    if (WIFEXITED(rv))
    {
        printf("subprocess exited, exit code: %d\n", WEXITSTATUS(rv));
        if (0 == WEXITSTATUS(rv))
        {
            // if command returning 0 means succeed
            printf("command succeed\n");
        }
        else
        {
            if(127 == WEXITSTATUS(rv))
            {
                printf("command not found\n");
                return WEXITSTATUS(rv);
            }
            else
            {
                printf("command failed: %s\n", strerror(WEXITSTATUS(rv)));
                return WEXITSTATUS(rv);
            }
        }
    }
    else
    {
        printf("subprocess exit failed\n");
        return -1;
    }
 
    return 0;
}
