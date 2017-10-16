/*
*本程序主要测试根据不同输入参数，返回不同类型的功能 
* 1.学习关键字typedef,union,strlen的使用 
* 2.学习memcpy函数，用于字符串的赋值 
*/ 
#include<stdio.h>  
#include<string.h>   
typedef union{    
        char strIp[20];
        int port;    
}IPPORT;

//根据输入参数标志1,返回不同类型 
IPPORT GetValByKey(IPPORT & v ,int x)
{
	if(x == 1)
	{
	   strcpy(v.strIp, "192.168.123.115");  //字符数组赋值	
	}else
	{
		v.port = 6;    
	}
	
	return v;
}    
int main(){    
        IPPORT v;    
        //输入参数标志1,返回字符串 
        printf("v is %s\n",GetValByKey(v,1).strIp);
        
        //输入参数标志2，返回整型 
        printf("now v is %d\n",GetValByKey(v,2).port);  
        
		/*返回s的长度,不包括结束符NULL*/  
		//printf("%d\n",strlen(v.strIp));  //测试一个完整的IP实际长度 

        /*验证共用体成员使用相同地址*/
        //printf("now v.l is %d! the address is %p\n",v.strIp,&v.strIp);    
        //printf("now v.i is %d! the address is %p\n",v.port,&v.port); 
		
		return 0;   
}    
