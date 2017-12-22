//该程序测试将十进制的字节数组（表示时间），转换为十进制，进而转换为16进制放入buf内
//例如： 一天86400秒。 23:99:99 -> 86400 -> 15180。此时在buf由低地址到高地址依次存放为80 51 01 00
#include <stdio.h>
#include <stdlib.h>
#include<string.h> 
typedef unsigned char u8;
typedef unsigned int u32;


u8 send_buf[2048]; //填充的全局数组buf


int getSecond(u8 arr[],int len)
{
	for(int i=0;i<len;i++)
	{
		if(arr[i]<'0' && arr[i]>'9')
		{
			printf("time format illeagl\n");
			return -1;
		}
	} 
	int hour = (arr[0]-'0')*10 + arr[1]-'0';
	int min =  (arr[2]-'0')*10 + arr[3]-'0';
	int sec =  (arr[4]-'0')*10 + arr[5]-'0';
	
	int totalSec = hour*3600 + min*60 + sec;
	
	return totalSec;
}

//打印测试
void printArr(u8 arr[], int len)
{
    for(int i=0; i<len; i++)
    {
        printf("%x ",arr[i]);
    }
    printf("\n-----------------------------------------------\n");
}

int main()
{
	u8 gps_buf[] = {'0','8','2','3','4','9'};
	
	int totalSec = getSecond(gps_buf,6);
	
	printf("seconds:%d\n",totalSec); //30299 
	
	
    *(u32*)(&send_buf[0]) = 86400;  //一天86400秒 
    printArr(send_buf, 5); //放入四字节数据内 
}
