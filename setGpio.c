//本代码使用system命令的方式对GPIO进行端口方向及电平设置
//若后面使用文件IO方式参考博客：https://www.cnblogs.com/zym0805/p/5814351.html


#include <stdio.h>
#include <string.h>
typedef unsigned char u8;

//int转string
void itoa(int i,char*string)
{
    int power,j;
    j=i;
    for(power=1;j>=10;j/=10)
        power*=10;
    for(;power>0;power/=10)
    {
        *string++='0'+i/power;
        i%=power;
    }
    *string='\0';
}

//设置端口电平为高 
void Gpio_Set_High(int N)
{

    char str[10];
    char str5[50]="echo 1 >/sys/class/gpio/gpio";
    char str6[20]="/value";
    itoa(N,str);
    strcat(str5,str);
    strcat(str5,str6);
    //system(str5);

	printf("%s\n",str5);
}
//设置端口电平为低
void Gpio_Set_Low(int N)
{
    char str[10];
    char str5[50]="echo 0 >/sys/class/gpio/gpio";
    char str6[20]="/value";
    itoa(N,str);
    strcat(str5,str);
    strcat(str5,str6);
    //system(str5);

	printf("%s\n",str5);
}

//设置端口方向为输出
void Gpio_Set_DirectionOut(int pin)
{
	char str[10];
    char str5[50]="echo out > /sys/class/gpio/gpio";
    char str6[20]="/direction";
    itoa(pin,str);
    strcat(str5,str);
    strcat(str5,str6);
    //system(str5);

	printf("%s\n",str5);
}

//设置端口方向为输入  
void Gpio_Set_DirectionIn(int pin)
{
	char str[10];
    char str5[50]="echo in > /sys/class/gpio/gpio";
    char str6[20]="/direction";
    itoa(pin,str);
    strcat(str5,str);
    strcat(str5,str6);
    //system(str5);

	printf("%s\n",str5);
}


//设置端口值：1为高电平 0为低电平
void Set_Gpio_value(int num,u8 value)
{
	if(value == 1) Gpio_Set_High(num);
	else if(value == 0) Gpio_Set_Low(num);
}

//设置端口方向：1为输出 0为输入
void Set_Gpio_Direction(int num,u8 value)
{
	if(value == 1) Gpio_Set_DirectionOut(num);
	else if(value == 0) Gpio_Set_DirectionIn(num);
}

int main()
{
	Set_Gpio_Direction(984,1);//输出
    Set_Gpio_value(984,1);//高电平
	return 0;
}
