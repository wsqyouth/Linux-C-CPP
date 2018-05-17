
//对sys路径下设备文件读写，当然也可以使用system系统调用的方法
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>
 #include <unistd.h>

 

//写入设备文件特定属性
static int write_dev_attr(uint8_t index, const char *attr, const char *src, size_t len)
{
    FILE *f;
    char buf[1024];
    ssize_t ret;

    sprintf(buf, "/sys/bus/iio/devices/iio:device%d/%s",index,attr);

    printf("%s\n",buf);
    //写入
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
//读取测试
 int read_dev_attr(uint8_t index, const char *attr)
{
    FILE *f;
    char filebuf[1024];
	char contentBuf[1024];
	char *cp;
    ssize_t ret;

    sprintf(filebuf, "/sys/bus/iio/devices/iio:device%d/%s",index,attr);

    //printf("%s\n",filebuf);
//读取
	f = fopen(filebuf, "r");
    if (!f)
        return -1;
    // this is a string
    cp = fgets(contentBuf,sizeof(contentBuf),f);
    if (cp == NULL)
		return -1;

    fflush(f);
    if (ferror(f))
        ret = -1;
    fclose(f);
	
	printf("%s\n",contentBuf);
   return ret ;
}

/*
*  设置特定接收通道号的增益
*  index:       通道号0~3
*  extern_gain: 增益范围-3-100dB
*/
int rxGainValueConfig_byIndex(uint8_t index,int8_t extern_gain){
    char buffer[16];
	float gain_val=(float)extern_gain;
	sprintf(buffer,"%.2f",gain_val);

    return write_dev_attr(index,"in_voltage0_hardwaregain",buffer,strlen(buffer));
}
/*
*  设置特定接收通道号的增益模式
*  index:       通道号0~3
*  flag:        0(manual模式)  1(slow_attack模式) 2(fast_attack模式)
*/
int rxGainModeConfig_byIndex(uint8_t index,int8_t flag){
	//flag非零值1：slow模式
	if(1==flag){
		char buffer[ ]={"slow_attack"};
        return write_dev_attr(index,"in_voltage0_gain_control_mode",buffer,strlen(buffer));

	}else if(2==flag){   //flag非零值2：fast模式
		char buffer[ ]={"fast_attack"};
        return write_dev_attr(index,"in_voltage0_gain_control_mode",buffer,strlen(buffer));
	}else{   //flag为零值：manual模式
		char buffer[ ]={"manual"};
        return write_dev_attr(index,"in_voltage0_gain_control_mode",buffer,strlen(buffer));
	}
    
}
/*
*  设置特定接收通道号的射频带宽 
*  index:       通道号0~3
*  bandwidth:   rf bandwidth(无符号类型)
*/
int rxFreqBandwidth_byIndex(uint8_t index,uint32_t bandwidth){
	char buffer[16];
	//float rxfreq_bandwidth=(float)bandwidth; 
	sprintf(buffer,"%Ld",bandwidth);
    return write_dev_attr(index,"in_voltage_rf_bandwidth",buffer,strlen(buffer));
}

/*
*  设置特定接收通道号的采样频率 
*  index:             通道号0~3
*  samplefrequency:   采样频率(无符号类型)
*/
int rxFreqSample_byIndex(uint8_t index,uint32_t samplefrequency){
	char buffer[16];
	sprintf(buffer,"%Ld",samplefrequency);
    return write_dev_attr(index,"in_voltage_sampling_frequency",buffer,strlen(buffer));
}
/*
*  设置特定接收通道号的本振频率 
*  index:             通道号0~3
*  lofrequency:       本振频率(无符号类型)
*/
int rxLoFrequency_byIndex(uint8_t index,uint32_t lofrequency){
	char buffer[16];
	sprintf(buffer,"%Ld",lofrequency);//无符号
    return write_dev_attr(index,"out_altvoltage0_RX_LO_frequency",buffer,strlen(buffer));
}

int main()
{
	
    char readBuf[1024];
	
    //0通道为0(manual模式),1(slow模式),2(fast模式)
	rxGainModeConfig_byIndex(0,0);
    read_dev_attr(0,"in_voltage0_gain_control_mode");
	//manual模式时，设置增益值
	rxGainValueConfig_byIndex(0,50);
	read_dev_attr(0,"in_voltage0_hardwaregain");

	//rx 中心频率带宽
	rxFreqBandwidth_byIndex(0,5000000);
 	read_dev_attr(0,"in_voltage_rf_bandwidth");
    
	//rx Lo频率
	rxLoFrequency_byIndex(0,2450000000);
	read_dev_attr(0,"out_altvoltage0_RX_LO_frequency");
	//rx 采样频率
	rxFreqSample_byIndex(0,10000000);
 	read_dev_attr(0,"in_voltage_sampling_frequency");
    return 0;
}
