#include <stdio.h>      /*fprintf(),stderr,BUFSIZ*/
#include <stdlib.h>     /**/
#include <string.h>     /*stderror()*/
#include <fcntl.h>      /*open(),flag*/
#include <errno.h>      /*errno*/
#include <unistd.h>     /*ssize_t*/
#include <sys/types.h>
#include <sys/stat.h>   /*mode_t*/

#define BUFFER_SIZE 3

int main(int argc,char **argv)
{
   int from_fd,to_fd;
   int bytes_read,bytes_write;
   char buffer[BUFFER_SIZE];
   char *ptr;

   if(argc!=3)
   {
     fprintf(stderr,"Usage:%s fromfile tofile\n\a",argv[0]);
     exit(1);
   }

   /* 打开源文件 */
   if((from_fd=open(argv[1],O_RDONLY))==-1)   /*open file readonly,返回-1表示出错，否则返回文件描述符*/
   {
     fprintf(stderr,"Open %s Error:%s\n",argv[1],strerror(errno));
     exit(1);
   }

   /* 创建目的文件 */
   /* 使用了O_CREAT选项-创建文件,open()函数需要第3个参数,
      mode=S_IRUSR|S_IWUSR表示S_IRUSR 用户可以读 S_IWUSR 用户可以写*/
   if((to_fd=open(argv[2],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1) 
   {
     fprintf(stderr,"Open %s Error:%s\n",argv[2],strerror(errno));
     exit(1);
   }

   /* 以下代码是一个经典的拷贝文件的代码 */
   while(bytes_read=read(from_fd,buffer,BUFFER_SIZE))
   {
     /* 一个致命的错误发生了 */
     if((bytes_read==-1)&&(errno!=EINTR)) 
         break;
     else if(bytes_read>0)
     {
       ptr=buffer;
       while(bytes_write=write(to_fd,ptr,bytes_read))
       {
         /* 一个致命错误发生了 */
         if((bytes_write==-1)&&(errno!=EINTR))
             break;
         /* 写完了所有读的字节 */
         else if(bytes_write==bytes_read) 
             break;
         /* 只写了一部分,继续写 */
         else if(bytes_write>0)
         {
           ptr+=bytes_write;
           bytes_read-=bytes_write;
         }
       }

       /* 写的时候发生的致命错误 */
       if(bytes_write==-1)
           break;
     }
   }
   close(from_fd);
   close(to_fd);

   return 0;
} 
