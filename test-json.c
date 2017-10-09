#include <stdio.h>
#include <json-c/json.h>

/*******************************************************************
 * json-test.c测试功能：
 * 
 * 1. 创建一个json对象
 * 2. 给json对象加入成员，并且赋值
 * 3. 以字符串的形式输出json对象
 * 4. 将json格式的字符串转成json对象
 * 5. 获取json对象的成员值
 * 
 * 编译：
 * x86平台：gcc json-test.o json-test -ljson-c
 * ARM平台：需要指定交叉编译，通过 -I 指定头文件所在路径 -L指定库所在路径 -l指定库名
 * 
 * 注意：
 *编译时，会提示"/usr/include/json/json.h:27:34: fatal error: json_object_iterator.h: No such file or directory",
 *原因是我们在JSON库安装的时候,没有将json源码下的" json_object_iterator.h"头文件安装到/usr/include/json目录下,
 *解决的方法很简单，我们只需要将json源码树下的" json_object_iterator.h"拷贝到/usr/include/json目录下就可以了。
 ******************************************************************
 */
 

 void print_json_object_member(json_object * new_object, const char *field)
 {
     //根据指定对象的成员名，获取这个成员对象的json对象
     struct json_object *object = json_object_object_get(new_object , field);
     //获取json对象的类型
     enum json_type object_type = json_object_get_type(object);

     switch(object_type){
        case json_type_int:
            printf("new_obj:%s, json_object_get_type[]=%s\n",field,json_type_to_name(object_type));
            printf("new_obj:%s, json_object_get_int[]=%d\n",field,json_object_get_int(object));
            break;
        case json_type_string:
            printf("new_obj:%s json_object_get_type[]=%s\n",field,json_type_to_name(object_type));
            printf("new_obj:%s json_object_get_string[]=%s\n",field,(char *)json_object_get_string(object));
            break;
        default:
            break;
     }

     printf("\n");

     //释放json对象
     json_object_put(object);
 }

 void print_json_object(json_object * m_ojbect)
 {
     print_json_object_member(m_ojbect,"one");
     print_json_object_member(m_ojbect,"two");
     print_json_object_member(m_ojbect,"three");
     print_json_object_member(m_ojbect,"name");
     print_json_object_member(m_ojbect,"id");
 }

 int main(int argc, const char *argv[])
 {
     json_object *my_object;
     json_object *new_object;
     unsigned char *my_object_sring;
     //unsigned char json_to_string[1024];

     //创建一个空的json对象
     my_object = json_object_new_object();

     //以key-value形式添加json对象成员
     json_object_object_add(my_object,"one",json_object_new_int(10));
     json_object_object_add(my_object,"two",json_object_new_int(20));
     json_object_object_add(my_object,"three",json_object_new_int(30));
     json_object_object_add(my_object,"name",json_object_new_string("paopao"));
     json_object_object_add(my_object,"id",json_object_new_string("myid"));

     //将json对象内容，转成json格式的字符串
     my_object_sring = (char *)json_object_to_json_string(my_object);
    printf("my_object to string= %s\n",my_object_sring);

     //将json格式的字符串转换成json对象
     new_object = json_tokener_parse(my_object_sring);
     printf("new_object=%s\n",json_object_to_json_string(new_object));

     //输出json格式的成员
  // print_json_object(my_object);
     print_json_object(new_object);

     //释放json对象
     json_object_put(my_object);
     json_object_put(new_object);

     return 0;
 }
