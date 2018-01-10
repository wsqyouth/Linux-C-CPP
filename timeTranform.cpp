------
时间转换为时间戳

#include <stdio.h>  
#include <time.h>  
#include <stdlib.h>    
int main(int argc, const char * argv[])  
{  
    setenv("TZ", "GMT-8", 1); //默认 
    
    struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));  
    strptime("19700101080000","%Y%m%d%H%M%S",tmp_time); //时间24时制
    time_t t = mktime(tmp_time);  
    printf("%ld\n",t);  
    free(tmp_time);  
    return 0;  
}  


------
时间戳转换为格式化时间

#include <stdio.h>  
#include <time.h>  

int main(int argc, const char * argv[])  
{  
    time_t t;  
    struct tm *p;  
    t=0;  
    p=gmtime(&t);  
    char s[100];  
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", p);  
    printf("%d: %s\n", (int)t, s);  
    return 0;  
}  

------
时间戳转换为格式化时间2
#include <stdio.h>  
#include <time.h>  
#include <string.h>  
#include <stdlib.h>  
  
long GetTick()  
{  
    struct tm stm;  
    int iY, iM, iD, iH, iMin, iS;  
  
    memset(&stm,0,sizeof(stm));  
  
    iY = 1970;  
    iM = 1;  
    iD = 1;  
    iH = 8;  
    iMin = 0;  
    iS = 0;  
  
    stm.tm_year=iY-1900;  
    stm.tm_mon=iM-1;  
    stm.tm_mday=iD;  
    stm.tm_hour=iH;  
    stm.tm_min=iMin;  
    stm.tm_sec=iS;  
  
    /*printf("%d-%0d-%0d %0d:%0d:%0d\n", iY, iM, iD, iH, iMin, iS);*/  
  
    return mktime(&stm);  
}  
  
int main()
{
	//setenv("TZ", "GMT+0", 1);	//将当前时区设置成
	setenv("TZ", "GMT-8", 1); //将当前时区设置成东八区。需要注意的是，这里要写成GMT-8才是东八区，不是GMT+8	
	printf("%ld\n",GetTick());
    return 0;
}
