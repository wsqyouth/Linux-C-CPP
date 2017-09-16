#include <stdio.h>
#include <stdlib.h>
/*
This program is only adjust to the static array, 
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
	u8 len = sizeof(recv_buf)/sizeof(recv_buf[0]);
	//printf("%d\r\n",len);
    u16 packet_head;  	//包头
	u32 packet_len;		//整包长度
	u16 source_adress;	//源地址
	u16 dest_adress;	//目的地址 
	u16 station_num;	//站点号 
	u16 cmd_num;		//命令号
	u8	cmd_content;	//内容
	u16 packet_tail;	//包尾
	
	for(i=0; i< len;i++)
	{
		printf("%d --%x\r\n",i,recv_buf[i]);
	}
	//起始地址作为这个字节的低地址 
	packet_head   = *(u16*)(&recv_buf[0]);  //0-1
	packet_len    = *(u32*)(&recv_buf[2]);  //2-5
	//如果使用malloc重新接收数据，后面所有下标均-6，从0开始 
	source_adress = *(u16*)(&recv_buf[6]);  //6-7
	dest_adress   = *(u16*)(&recv_buf[8]);  //8-9
	station_num   = *(u16*)(&recv_buf[10]); //10-11
	cmd_num  	  = *(u16*)(&recv_buf[12]); //12-13
	
	cmd_content   = *(u8*)(&recv_buf[14]);  //14
	packet_tail   = *(u16*)(&recv_buf[15]); //15-16
	
	printf("--------------------------\r\n"); 
	printf("包头: 0x%04x\r\n",packet_head);  //16进制数据，高位补零
	printf("整包长度: 0x%08x\r\n",packet_len);
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
