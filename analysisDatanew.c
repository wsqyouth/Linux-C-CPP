#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
This program can include two parts:
1. the first two types put in array :recv_buf
2. the last types can be copy a new malloc array :data
we can analysis efficient data according to specific data form! 
as follows:
包头
--7e
--7e
整包长度
--11
--00
--00
--00
源地址
--00
--00
目的地址
--00
--00
站点号
--00
--00
命令号
--06
--01
内容
--00
包尾
--0d
--0a
*/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;



u8 recv_buf[] = {0x7e,0x7e, 0x11,0x00,0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x06,0x01, 0x00, 0x0d,0x0a};
int main()
{
	u8 i = 0;
	//printf("%d\r\n",len);
	u8* data;
    u16 packet_head;  	//包头
	u32 packet_len;		//整包长度
	u16 source_adress;	//源地址
	u16 dest_adress;	//目的地址 
	u16 station_num;	//站点号 
	u16 cmd_num;		//命令号
	u8	cmd_content;	//内容
	u16 packet_tail;	//包尾
	
	for(i=0; i< 6;i++)
	{
		printf("%d --%x\r\n",i,recv_buf[i]);
	}
	//起始地址作为这个字节的低地址 
	packet_head   = *(u16*)(&recv_buf[0]);  //0-1
	packet_len    = *(u32*)(&recv_buf[2]);  //2-5
	printf("包头: 0x%04x\r\n",packet_head);  //16进制数据，高位补零
	printf("整包长度: 0x%08x\r\n",packet_len);
	data =(u8*) malloc(sizeof(u8)*packet_len);
	//模拟接收后面的数据 
	memcpy(data,recv_buf+6,packet_len-6);//从第7个字符(V)开始复制，连续复制17-6个字符(View)
	for(i=0; i< packet_len-6;i++)
	{
		printf("%d --%x\r\n",i,data[i]);
	}
	
	//如果使用malloc重新接收数据，后面所有下标均-6，从0开始 
	source_adress = *(u16*)(&data[0]);  //0-1
	dest_adress   = *(u16*)(&data[2]);  //2-3
	station_num   = *(u16*)(&data[4]); //4-5
	cmd_num  	  = *(u16*)(&data[6]); //6-7
	
	cmd_content   = *(u8*)(&data[8]);  //8
	packet_tail   = *(u16*)(&data[9]); //9-10
	
	printf("--------------------------\r\n"); 

	printf("源地址: 0x%04x\r\n",source_adress);
	printf("目的地址 : 0x%04x\r\n",dest_adress);
	printf("站点号: 0x%04x\r\n",station_num);
	printf("命令号: 0x%04x\r\n",cmd_num);
	printf("内容:0x%02x\r\n",cmd_content);
	printf("包尾: 0x%04x\r\n",packet_tail);
	printf("--------------------------\r\n"); 
	
	
	system("pause");
	return 0;
}
