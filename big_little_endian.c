#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	union{
		short s;
		char c[sizeof(short)];
	}un;
	
	un.s = 0x0102;
	if( sizeof(short) == 2)
	{
		if(un.c[0] == 1 && un.c[1]== 2)   //将低序字节存储在起始地址，为小端字节序 
			printf("big-endian\n");
		else if(un.c[0] == 2 && un.c[1]== 1) //将高序字节地址存储在起始地址，成为大端字节序 
			printf("little-endian\n");
	}
	else
	{
		printf("sizeof(short)= %d\n",sizeof(short)); 
	}
	
	
	system("pause");
	return 0;
}
