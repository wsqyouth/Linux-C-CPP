/*
 * libiio - AD9361 IIO streaming example
 *
 * Copyright (C) 2014 IABG mbH
 * Author: Michael Feilen <feilen_at_iabg.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 **/

 /*
 *问题：
 * 1. 内部函数被修改(参数末尾添加参数phy_index)，但在组合字符串时为什么要加'-'?  "ad9361-phy-0" ?
 * 2. 结构体对象作为参数时，若需要通过函数内部从无到有生成，**双指针
 * 3. 修改了本振频率配置，通过参数配置
 * 4. 添加了gain_config_by_freq增益配置函数，可暂时不考虑
 * 5. 添加接受函数，考虑数据的接收通道
 **/
#include "ad9361.h"
/*****************************Private  Function************************************/

/* check return value of attr_write function */
static void errchk(int v, const char* what) {
    if (v < 0) { 
        fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); 
        //shutdown(); 
    }
}

/* write attribute: long long int */
static void wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
{
	errchk(iio_channel_attr_write_longlong(chn, what, val), what);
}

/* write attribute: string */
static void wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	errchk(iio_channel_attr_write(chn, what, str), what);
}

/* helper function generating channel names */
static char* get_ch_name(const char* type, int id)
{
    static char tmpstr[64];
	snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
	return tmpstr;
}
/* returns ad9361 phy device */
static struct iio_device* get_ad9361_phy(struct iio_context *ctx)
{
	struct iio_device *dev =  iio_context_find_device(ctx, "ad9361-phy");
	ASSERT(dev && "No ad9361-phy found");
	return dev;
}

/* finds AD9361 local oscillator IIO configuration channels */
static bool get_lo_chan(struct iio_context *ctx, enum iodev d, struct iio_channel **chn)
{
	switch (d) {
	 // LO chan is always output, i.e. true
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 0), true); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 1), true); return *chn != NULL;
	default: ASSERT(0); return false;
	}
}

/* finds AD9361 phy IIO configuration channel with id chid */
static bool get_phy_chan(struct iio_context *ctx, enum iodev d, int chid, struct iio_channel **chn)
{
	switch (d) {
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), false); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), true);  return *chn != NULL;
	default: ASSERT(0); return false;
	}
}

/* finds AD9361 streaming IIO devices */
static bool get_ad9361_stream_dev(struct iio_context *ctx, enum iodev d, struct iio_device **dev)
{
	switch (d) {
	case TX: *dev = iio_context_find_device(ctx, "cf-ad9361-dds-core-lpc"); return *dev != NULL;
	case RX: *dev = iio_context_find_device(ctx, "cf-ad9361-lpc");  return *dev != NULL;
	default: ASSERT(0); return false;
	}
}

/* finds AD9361 streaming IIO channels */
static bool get_ad9361_stream_ch(struct iio_context *ctx, enum iodev d, struct iio_device *dev, int chid, struct iio_channel **chn)
{
	*chn = iio_device_find_channel(dev, get_ch_name("voltage", chid), d == TX);
	if (!*chn)
		*chn = iio_device_find_channel(dev, get_ch_name("altvoltage", chid), d == TX);
	return *chn != NULL;
}

/* applies streaming configuration through IIO */
static bool cfg_ad9361_streaming_ch(struct iio_context *ctx, struct stream_cfg *cfg, enum iodev type, int chid)
{
	struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring AD9361 phy channel %d\n", chid);
	if (!get_phy_chan(ctx, type, chid, &chn)) {	return false; }
	wr_ch_str(chn, "rf_port_select",     cfg->rfport);
	wr_ch_lli(chn, "rf_bandwidth",       cfg->bw_hz);
	wr_ch_lli(chn, "sampling_frequency", cfg->fs_hz);

	// Configure LO channel
	printf("* Acquiring AD9361 %s lo channel\n", type == TX ? "TX" : "RX");
	if (!get_lo_chan(ctx, type, &chn)) { return false; }
	wr_ch_lli(chn, "frequency", cfg->lo_hz);
	return true;
}

////////////////////////////////////////////////////////////////////////////
//					AD9361   Funchtion（Public Interface)				  //
////////////////////////////////////////////////////////////////////////////

/* cleanup and exit 
* buffer: txbuf rxbuf
* channel:tx0_i  rx0_i
*/
void shut_down(struct iio_context *ctx,struct iio_buffer  *txbuf, struct iio_buffer  *rxbuf, struct iio_channel *tx0_i, struct iio_channel *tx0_q,
																							struct iio_channel *rx0_i, struct iio_channel *rx0_q)
{
	printf("* Destroying buffers\n");
	if (rxbuf) { iio_buffer_destroy(rxbuf); }
	if (txbuf) { iio_buffer_destroy(txbuf); }

	printf("* Disabling streaming channels\n");
	if (rx0_i) { iio_channel_disable(rx0_i); }
	if (rx0_q) { iio_channel_disable(rx0_q); }
	if (tx0_i) { iio_channel_disable(tx0_i); }
	if (tx0_q) { iio_channel_disable(tx0_q); }

	printf("* Destroying context\n");
	if (ctx) { iio_context_destroy(ctx); }
	exit(0);
}


int  init_ad9361_tx(struct iio_context *ctx ,struct iio_device **tx_dev,struct iio_buffer  **txbuf, 
        struct iio_channel **tx0_i, struct iio_channel **tx0_q,struct stream_cfg *txcfg)
{

	// Streaming devices
	struct iio_device *tx;

	printf("* Acquiring IIO context\n");
	ASSERT(iio_context_get_devices_count(ctx) > 0 && "No devices");
    
	printf("* Acquiring AD9361 streaming devices\n");
    ASSERT(get_ad9361_stream_dev(ctx, TX, &tx) && "No tx dev found");

	*tx_dev = tx;
    printf("device count :%d,ch size:%d\n",iio_context_get_devices_count(ctx),iio_device_get_channels_count(tx));
	printf("* Configuring AD9361 for TX streaming\n");
	assert(cfg_ad9361_streaming_ch(ctx, txcfg, TX, 0) && "TX port 0 not found");

    printf("* Initializing AD9361 IIO TX streaming channels\n");
	ASSERT(get_ad9361_stream_ch(ctx, TX, tx, 0, tx0_i) && "TX chan i not found");
    ASSERT(get_ad9361_stream_ch(ctx, TX, tx, 1, tx0_q) && "TX chan q not found");
    
    printf("* Enabling IIO streaming TX channels\n");
	iio_channel_enable(*tx0_i);
    iio_channel_enable(*tx0_q);
    
    printf("* Creating cyclic IIO TX buffers with 1024 Byte\n");
    *txbuf = iio_device_create_buffer(tx, 1024, true);//
	if (!*txbuf) {
		perror("Could not create TX buffer");
		//shutdown(ctx,txbuf,rxbuf,tx0_i,tx0_q,rx0_i,rx0_q);
		return -1;
	}

	return 0;
}

	
int  init_ad9361_rx(struct iio_context *ctx ,struct iio_device **rx_dev,struct iio_buffer  **rxbuf, 
        struct iio_channel **rx0_i, struct iio_channel **rx0_q,struct stream_cfg *rxcfg)
{
	// Streaming devices
	struct iio_device *rx;
	
	printf("* Acquiring IIO context\n");
	ASSERT(iio_context_get_devices_count(ctx) > 0 && "No devices");
    
	printf("* Acquiring AD9361 RX streaming devices\n");
    ASSERT(get_ad9361_stream_dev(ctx, RX, &rx) && "No rx dev found");

	*rx_dev = rx;
    printf("device count :%d,ch size:%d\n",iio_context_get_devices_count(ctx),iio_device_get_channels_count(rx));
    printf("* Configuring AD9361 for RX streaming\n");
    ASSERT(cfg_ad9361_streaming_ch(ctx, rxcfg, RX, 0) && "RX port 0 not found");
    
    printf("* Initializing AD9361 IIO RX streaming channels\n");
	ASSERT(get_ad9361_stream_ch(ctx, RX, rx, 0, rx0_i) && "RX chan i not found");
    ASSERT(get_ad9361_stream_ch(ctx, RX, rx, 1, rx0_q) && "RX chan q not found");
    
    printf("* Enabling IIO RX streaming channels\n");
	iio_channel_enable(*rx0_i);
	iio_channel_enable(*rx0_q);

    printf("* Creating non-cyclic IIO RX buffers with 1024 Bytes\n");
	*rxbuf = iio_device_create_buffer(rx, 1024, false);
	if (!*rxbuf) {
		perror("Could not create RX buffer");
		//shutdown(ctx,txbuf,rxbuf,tx0_i,tx0_q,rx0_i,rx0_q);
		return -1;
	}
  	return 0;
}


/* 本函数中仅仅考虑两路发：tx0_i  tx0_q,实际只用了一路发tx0_i
*  freq   载频
*  bufI    待发送数据I
*  bufQ    待发送数据Q
*  len    待发送数据的长度
*  1024/32=32
*  尝试最大次数为5次，成功返回0，失败为-1
*/
int  iio_send_data(struct iio_device *tx,struct iio_buffer *txbuf,struct iio_channel *tx0_i,struct iio_channel *tx0_q,int16_t *bufI,int16_t *bufQ,unsigned int len)
{
	ssize_t nbytes_rx, nbytes_tx;
	char *p_dat, *p_end;
	ptrdiff_t p_inc;

	int16_t i_val,q_val;
	unsigned int count = 0;
	uint32_t retry = 0;

	if(txbuf == NULL || tx0_i == NULL){
			printf("null ");
            return -1;
    }
	//while(1){
    {
			// WRITE: Get pointers to TX buf and write IQ to TX buf port 0
			p_inc = iio_buffer_step(txbuf);
			p_end = iio_buffer_end(txbuf);
			for (p_dat = (char *)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
					// Example: fill with zeros
					// 12-bit sample needs to be MSB alligned so shift by 4
					// https://wiki.analog.com/resources/eval/user-guides/ad-fmcomms2-ebz/software/basic_iq_datafiles#binary_format
					i_val = *(int16_t *)(bufI+count);
					q_val = *(int16_t *)(bufQ+count);
					//printf("%08x(%d)    %08x(%d) \n",i_val,i_val,q_val,q_val);

					((int16_t*)p_dat)[0] = i_val<<4; // Real (I)
					((int16_t*)p_dat)[1] = q_val<<4; // Imag (Q)

		
					count++;

					if(count >= len)
					count=0;    	
			}
	}
/*
	//尝试发送出去, 这里由于len定义为了填充数据的长度32，实际应该调整为buf的实际字节数4096，因此暂时没有做重发功能
	//nbytes_tx = 4096
	while(retry++ <5) {
			// Schedule TX buffer
			nbytes_tx = iio_buffer_push(txbuf);
			if (nbytes_tx==len ) { 
				//push succeed
				break;
			}
			usleep(1);
	}
*/
	
	// Schedule TX buffer
	nbytes_tx = iio_buffer_push(txbuf);
	printf("start push buffer %d\n",nbytes_tx);
	//sleep(1);
	if (nbytes_tx < 0 ) { 
			printf("Error pushing buf %d after %d times\n", (int) nbytes_tx,retry);
			return -1;
	}
	return 0;

}

////////////////////////////////////////////////////////////////////////////
//					attruibute   Funchtion								  //
////////////////////////////////////////////////////////////////////////////

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



#if 0
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
#endif
