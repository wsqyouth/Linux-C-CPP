#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
int main(int argc, char *argv[])
{
    char ip1[] = "192.168.0.74";
    char ip2[] = "211.100.21.179";
    struct in_addr addr1, addr2;
    long l1, l2;
    l1 = inet_addr(ip1);   //IP字符串——》网络字节  
    l2 = inet_addr(ip2);
    printf("IP1: %s\nIP2: %s\n", ip1, ip2);
    printf("Addr1: %ld\nAddr2: %ld\n", l1, l2);

    memcpy(&addr1, &l1, 4); //复制4个字节大小  
    memcpy(&addr2, &l2, 4);
    printf("%s <--> %s\n", inet_ntoa(addr1), inet_ntoa(addr2)); //注意：printf函数自右向左求值、覆盖  
    printf("%s\n", inet_ntoa(addr1)); //网络字节 ——》IP字符串  
    printf("%s\n", inet_ntoa(addr2));
    return 0;
}
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
~                                                                                                                                                                                     
-- 插入 --                                                                                                                                                          1,1          全部
