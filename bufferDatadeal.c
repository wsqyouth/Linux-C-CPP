
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h> 


//本函数模拟对一个buffer进行打印，写入，同步，延时
int write_protocol(const unsigned char* buf, int n)
{
    
    int rc = 0;
    int i = 0;
    
    fprintf(stderr,"print,write,sync,sleep>>>>>>>>>>>>>>>>test\n");
    for(i = 0; i < n; i++){
       
        printf("%02X ", buf[i]);
       
        
    }
    printf("\n");
    //rc = write(uart_fd, buf, n);  //模拟write
    sync(); //同步 
    usleep(5000);//延迟5ms

    return rc;
}

//处理函数最好做成回调函数，在引用文件中直接用 extern 来注明外部函数即可。

int main()
{
	unsigned char buffer[6] = {0x12, 0x32,0x56,0x78,0x90,0xab};
	write_protocol(buffer,6);

	return 0;

}
