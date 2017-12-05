#include <stdio.h>

#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>   //define O_WRONLY and O_RDONLY

void initGpio(int n)
{
    FILE * fp =fdopen("/sys/class/gpio/export","w");
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
    FILE * fp =fdopen(path,"w");
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
        printf("%d\n",getGpioValue(18));<span style="white-space:pre">	</span>//每隔1s输出一次gpio18的值
        sleep(1);
    }

    return 0;

}
