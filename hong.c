打印日志是有代价的：
尤其是在嵌入式对时序要求较高时，打印影响系统性能。

------
第一个版本： 最简单的打印宏开关控制

#include <stdio.h>
#include <string.h>

#define __DEBUG

#ifdef __DEBUG
#define DEBUG(msg)  printf(msg)
#else
#define DEBUG(msg)
#endif

int main()
{
    DEBUG("hello world\n");
    return 0;
}

当我们进行了宏定义时，main函数打印了信息。如果将其注释，main函数没有信息打印出来。
但是此时会有问题，比如当使用DEBUG("hello %s\n","paopao")时，宏定义并不支持变参数。


-----
第二个版本：
C99标准对可变参数宏的支持： __VA_ARGS__
#include <stdio.h>
#include <string.h>

#define __DEBUG

#ifdef __DEBUG
#define DEBUG(msg,...)  printf(msg,##__VA_ARGS__)
#else
#define DEBUG(msg)
#endif

int main()
{
    DEBUG("hello %s\n","paopao");
    return 0;
}
