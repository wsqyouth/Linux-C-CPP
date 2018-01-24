
控制GPIO的目录/sys/class/gpio
/sys/class/gpio/export文件用于通知系统需要导出控制的GPIO引脚编号
/sys/class/gpio/unexport 用于通知系统取消导出
/sys/class/gpio/gpiochipX目录保存系统中GPIO寄存器的信息，包括每个寄存器控制引脚的起始编号base，寄存器名称，引脚总数 导出一个引脚的操作步骤
direction文件，定义输入输入方向，可以通过下面命令定义为输出。direction接受的参数：in, out, high, low。high/low同时设置方向为输出，并将value设置为相应的1/0
value文件是端口的数值，为1或0

1. 导出

/sys/class/gpio# echo 44 > export

2. 设置方向

/sys/class/gpio/gpio44# echo out > direction

3. 查看方向

/sys/class/gpio/gpio44# cat direction

4. 设置输出

/sys/class/gpio/gpio44# echo 1 > value

5. 查看输出值

/sys/class/gpio/gpio44# cat value

6. 取消导出

/sys/class/gpio# echo 44 > unexport

////////////////////////////////////////////////////////////

#include <stdio.h>  

#include <stdlib.h>  

#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>   //define O_WRONLY and O_RDONLY  

void initGpio(int n)  
{  
    FILE * fp =fopen("/sys/class/gpio/export","w");  
    if (fp == NULL)  
        perror("export open filed");  
    else  
        fprintf(fp,"%d",n);  
    fclose(fp);  
}   //create gpio file  

void setGpioDirection(int n,char *direction)  
{  
    char path[100] = {0};  
    sprintf(path,"/sys/class/gpio/gpio%d/direction",n);  
    FILE * fp =fopen(path,"w");  
    if (fp == NULL)  
        perror("direction open filed");  
    else  
        fprintf(fp,"%s",direction);  
    fclose(fp);  
}   //set gpio "in" or "out"  

int getGpioValue(int n)  
{  
    char path[64];  
    char value_str[3];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", n);  
    fd = open(path, O_RDONLY);  
    if (fd < 0) {  
        perror("Failed to open gpio value for reading!");  
        return -1;  
    }  

    if (read(fd, value_str, 3) < 0) {  
        perror("Failed to read value!");  
        return -1;  
    }  

    close(fd);  
    return (atoi(value_str));  
}   //get gpio(n)'s value  


int main()  

{  
    initGpio(18);  
    setGpioDirection(18,"in");  
    while(1)  
    {  
        printf("%d\n",getGpioValue(18));//每隔1s输出一次gpio18的值  
        sleep(1);  
    }  

    return 0;  

}  
