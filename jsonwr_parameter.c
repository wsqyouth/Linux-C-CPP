#include "json-c/json.h"

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>   
#include <strings.h>  

#define CONFIG_FILE "config.json" 

 void print_json_object_member(json_object * new_object, const char *field)
 {
     //根据指定对象的成员名，获取这个成员对象的json对象
     struct json_object *object = json_object_object_get(new_object , field);
     //获取json对象的类型
     enum json_type object_type = json_object_get_type(object);

     switch(object_type){
        case json_type_int:
 
            printf("new_obj:%s, type=%s,int_value=%d\n",field,json_type_to_name(object_type),json_object_get_int(object));
            break;
        case json_type_string:
           
            printf("new_obj:%s, type=%s, string_value=%s\n",field,json_type_to_name(object_type),(char *)json_object_get_string(object));
            break;
        default:
            break;
     }

     printf("\n");

     //释放json对象
     json_object_put(object);
 }


void write_para()
{
    //参数节点
    struct json_object *para_object;
    //创建一个空的json对象
    para_object = json_object_new_object();  
    //添加json名称和值到json对象集合中
    json_object_object_add(para_object, "AIp", json_object_new_string("192.168.0.33")); 
    json_object_object_add(para_object, "APort", json_object_new_int(8000));
    json_object_object_add(para_object, "BIp", json_object_new_string("192.168.0.34")); 
    json_object_object_add(para_object, "BPort", json_object_new_int(8000));
    //显示json对象字符串
    printf("write: %s\n", json_object_to_json_string(para_object));
    //存储json对象
    json_object_to_file(CONFIG_FILE,para_object);  
    //释放json对象
    json_object_put(para_object);
}


int main(int argc, char *argv[])
{
    
    struct json_object *read_object;    
    /*************写入参数*************/
    write_para();
    /*************读取参数*************/
    //从存储文件获取json对象
    read_object = json_object_from_file(CONFIG_FILE);
    printf("read: \n");

    //json object to string
    printf("\t%s\n", json_object_to_json_string(read_object));
    //analysis valid data
    print_json_object_member(read_object,"AIp");
    print_json_object_member(read_object,"APort");
    print_json_object_member(read_object,"BIp");
    print_json_object_member(read_object,"BPort");
    //free json object
    json_object_put(read_object);
    
    return 0;
}


-----
write: { "AIp": "192.168.0.33 ", "APort": 8000, "BIp": "192.168.0.34 ", "BPort": 8000 }
read: 
	{ "AIp": "192.168.0.33 ", "APort": 8000, "BIp": "192.168.0.34 ", "BPort": 8000 }
new_obj:AIp, type=string, string_value=192.168.0.33 
new_obj:APort, type=int,int_value=8000
new_obj:BIp, type=string, string_value=192.168.0.34 
new_obj:BPort, type=int,int_value=8000
