#include <stdio.h>


/*
*function：ftoc(float fvalue,unsigned char*arr)
*decription:  浮点数转化成四个字节
*input: 浮点数 
*output: 4个字节的字节数组
*/
void ftoc(float fvalue,unsigned char*arr) //例如12.5--0x41 48 00 00;转换完之后，arr[0]-00，arr[1]-00，arr[2]-48，arr[3]-41
{
	unsigned char  *pf;
	unsigned char *px;
	unsigned char i;   //计数器 
	pf =(unsigned char *)&fvalue;            /*unsigned char型指针取得浮点数的首地址*/  
	px = arr;                               /*字符数组arr准备存储浮点数的四个字节,px指针指向字节数组arr*/
	
	for(i=0;i<4;i++)
	{
		*(px+i)=*(pf+i);     /*使用unsigned char型指针从低地址一个字节一个字节取出*/
	}
}

/* 
*function：float ByteToFloat(unsigned char* byteArray) 
*decription:  将字节型转化成32bits浮点型 
*input:       长度为4的字节数组 
*output: 
*/  
float ByteToFloat(unsigned char* byteArray)  
{  
    return *((float*)byteArray);  
}  

int main(int argc, char *argv[])
{
   
    int i;
    unsigned char byteArray[4];
    ftoc(-12.031639,byteArray);
    
    for(i=0;i<4;i++)
    	printf("%x  ",byteArray[i]);
    
    float x=0;
    x = ByteToFloat(byteArray);
    printf("\n%f  ",x);
    
    
    return 0;
}
