//该代码思路还是挺好的，将buf设置为一个队列类，互斥锁保护临界区，然后进行数据的各种操作，同时有内存不足时2倍扩容操作
//stream类的对象作为串口类的成员，优先使用组合而不是重载，理念很好。
//但是此时若串口类有多个，对应类成员对象Stream对象也有多个，应用场景不一样吧
//缺点：相对于目前的项目代码，将数据的锁保护放在了上层线程处，给代码的可阅读性带来一定的障碍


参考网址：
https://zhoujianshi.github.io/articles/2016/Linux%20%E4%B8%B2%E5%8F%A3%E6%93%8D%E4%BD%9C%EF%BC%88C++
%E5%AE%9E%E7%8E%B0%E5%90%8C%E6%AD%A5%E5%8F%91%E3%80%81%E5%BC%82%E6%AD%A5%E6%94%B6%E7%9A%84%E4%B8%B2%E5%8F%A3%E7%B1%BB%EF%BC%89
/index.html

Timestamp.h
----------
//本文件定义一个毫秒级的时间戳工具类
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

//时间戳类型
typedef long long timestamp_t;

class Timestamp
{

public:
    //获取以毫秒计的时间戳
    static timestamp_t now();

};

#endif

Timestamp.cpp
----------
#include "Timestamp.h"
#include <time.h>
#include <sys/time.h>

timestamp_t Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    timestamp_t time=(timestamp_t)tv.tv_sec*1000+tv.tv_usec/1000;
    return time;
}


Stream.h
---------
//本文件实现一个字符流
#ifndef STREAM_H
#define STREAM_H

class Stream
{

private:
    //缓冲区
    char* buffer;
    //缓冲区容量
    int capacity;
    //流的开头
    int start;
    //流的长度
    int length;

public:
    //初始化一个流，参数为：初始容量
    Stream(int initCapacity=16);
    //清理
    ~Stream();
    //获取流的长度
    int getLength();
    //向流的末尾增加一字符
    void append(char aChar);
    //向流的末尾增加多个字符
    void append(const char* buf,int len);
    //查看流的第一个字符，如果长度为0则返回0
    char peek();
    //查看流开头的多个字符，返回实际查看到的长度
    int peek(char* buf,int len);
    //取出流的第一个字符，如果长度为0则返回0
    char take();
    //取出流开头的多个字符，返回实际取出的长度
    int take(char* buf,int len);

private:
    //扩大容量至两倍
    void expand();

};

#endif
Stream.cpp
---------
#include "Stream.h"

Stream::Stream(int initCapacity)
{
    buffer=new char[initCapacity];
    capacity=initCapacity;
    start=0;
    length=0;
}

Stream::~Stream()
{
    delete[] buffer;
}

int Stream::getLength()
{
    return length;
}

void Stream::append(char aChar)
{
    if(length>=capacity)
        expand();
    int pos=start+length;
    if(pos>=capacity)
        pos-=capacity;
    buffer[pos]=aChar;
    length++;
}

void Stream::append(const char* buf,int len)
{
    for(int i=0;i<len;i++)
        append(buf[i]);
}

char Stream::peek()
{
    if(length==0)
        return 0;
    return buffer[start];
}

int Stream::peek(char* buf,int len)
{
    if(len>length)
        len=length;
    for(int i=0;i<len;i++)
    {
        int pos=start+i;
        if(pos>=capacity)
            pos-=capacity;
        buf[i]=buffer[pos];
    }
    return len;
}

char Stream::take()
{
    if(length==0)
        return 0;
    char aChar=buffer[start];
    start++;
    length--;
    if(start>=capacity)
        start-=capacity;
    return aChar;
}

int Stream::take(char* buf,int len)
{
    if(len>length)
        len=length;
    for(int i=0;i<len;i++)
        buf[i]=take();
    return len;
}

void Stream::expand()
{
    int newCapacity=capacity*2;
    char* newBuf=new char[newCapacity];
    int newLength=length;
    take(newBuf,newLength);
    delete[] buffer;
    buffer=newBuf;
    capacity=newCapacity;
    start=0;
    length=newLength;
}


Serial.h
---------
//本文件定义了一个串口类
#ifndef SERIAL_H
#define SERIAL_H

#include "Stream.h"
#include <pthread.h>

//串口类
class Serial
{

public:
    //无校验
    static const int PARITY_NONE=0;
    //奇校验
    static const int PARITY_ODD=1;
    //偶校验
    static const int PARITY_EVEN=2;
    //函数成功
    static const int OK=1;
    //设备未找到
    static const int DEV_NOT_FOUND=-1;
    //不支持该波特率
    static const int BAUD_NOT_SUPPORTED=-2;
    //不支持该数据位数
    static const int DATABITS_NOT_SUPPORTED=-3;
    //不支持该校验模式
    static const int PARITYMODE_NOT_SUPPORTED=-4;
    //不支持该停止位数
    static const int STOPBITS_NOT_SUPPORTED=-5;
    //未知配置错误
    static const int CONFIG_FAIL=-6;
    //创建线程出错
    static const int NEW_THREAD_FAIL=-7;
    //成功读到结尾符
    static const int READ_END=1;
    //读取超时
    static const int READ_TIMEOUT=-1;
    //读取时缓冲区满
    static const int READ_BUFFER_FULL=-2;

private:
    //串口设备文件描述符
    int fd;
    //字符流
    Stream stream;
    //后台接收线程
    pthread_t tid;
    //对字符流加的锁
    pthread_mutex_t mutex;

public:
    Serial();
    ~Serial();
    //开启串口，参数为：设备名、波特率、数据位数、校验模式、停止位数，返回函数执行结果
    int open(const char* dev,int baud,int dataBits,int parityMode,int stopBits);
    //关闭串口
    int close();
    //写串口，参数为：数据、长度，返回实际写入长度
    int write(const char* data,int len);
    //获取可读长度
    int available();
    //读串口，但不移除数据，返回实际读取长度
    int peek(char* buf,int len);
    //读串口，直到收到预期长度的数据或超时，参数为：接收缓冲区、预期接收长度、超时（毫秒）,返回实际读取长度
    int read(char* buf,int len,int timeout);
    //读串口，直到读到预期的结尾符或缓冲区满或超时，参数为：接收缓冲区、最大长度、预期结尾符、超时（毫秒）、实际接收长度，
    //返回READ_END、READ_TIMEOUT或READ_BUFFER_FULL
    int read(char* buf,int maxLen,const char* end,int timeout,int* recvLen);

private:
    //将数字型波特率转化为系统调用参数
    int transformBaud(int baud);
    //将数字型数据位数转化为系统调用参数
    int transformDataBits(int dataBits);
    long long getTimestamp();
    //判断字符串str是否以字符串end结尾
    bool endsWith(const char* str,int strLen,const char* end,int endLen);

    //后台接收线程函数
    friend void* receiveThread(void* arg);

};

#endif

Serial.cpp
---------
#include "Stream.h"

Stream::Stream(int initCapacity)
{
    buffer=new char[initCapacity];
    capacity=initCapacity;
    start=0;
    length=0;
}

Stream::~Stream()
{
    delete[] buffer;
}

int Stream::getLength()
{
    return length;
}

void Stream::append(char aChar)
{
    if(length>=capacity)
        expand();
    int pos=start+length;
    if(pos>=capacity)
        pos-=capacity;
    buffer[pos]=aChar;
    length++;
}

void Stream::append(const char* buf,int len)
{
    for(int i=0;i<len;i++)
        append(buf[i]);
}

char Stream::peek()
{
    if(length==0)
        return 0;
    return buffer[start];
}

int Stream::peek(char* buf,int len)
{
    if(len>length)
        len=length;
    for(int i=0;i<len;i++)
    {
        int pos=start+i;
        if(pos>=capacity)
            pos-=capacity;
        buf[i]=buffer[pos];
    }
    return len;
}

char Stream::take()
{
    if(length==0)
        return 0;
    char aChar=buffer[start];
    start++;
    length--;
    if(start>=capacity)
        start-=capacity;
    return aChar;
}

int Stream::take(char* buf,int len)
{
    if(len>length)
        len=length;
    for(int i=0;i<len;i++)
        buf[i]=take();
    return len;
}

void Stream::expand()
{
    int newCapacity=capacity*2;
    char* newBuf=new char[newCapacity];
    int newLength=length;
    take(newBuf,newLength);
    delete[] buffer;
    buffer=newBuf;
    capacity=newCapacity;
    start=0;
    length=newLength;
}


main.c
----------
#include "Serial.h"
#include <stdio.h>
#include <string.h>

int main()
{
    Serial s;
    //测试open()
    if(s.open("/dev/ttyUSB0",115200,8,Serial::PARITY_NONE,1)!=Serial::OK)
    {
        printf("Cannot open serial port!\n");
        return -1;
    }
    //测试write和int read(char* buf,int len,int timeout);
    const char* output="hello,paopao!";
    s.write(output,strlen(output));
    char buf[1024];
    int len=s.read(buf,sizeof(buf),100);
    buf[len]=0;
    printf("recv(len=%d): %s\n",len,buf);
    //测试write和int read(char* buf,int maxLen,const char* end,int timeout,int* recvLen);
    output="hello world\r\nMy name is paopao!\r\nI like programming~\r\n";
    s.write(output,strlen(output));
    while(s.read(buf,sizeof(buf),"\r\n",100,&len)==Serial::READ_END)
    {
        buf[len]=0;
        printf("recv line(len=%d): %s\n",len,buf);
    }
    return 0;
}



makefile
-------
main: Timestamp.cpp Stream.cpp Serial.cpp main.c
	g++ -c Timestamp.cpp -o Timestamp.o
	g++ -c Stream.cpp -o Stream.o
	g++ -c Serial.cpp -o Serial.o
	g++ -c main.c -o main.o
	g++ main.o Timestamp.o Stream.o Serial.o -lpthread -o main
clean:
	rm -rf Timestamp.o Stream.o Serial.o main.o main
