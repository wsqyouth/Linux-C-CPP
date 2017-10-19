 
#include "json-c/json.h"

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>   
#include <strings.h>  

#define CONFIGBOARD_FILE "configBoard.json"
#define CONFIGSERVER_FILE "configServer.json"
//
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

//声明一个结构体类型BoardPara, 包括所有开发板启动时所需配置属性
 typedef struct Board{       
 u16 Board_StationCmd; 	     //开发板的站点号(手动)
 u8 Board_AIp[20];   	     //开发板A的IP字符串
 u8 Board_BIp[20];    	     //开发板B的IP字符串

 u8 Server_masterAip[20];    //主服务器A的IP字符串
 u16 Server_masterAport;      //主服务器A的端口号(手动)
 u8 Server_masterBip[20];    //主服务器B的IP字符串
 u16 Server_masterBport;      //主服务器B的端口号(手动)
}BoardPara;

/*
*
*新建文件句柄,将开发板A,B的IP地址，以及站点号写入配置文件
*Eg:write_para_board(6,"192.168.0.33","192.168.0.34");  
*/
void write_para_board(u8 Station_cmd,u8 *Aip_str,u8 *Bip_str)
{
    //参数节点
    struct json_object *para_object;
    //创建一个空的json对象
    para_object = json_object_new_object();
    
    //添加json名称和值到json对象集合中
    json_object_object_add(para_object, "Board_StationCmd", json_object_new_int(Station_cmd));

    json_object_object_add(para_object, "Board_AIp", json_object_new_string(Aip_str));
    //json_object_object_add(para_object, "APort", json_object_new_int(8000));
    json_object_object_add(para_object, "Board_BIp", json_object_new_string(Bip_str));
    //json_object_object_add(para_object, "BPort", json_object_new_int(8000));
    //显示json对象字符串
    printf("write board all: %s\n", json_object_to_json_string(para_object));
    //存储json对象
    json_object_to_file(CONFIGBOARD_FILE,para_object);
    //释放json对象
    json_object_put(para_object);
}

/*
*
*新建文件句柄,将主服务器A,B服务器的IP，端口号写入配置文件
*Eg:write_para_Server("192.168.0.33",6001,"192.168.0.34",6001); 
*/
void write_para_Server(u8 *Server_masterAip,u16 Server_masterAport, u8 *Server_masterBip,u16 Server_masterBport)
{
    //参数节点
    struct json_object *para_object;
    //创建一个空的json对象
    para_object = json_object_new_object();
    //添加json名称和值到json对象集合中
    json_object_object_add(para_object, "Server_masterAip", json_object_new_string(Server_masterAip));
    json_object_object_add(para_object, "Server_masterAport", json_object_new_int(Server_masterAport));
    json_object_object_add(para_object, "Server_masterBip", json_object_new_string(Server_masterBip));
    json_object_object_add(para_object, "Server_masterBport", json_object_new_int(Server_masterBport));
    //显示json对象字符串
    printf("write master server all: %s\n", json_object_to_json_string(para_object));
    //存储json对象
    json_object_to_file(CONFIGSERVER_FILE,para_object);
    //释放json对象
    json_object_put(para_object);
}

/*
*
*将主服务器A,B服务器的IP写入配置文件
*Eg:write_para_ServerIp("192.168.0.33","192.168.0.34"); 
*/
void write_para_ServerIp(u8 *Server_masterAip,u8 *Server_masterBip)
{
    //参数节点
    struct json_object *para_object;
    //创建一个空的json对象
    //para_object = json_object_new_object();
    //从存储文件获取json对象
    para_object = json_object_from_file(CONFIGSERVER_FILE);
   if(para_object != NULL)
    {
	    //添加json名称和值到json对象集合中
	    json_object_object_add(para_object, "Server_masterAip", json_object_new_string(Server_masterAip));
	    json_object_object_add(para_object, "Server_masterBip", json_object_new_string(Server_masterBip));

	    //显示json对象字符串
	    printf("write master server Ip: %s\n", json_object_to_json_string(para_object));
	    //存储json对象
	    json_object_to_file(CONFIGSERVER_FILE,para_object);
	    //释放json对象
	    json_object_put(para_object);
   }else
   {
	printf("configServer.json not found!\n");
   }
}


/*
*
*从配置文件获取句柄,将开发板A,B的IP地址写入配置文件
*Eg:write_para_board(6,"192.168.0.33","192.168.0.34");  
*/
void write_para_boardIp(u8 *Aip_str,u8 *Bip_str)
{
    //参数节点
    struct json_object *para_object;
    //从存储文件获取json对象
    para_object = json_object_from_file(CONFIGBOARD_FILE);
    if(para_object != NULL)
    {
	
	    //添加json名称和值到json对象集合中
	   
	    json_object_object_add(para_object, "Board_AIp", json_object_new_string(Aip_str));
	    //json_object_object_add(para_object, "APort", json_object_new_int(8000));
	    json_object_object_add(para_object, "Board_BIp", json_object_new_string(Bip_str));
	    //json_object_object_add(para_object, "BPort", json_object_new_int(8000));
	    //显示json对象字符串
	    printf("write borad Ip: %s\n", json_object_to_json_string(para_object));
	    //存储json对象
	    json_object_to_file(CONFIGBOARD_FILE,para_object);
	    //释放json对象
	    json_object_put(para_object);
   }else
   {
	printf("configBoard.json not found!\n");
   }
}

void GetStringValByKey(json_object * jobj, const  char  *sname, char *buf)
{
    json_object     *pval = NULL;
    enum json_type type;
    pval = json_object_object_get(jobj, sname);
    if(NULL!=pval){
        type = json_object_get_type(pval);
	if(type ==json_type_string)
	{
	   //printf("Key:%s  value: %s\n", sname, json_object_get_string(pval));
	   //must be string
	   strcpy(buf,json_object_get_string(pval));	
	}
    }
}

int GetIntValByKey(json_object * jobj, const  char  *sname)
{
    json_object     *pval = NULL;
    enum json_type type;
    pval = json_object_object_get(jobj, sname);
    if(NULL!=pval){
        type = json_object_get_type(pval);
	if(type ==json_type_int)
	{
	   //printf("Key:%s  value: %d\n", sname, json_object_get_int(pval));
	   //return int result
	   return json_object_get_int(pval);	
	}
    }
     return -1;
}

/*
*
*从配置文件读取服务器A,B的IP至结构体
*/
void read_para_Server(BoardPara * boardPara)
{
   struct json_object *read_object;
   //从存储文件获取json对象
    read_object = json_object_from_file(CONFIGSERVER_FILE);
    if(read_object != NULL)
    {
		printf("read Board: \n");

		//json object to string
		printf("\t%s\n", json_object_to_json_string(read_object));
		//analysis valid data
		//GetValByKey(read_object,"STATION_CMD"); //第三个参数有问题，先不管
		//bzero(Aip_setstr,sizeof(Aip_setstr));
		GetStringValByKey(read_object,"Server_masterAip",boardPara->Server_masterAip);
		boardPara->Server_masterAport=GetIntValByKey(read_object,"Server_masterAport");
		//print_json_object_member(read_object,"APort");
		//bzero(Bip_setstr,sizeof(Bip_setstr));
		GetStringValByKey(read_object,"Server_masterBip",boardPara->Server_masterBip);
		boardPara->Server_masterBport=GetIntValByKey(read_object,"Server_masterBport");
		//print_json_object_member(read_object,"BPort");
		//free json object
		json_object_put(read_object);
  }
}
/*
*
*从配置文件读取读开发板A,B的IP以及站点号至结构体
*/
void read_para_board(BoardPara * boardPara)
{
   struct json_object *read_object;
   //从存储文件获取json对象
    read_object = json_object_from_file(CONFIGBOARD_FILE);
    if(read_object != NULL)
    {
		printf("read: Server \n");

		//json object to string
		printf("\t%s\n", json_object_to_json_string(read_object));
		//analysis valid data
		boardPara->Board_StationCmd=GetIntValByKey(read_object,"Board_StationCmd"); //开发板的站点号
		//bzero(Aip_setstr,sizeof(Aip_setstr));
		GetStringValByKey(read_object,"Board_AIp",boardPara->Board_AIp);//开发板A的IP字符串
		
		//bzero(Bip_setstr,sizeof(Bip_setstr));
		GetStringValByKey(read_object,"Board_BIp",boardPara->Board_BIp); //开发板B的IP字符串
		
		//free json object
		json_object_put(read_object);
    }
}

/*
*检查配置文件configServer.json是否存在，如果不存在则初始化生成
*/
void check_fileServer_init()
{
   struct json_object *read_object;
   //从存储文件获取json对象
    read_object = json_object_from_file(CONFIGSERVER_FILE);
    if(read_object == NULL)
    {	
	 write_para_Server("192.168.33.35",8000,"192.168.34.35",8000); //用来标准化配置文件格式用
    }
}
/*
*检查配置文件configBoard.json是否存在，如果不存在则初始化生成
*/
void check_fileBoard_init()
{
   struct json_object *read_object;
   //从存储文件获取json对象
    read_object = json_object_from_file(CONFIGBOARD_FILE);
    if(read_object == NULL)
    {	
	  write_para_board(6,"192.168.33.66","192.168.34.66");  //用来标准化配置文件格式用
    }
}
int main()
{
	  /*运行应用程序之前检查配置文件是否存在，若不存在则生成配置文件*/
	 check_fileBoard_init();
	 check_fileServer_init();

	 write_para_ServerIp("192.168.33.33","192.168.34.34"); //从配置文件获取句柄，并修改服务器IP参数
	 write_para_boardIp("192.168.33.66","192.168.34.66");//从配置文件获取句柄，并修改开发板IP参数
         BoardPara  boardpara;
         read_para_Server(&boardpara);
	 //set ipA and ipB
		printf("Server_masterAip:%s\n",boardpara.Server_masterAip);
		printf("Server_masterAport:%d\n",boardpara.Server_masterAport);
		printf("Server_masterBip:%s\n",boardpara.Server_masterAip);
		printf("Server_masterBport:%d\n",boardpara.Server_masterBport);
         read_para_board(&boardpara);
		printf("Board_StationCmd:%d\n",boardpara.Board_StationCmd);
		printf("Board_AIp:%s\n",boardpara.Board_AIp);
		printf("Board_BIp:%s\n",boardpara.Board_BIp);
	return 0;
}
