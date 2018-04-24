#本函数主要是对libiio的数据分解有一个清晰的认识，获取32位整型数据，分别获得其高16位、低16位
最后对应得到相应的I、Q数据
疑惑：究竟高16位是I数据吗？
-------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
/*
static int write_dev_attr(uint8_t index, const char *attr, const char *src, size_t len)
{
    FILE *f;
    char buf[1024];
    ssize_t ret;

    sprintf(buf, "/sys/bus/iio/devices/iio:device%d/%s",index,attr);
    printf("%s\n",buf);
    f = fopen(buf, "we");
    if (!f)
        return -1;

    ret = fwrite(src, 1, len, f);
    fflush(f);
    if (ferror(f))
        ret = -1;
    fclose(f);
    return ret ;
}


int device_lo_power(uint8_t index,uint8_t down){
    char buffer[2];
    if(down >0){
        strcpy(buffer,"1");
    }else{
        strcpy(buffer,"0");
    }
     printf("%s\n",buffer);
    return write_dev_attr(index,"out_altvoltage1_TX_LO_powerdown",buffer,1);
} 
*/

void decompose(int32_t  buf[], int n)
{
    printf("32-bit Word: I : Q\n");
    for(int j=0;j<n;j++)
    {
         int32_t val = buf[j];
         char *p_dat=(char *)&val;
         const int16_t i = ((int16_t*)p_dat)[0]; // Real (I)
	     const int16_t q = ((int16_t*)p_dat)[1]; // Imag (Q)
	     printf("0x%.8X : %8d : %8d\n",val,i,q);
    }
}
int main (int argc , char* argv[])
{
    int32_t buf[] = {0x00007FFF,0x278D79BB,0x4B3B678D,0x678D4B3B,0x79BB278D,0x7FFF0000,0x79BBD873,
                     0x678DB4C5,0x4B3B9873,0x278D8645,0x00008001,0xD8738645,0xB4C59873,0x8645D873,
                     0x80010000,0x8645278D,0x98734B3B,0xB4C5678D,0xD87379BB};
    int n = 19;
    decompose(buf,n);
}

输出：
32-bit Word: I : Q
0x00007FFF :    32767 :        0
0x278D79BB :    31163 :    10125
0x4B3B678D :    26509 :    19259
0x678D4B3B :    19259 :    26509
0x79BB278D :    10125 :    31163
0x7FFF0000 :        0 :    32767
0x79BBD873 :   -10125 :    31163
0x678DB4C5 :   -19259 :    26509
0x4B3B9873 :   -26509 :    19259
0x278D8645 :   -31163 :    10125
0x00008001 :   -32767 :        0
0xD8738645 :   -31163 :   -10125
0xB4C59873 :   -26509 :   -19259
0x8645D873 :   -10125 :   -31163
0x80010000 :        0 :   -32767
0x8645278D :    10125 :   -31163
0x98734B3B :    19259 :   -26509
0xB4C5678D :    26509 :   -19259
0xD87379BB :    31163 :   -10125
