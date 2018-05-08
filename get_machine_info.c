参考一份工程代码，提取出获取Linux系统主机的信息：CPU频谱，CPU利用率，磁盘剩余空间
----
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>
 #include <unistd.h>
typedef char         		s8;
typedef unsigned char       u8;
typedef short       		s16;
typedef unsigned short      u16;
typedef int                 s32;
typedef unsigned    int     u32;
typedef float               f32;
typedef signed long long    s64;
typedef unsigned long long  u64;


/* 字符串解析*/
//首先打开file,读取内容到buf,然后根据string定位该字符串首地址
//移位到字符串尾地址，去除空格，以\n结尾得到数据
s8 *parse_string(char *file, char *string)
{
        char *pbuf;
        char *ptmp;
        char *ptmp1;
        int fd;
        int size;
        int nread;
        struct stat stat;

        if (!file || !string) return NULL;  
        /* 打开文件*/
        fd = open(file, O_RDONLY);
        if (fd < 0) {
                printf("open %s file error!\n", file);
                return NULL;
        }
       
        /*将文件读入缓冲区中*/
        fstat(fd, &stat);
        size = stat.st_size; 
        pbuf = malloc(size);
        if (pbuf < 0) {
                printf("malloc error!\n");
                return NULL;
        } 
		memset(pbuf, 0, size); 

        nread = read(fd, pbuf, size);
        if (nread < 0) {
                printf("read %s file error!", file);
                return NULL;
        }
		
        /* 解析字符串*/
        ptmp = strstr(pbuf, string);
		
        ptmp += strlen(string);
		
        while (1) {
                if ((*ptmp == ' ') || (*ptmp == ':') || (*ptmp == '\t')) { 
                        /* nothing */
                } else {
                        ptmp1 = strchr(ptmp, '\n');
                        *ptmp1 = '\0';
                        break;
                }
                ptmp++;
        }
        
        return ptmp;
}
/* 获取cpu频率*/
//root权限首先拷贝至根目录 cat /proc/cpuinfo 查找bogomips
void get_cpu_freq(f32 *cpufreq)
{
        s8 *file = "/cpuinfo";
        s8 *pstr  = "bogomips";
        s8 *ptmp;

        system("cp /proc/cpuinfo /");
        ptmp = parse_string(file, pstr);
	    
        *cpufreq = strtod(ptmp, NULL);
}



/* 获取当前剩余内存*/
//root权限首先拷贝至根目录 cat /proc/meminfo 查找MemFree
void get_freemem_space(u32 *freemem)
{
        char *file = "/meminfo";
        char *pstr = "MemFree:";
        char *ptmp;

        system("cp /proc/meminfo /");
        ptmp = parse_string(file, pstr);
        
        *freemem = strtol(ptmp, NULL, 10);;
}

/* 获取当前剩余磁盘空间*/
//df / -h 查找Avaiable
void get_freedisk_space(u32 *freedisk)
{
        unsigned long long free;
        unsigned long long blocks;
        struct statfs stat;
        char *dev = "/";

        statfs(dev, &stat);
        
        blocks    = stat.f_blocks;//f_blocks为block的数目          
        free      = stat.f_bfree; //每个free block里包含的字节数  f_bavail
        *freedisk  = (free * stat.f_bsize)>>20; //>>20是MB  >>30是GB
}
/* Main->MCM 反馈系统信息*/
struct stru_machine_info{
    f32             f32CpuFreq;
    u32             u32FreeMem;
    u32             u32FreeDisk;
};

/* 功能：获取系统运行状态 
 * 参数: MCM请求消息 
 * 返回值：无
 * */
void get_machine_info()
{
	struct stru_machine_info   rMachineInfo; 
	get_cpu_freq(&rMachineInfo.f32CpuFreq);
	get_freemem_space(&rMachineInfo.u32FreeMem);
	get_freedisk_space(&rMachineInfo.u32FreeDisk);
	
	printf("cpuinfo:%lf\n",rMachineInfo.f32CpuFreq);
	printf("freemem: %u\n",rMachineInfo.u32FreeMem);
	printf("freedisk:%u\n",rMachineInfo.u32FreeDisk);
}

int main()
{
    get_machine_info();
	
    
    return 0;
}
