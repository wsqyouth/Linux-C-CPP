

#include <stdio.h> 

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define BLOCK_SIZE 256

u32 code0,code1,code2,code3;
u32 pps_cnt;
u32 abs,toa,s_short; //幅度、到达时间 
u32 num = 0; //?
u32 code_error=0;

void decoder_data(u32* ptr)
{
	volatile u32 *p;
	int i;
	u32 tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
	u32 s_short;
    p = (volatile u32 *)ptr;
    printf("enter coder!\r\n");
    //每循环一次P增加32个字节，共8次
	for(i = 0;i < BLOCK_SIZE;i = i + 32)
	{
		
		tmp0 = *(p++);
		tmp1 = *(p++);
        tmp2 = *(p++);
        tmp3 = *(p++);
        tmp4 = *(p++);
        tmp5 = *(p++);
        tmp6 = *(p++);
        tmp7 = *(p++);
        
        printf("----%p\n",p);
        if((tmp0 & 0xf0000000) == 0x10000000)
		{
			printf("---------------ac0 code--------------------\n");
			toa = tmp0 & 0xfffffff;
			printf("ac0_start is %d\n",toa);
			abs = (tmp1 & 0x3fffffff)>>14;
			printf("ac0_abs is %d\n",abs);
			code0 = tmp1 & 0x3fff;
			code1 = 0;
			code2 = 0;
			code3 = 0;
			num = tmp6&0xff;
			printf("find ac0 code is %d\n",code0);
			printf("time_counter is %d\n",pps_cnt);
			if((tmp6 & 0xffff0000) != 0x55aa0000)
			{
				printf("------------------head code is %4x\n",tmp6);
			}
			
			printf("time num is %d\n",num);
			printf("error count is %d\n",code_error);
			if((code0 != toa*2) || (abs != toa*4))
			{
				code_error++;
				printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxcode0 eeror xxxxxxxxxxxxxxxxxxxxxxxx\n");
			}
            // *(u16*)(&send_buf[32]) = 0x00A0; //IFF模式:AC
		}
		else if((tmp0 & 0xf0000000) == 0x20000000)
		{
			printf("---------------ac1 code--------------------\n");
			toa = tmp0 & 0xfffffff;
			printf("ac1_start is %d\n",toa);
			abs = (tmp1 & 0x3fffffff)>>14;
			printf("ac1_abs is %d\n",abs);
			code0 = tmp1 & 0x3fff;
			code1 = 0;
			code2 = 0;
			code3 = 0;
			num = tmp6&0xff;
			printf("find ac1 code is %d\n",code0);
			printf("time_counter is %d\n",pps_cnt);
            if((tmp6 & 0xffff0000) != 0x55aa0000)
			{
				printf("------------------head code is %4x\n",tmp6);
			}
        	printf("error count is %d\n",code_error);
			printf("time num is %d\n",num);
			if((code0 != toa*2) || (abs != toa*4))
			{
				code_error++;
				printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxcode0 eeror xxxxxxxxxxxxxxxxxxxxxxxx\n");
			}
    	}
    	else if((tmp0 & 0xf0000000) == 0x30000000)
		{
			printf("---------------s0 code--------------------\n");
			toa = tmp0 & 0xfffffff;
			printf("s0_start is %d\n",toa);
			abs = (tmp1 & 0x1ffff)>>1;
			printf("s0_abs is %d\n",abs);
			s_short = tmp1 & 0x1;
			printf("find s0_short is %d\n",s_short);
			if(s_short)
			{
				code0 = tmp3;
				code1 = tmp2 & 0xffffff;
				code2 = 0;
				code3 = 0;
			}
			else
			{
				code0 = tmp3;
				code1 = (tmp2 & 0xffffff) + (tmp5 & 0xff);
				code2 = (tmp5 >> 8) + (tmp4 & 0xff);
				code3 = (tmp4 & 0xffffff) >> 8;
			}
			num = tmp6&0xff;
			printf("time_counter is %d\n",pps_cnt);
			if((tmp6 & 0xffff0000) != 0x55aa0000)
			{
				printf("------------------head code is %4x\n",tmp6);
			}
			printf("time num is %d\n",num);
			printf("error count is %d\n",code_error);
			printf("long scode0 is 0x%4x--0x%4x--0x%4x--%d\n",code3,code2,code1,code0);
			if((code0 != toa*2) || ((abs != toa*4)))
			{
				code_error++;
				printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxcode0 eeror xxxxxxxxxxxxxxxxxxxxxxxx\n");
			}
		}
		else if((tmp0 & 0xf0000000) == 0x60000000)
		{
			printf("---------------s1 code--------------------\n");
			toa =tmp0 & 0xfffffff;
			printf("s1_start is %d\n",toa);
			abs = (tmp1 & 0x1ffff)>>1;
			printf("s1_abs is %d\n",abs);
			s_short = tmp1 & 0x1;
			printf("find s1_short is %d\n",s_short);
			if(s_short)
			{
				code0 = tmp3;
				code1 = tmp2 & 0xffffff;
				code2 = 0;
				code3 = 0;
			}
			else{
				code0 = tmp3;
				code1 = (tmp2 & 0xffffff) + (tmp5 & 0xff);
				code2 = (tmp5 >> 8) + (tmp4 & 0xff);
				code3 = (tmp4 & 0xffffff) >> 8;
			}
			num = tmp6&0xff;
			printf("time_counter is %d\n",pps_cnt);
			if((tmp6 & 0xffff0000) != 0x55aa0000)
			{
				printf("------------------head code is %4x\n",tmp6);
			}
			printf("time num is %d\n",num);
			printf("error count is %d\n",code_error);
			printf("long scode1 is 0x%4x--0x%4x--0x%4x--%d\n",code3,code2,code1,code0);
			if((code0 != toa*2) || ((abs != toa*4)))
			{
				code_error++;
				printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxcode0 eeror xxxxxxxxxxxxxxxxxxxxxxxx\n");
			}
		}
		else
		{
			//head_error++;
			printf("tmp0 is %4x \n",tmp0);
			printf("tmp1 is %4x \n",tmp1);
			printf("tmp2 is %4x \n",tmp2);
			printf("tmp3 is %4x \n",tmp3);
			printf("tmp4 is %4x \n",tmp4);
			printf("tmp5 is %4x \n",tmp5);
			printf("tmp6 is %4x \n",tmp6);
			printf("pps_cnt is %4x \n",pps_cnt);
		}
    }
}
int main()
{  
	u32 *map_base =(u32 *)0x0000000;
	printf("%p\n",map_base + 256/4);
    decoder_data(map_base + 256/4);

    return 0;
}
