map_write.c
-----
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

typedef struct{
	char name[4];
	int age;
}people;

int main(int argc,char **argv)
{
	int fd,i;
	people *p_map;
	char temp;

	fd = open(argv[1],O_CREAT | O_RDWR | O_TRUNC,00777);
	lseek(fd,sizeof(people)*5-1,SEEK_SET);
	write(fd,"",1);
	p_map = (people*)mmap(NULL,sizeof(people)*10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	close(fd);
	temp = 'a';
	for(i=0;i<10;i++)
	{
		temp+=1;
		memcpy((*(p_map+i)).name,&temp,2);
		(*(p_map+i)).age = 20+i;
	}
	printf("initialize voer\n");
	sleep(5);
	munmap(p_map,sizeof(people)*10);
	printf("umap ok\n");
	
	return 0;
}

map_read.c
-----
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

typedef struct{
	char name[4];
	int age;
}people;

int main(int argc,char **argv)
{
	int fd,i;
	people *p_map;
	
	fd = open(argv[1],O_CREAT|O_RDWR,00777);
	p_map = (people*)mmap(NULL,sizeof(people)*10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	for(i=0;i<10;i++)
	{
		printf("name:%s  age:%d;\n",(*(p_map+i)).name,(*(p_map+i)).age);
	}

	munmap(p_map,sizeof(people)*10);

	return 0;
}

-----
gcc map_write.c -o map_write
gcc map_read.c -o map_read
./map_write /tmp/test_shm
./map_read /tmp/test_shm

-----
从程序的运行结果中可以得出的结论 
    1、最终被映射文件的内容的长度不会超过文件本身的初始大小，即映射不能改变文件的大小； 
    2、可以用于进程通信的有效地址空间大小大体上受限于被映射文件的大小，但不完全受限于文件大小。
    3、文件一旦被映射后，调用mmap()的进程对返回地址的访问是对某一内存区域的访问，暂时脱离了磁盘上文件的影响。
       所有对mmap()返回地址空间的操作只在内存中有意义，只有在调用了munmap()后或者msync()时，才把内存中的相应内容写回磁盘文件，
       所写内容仍然不能超过文件的大小。
