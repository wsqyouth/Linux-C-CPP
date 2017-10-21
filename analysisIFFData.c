#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

u8 send_buf[76];

void IFF_Data_Package(u16 src_addr,u16 taget_addr)
{

   u8 TOA;
   u8 taget;
   send_buf[0] = 0x7e; send_buf[1] = 0x7e;   //7E 7E 
   send_buf[2] = 0x40;//整包长度，４字节 //40 00 00 00 
   send_buf[3] = 0x0;
   send_buf[4] = 0x0;
   send_buf[5] = 0x0;
   *(u16*)(&send_buf[6]) = src_addr;  //29 00 
   *(u16*)(&send_buf[8]) = taget_addr;//65 00 
   *(u16*)(&send_buf[10]) = 0x01E5;//命令号，站点号，２字节  //
   *(u16*)(&send_buf[12]) = 0x0191;//命令码,２字节 //0x0191
   send_buf[14] = 0x03; //目的 // taget
   *(u32*)(&send_buf[15]) = 0x38E81D6B;  //4字节GPS时间，８个字节// 上 6B 1D E8 38 5F 01 00 00 
   *(u32*)(&send_buf[19]) = 0x0000015F ; //4字节GPS时间，８个字节// 下 
   send_buf[23] = 0x00; //TOA精度标志，１字节  // TOA
   *(u32*)(&send_buf[24]) = 0x00000001; //编码数据个数，４字节 // 01 00 00 00 
   *(u32*)(&send_buf[28]) = 0x0BEBC200; //晶振震荡次数，４字节//pps_data 00 C2 EB 0B 
   *(u16*)(&send_buf[32]) = 0x00A8; //IFF模式，２字节 //A8 00 
   *(u32*)(&send_buf[34]) = 0x54800004; //IFF信号编码１，４字节// code0  04 00 80 54 
   *(u32*)(&send_buf[38]) = 0x003422EF; //IFF信号编码２，４字节// code1  EF 22 34 00 
   *(u32*)(&send_buf[42]) = 0xFFFFFFFF; //IFF信号编码３，４字节// code2  FF FF FF FF 
   *(u32*)(&send_buf[46]) = 0x00FFFFFF; //IFF信号编码４，４字节// code3  FF FF FF 00 
   *(u32*)(&send_buf[50]) = 0x3B70ECA8; //到达时间TOA，４字节// start    A8 EC 70 3B 
   *(u32*)(&send_buf[54]) = 0x000039E8; //幅度，４字节// code_abs  E8 39 00 00 
   //printf("code_adbs = %d \r\n",*(u32*)(&send_buf[54]));
   *(u32*)(&send_buf[58]) = 0x070087B8;  //时间节拍，４字节 // B8 87 00 07
   *(u16*)(&send_buf[62]) = 0x0a0d; //帧尾 0D 0A

 
}

void print(u8 arr[],int len)
{
	for(int i=0;i<len;i++)
	{
		printf("%02x\t",arr[i]);
	}
	printf("\n"); 
}
//低地址字节在小坐标(低地址) 
int main()
{   IFF_Data_Package(0x0029,0x0065);
 	
	
	u8 data[64]; 
	memcpy(data,send_buf,64*sizeof(u8)); 
	print(data,64);

	u16 IFF_Mode;
	u32 IFF_Code0,IFF_Code1,IFF_Code2,IFF_Code3;
	u32 IFF_Toa;
	u32 IFF_Amp;
	u32 IFF_Tbeat;
	//如果使用malloc重新接收数据，后面所有下标均-6，从0开始
	//*(u16*)(&send_buf[32]) = 0x00A8; //IFF模式，２字节 //A8 00
	IFF_Mode = *(u16*)(&data[32]);  //32-33
	//*(u32*)(&send_buf[34]) = 0x54800004; //IFF信号编码１，４字节// code0  04 00 80 54 
	IFF_Code0 = *(u32*)(&data[34]); //34-37
    //*(u32*)(&send_buf[38]) = 0x003422EF; //IFF信号编码２，４字节// code1  EF 22 34 00 
    IFF_Code1 = *(u32*)(&data[38]); //38-41
    //*(u32*)(&send_buf[42]) = 0xFFFFFFFF; //IFF信号编码３，４字节// code2  FF FF FF FF 
    IFF_Code2 = *(u32*)(&data[42]); //42-45
    //*(u32*)(&send_buf[46]) = 0x00FFFFFF; //IFF信号编码４，４字节// code3  FF FF FF 00 
    IFF_Code3 = *(u32*)(&data[46]); //46-49
    //*(u32*)(&send_buf[50]) = 0x3B70ECA8; //到达时间TOA，４字节// start    A8 EC 70 3B 
    IFF_Toa  = *(u32*)(&data[50]); //50-53
    //*(u32*)(&send_buf[54]) = 0x000039E8; //幅度，４字节// code_abs  E8 39 00 00
    IFF_Amp = *(u32*)(&data[54]);  //54-57
    //*(u32*)(&send_buf[58]) = 0x070087B8;  //时间节拍，４字节 // B8 87 00 07
    IFF_Tbeat =  *(u32*)(&data[58]);
    
    printf("模式: 0x%04x\r\n",IFF_Mode);    //2个字节 
    printf("码字0: 0x%08x\r\n",IFF_Code0);  //4个字节 
    printf("码字1: 0x%08x\r\n",IFF_Code1);  //4个字节 
    printf("码字2: 0x%08x\r\n",IFF_Code2);  //4个字节 
    printf("码字3: 0x%08x\r\n",IFF_Code3);  //4个字节 
  	printf("时间： 0x%08x\r\n",IFF_Toa);    //4个字节 
    printf("幅度: 0x%08x\r\n",IFF_Amp);    //4个字节 
    printf("节拍：0x%08x\r\n",IFF_Tbeat);  //4个字节 
	return 0;
}
