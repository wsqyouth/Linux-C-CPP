#include <stdio.h>
#include <stdlib.h>
/*
函数指针,是指向printf函数的指针,最前面的int是函数的返回值类型,
中间的(*sh)表明这个是一个指针,(char*str,...)这个是函数的参数,
返回值和参数必须和被指向的函数一样.

根据C++ Primer Plus 说法，函数指针格式就是把函数名换为(*ptr),其他不变
*/
void myprint(const char *str)
{
	printf("----%s\n",str);
}
int main()
{
	void (*sh)(const char *)=myprint;
	//sh = myprint;
	sh("hello world");
	system("pause");
	return 0;
}
