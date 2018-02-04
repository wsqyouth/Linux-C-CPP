
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;



//获取某个值的位x的值
u8  getBit(u32 value, int bit)
{
	return (value&(1<<bit))>>bit;
}

//设置某值的特定位为0或1
/************************************************************* 
* 函数名称： Bit_Set 
* 功能描述： 对1字节的数据进行任意位操作
* 参数列表： p_data     是指定的源数据；   
           position   是指定位（取值范围为1~8)；
           flag       表示置0还是置1操作。
* 返回结果： 无
**************************************************************/
void Bit_Set(unsigned char *p_data, unsigned char position, int flag)
{
    int a = 1 << (position - 1);
    if (flag)
    {
        *p_data |= a;
    }
    else
    {
        *p_data &= ~a;
    }
}
