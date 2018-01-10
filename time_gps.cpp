#include <stdio.h>  
#include <time.h>  
#include <string.h>  
#include <stdlib.h>  
 
typedef unsigned long u32;
 
long GetTick(char *str_time)  
{  
    struct tm stm;  
    int iY, iM, iD, iH, iMin, iS;  
  
    memset(&stm,0,sizeof(stm));  
  
    iY = (str_time[10]-'0')*10 + str_time[11]-'0' +2000;  //2018(由于GPS串中没有明确年是2000年之后，要小心)
    iM = (str_time[12]-'0')*10 + str_time[13]-'0' ;  //01
    iD = (str_time[14]-'0')*10 + str_time[15]-'0' ;  //01
    
    iH = (str_time[17]-'0')*10 + str_time[18]-'0' ; 
    iMin =(str_time[19]-'0')*10 + str_time[20]-'0' ; 
    iS = (str_time[21]-'0')*10 + str_time[22]-'0' ; 
  
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
	//setenv("TZ", "GMT-8", 1); //将当前时区设置成东八区。需要注意的是，这里要写成GMT-8才是东八区，不是GMT+8	
    //char str_time[] = {"$CMTOD,04,180101,080000,37,0,A,3044.057250,N,10358.091980,E,529.7,M,2,1,K,1.796,0532281,O,14,09,83.0976*62"};
	//printf("%ld\n", GetTick(str_time));
    //printf("%d\n",sizeof(long));
	//printf("%d\n",sizeof(unsigned long));

    u32 val = 4294967295+1; //4294967294+2
    //printf("%d\n",sizeof(u32));
    printf("%ld\n",val);
    printf("%lX\n",val);
	printf("low:%ld\n",val&0XFFFFFFFF);
    printf("high:%ld\n",(val>>32)&0XFFFFFFFF);
    return 0;
}
