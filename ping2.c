//-c 指定发包次数
#include <stdio.h>
#include <unistd.h>

int main()
{
   if ( execlp("ping", "ping","-c","3","192.168.33.253", (char*)0) < 0)
	{
			printf("execlp error\n");
			
	}

   return 0;
}


//示例2
//在本地机可以添加选项  -i 0.5
//在arm是不识别，所以去掉了
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
     //-c 指定ping次数; -i指定ping的时间间隔
    while(1)
    {
        //string tmp = strping + strip[i];
       //关于linux的system函数返回值可点击查看
        if (!system("ping -c 3  192.168.33.33")) {
            printf("ping %s,success!\n","192.168.33.33");
        } else {
			printf("ping %s,failed!\n","192.168.33.33");
           
        }
    sleep(2);
    }
   return 0;
}
