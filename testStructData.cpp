//利用取消的结构体对齐存储数据，之后完成数据的拼包与解包操作。

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
//#include <thread>
#include <unistd.h>
#include <stdint.h>

#define START_FLAG 0x7E7E
#define END_FLAG  0xA5A5

typedef enum _OPERATION_CODE{
  SET_CMD_REQ = 0x00,
  QUERY_CMD_REQ = 0x01,
  SET_CMD_RSP = 0x80,
  QUERY_CMD_RSP = 0x81,
  STOP_CTRL_CMD=0xf0,
  FILE_DOWNLOAD_CMD=0xf1,
}OPERATION_CODE;


void display_data(uint8_t *buf ,uint16_t len){
    if(buf){
        int i;
        printf("\n-----buf start------\n");
        for(i=0;i<len;i++){
        
            printf(" %02x",buf[i]);
        }
        printf("\n-----end------\n");
    }else{
        printf("buf is null\n");
    }
}

typedef struct  _PDU_RSP_HEADER{
    uint16_t start_flag;
    uint8_t operation;
    uint8_t code;
    uint16_t len;
    uint32_t data;
    uint16_t crc;
    uint16_t end_flag;
}__attribute__ ((packed)) PDU_CFG_RSP_HEADER_ST;

int main( )
{
	printf("receive data len:%ld\n",sizeof(PDU_CFG_RSP_HEADER_ST));
	PDU_CFG_RSP_HEADER_ST *cfg_hdr = NULL;
	PDU_CFG_RSP_HEADER_ST cfg_cmd_ack={0};
    

     //fill in
	   cfg_cmd_ack.start_flag = START_FLAG;
	   cfg_cmd_ack.operation = SET_CMD_RSP;
     cfg_cmd_ack.code = 0x11;
     cfg_cmd_ack.end_flag = END_FLAG;
     cfg_cmd_ack.len =4;

    uint8_t *buf = reinterpret_cast<uint8_t *>(&cfg_cmd_ack);
  	display_data(buf,sizeof(PDU_CFG_RSP_HEADER_ST));

    //fetch out
	  cfg_hdr= reinterpret_cast<PDU_CFG_RSP_HEADER_ST *>(buf);
	  printf("%x\n",cfg_hdr->start_flag);
	  printf("%x\n",cfg_hdr->operation);
	  printf("%x\n",cfg_hdr->code);
	  printf("%x\n",cfg_hdr->len);
	  printf("%x\n",cfg_hdr->end_flag);
	  
    return 0;
}


输出：
g++ testStruct.cpp -o testStruct
./testStruct 
receive data len:14

-----buf start------
 7e 7e 80 11 04 00 00 00 00 00 00 00 a5 a5
-----end------
7e7e
80
11
4
a5a5
