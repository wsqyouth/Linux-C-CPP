/*
*extract specific numbers from some series in memory
*/
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

int main()
{
    printf("u8=%d,u16=%d,u32=%d\r\n",sizeof(u8),sizeof(u16),sizeof(u32));
    u32 x = 0x11223344;
    printf("---------取一个字节-----------\r\n");
    u8 *c =(u8 *)&x; 
	printf("x1=%x\n",*(c));
    printf("x2=%x\n",*(c+1));
    printf("x3=%x\n",*(c+2));
    printf("x4=%x\n",*(c+3));
    
    printf("x1=%x\n",*(u8*)(&x));
    printf("x2=%x\n",*((u8*)&(x)+1));
    printf("x3=%x\n",*((u8*)&(x)+2));
    printf("x4=%x\n",*((u8*)&(x)+3));
    
    printf("---------取两个字节-----------\r\n");
	u16 *d =(u16 *)&x; 
	printf("x1=%x\n",*(d));
    printf("x2=%x\n",*(d+1));
    
  	printf("x1=%x\n",*(u16*)(&x));
	printf("x2=%x\n",*((u16*)(&x)+1));
	
	printf("---------取四个字节-----------\r\n");
	//u32 *p =(u32*)&x;
	u32 *p =&x;  
	printf("x1=%x\n",*(p));
	
	printf("x1=%x\n",*(&x)); 
	system("pause");
	return 0;
}
