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
