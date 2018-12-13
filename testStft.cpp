
参考matlab代码改写：
%短时傅里叶变换STFT
%依据FFT手动实现STFT
clear
clc
close all
Fs = 1000;            % Sampling frequency
T = 1/Fs;             % Sampling period
L = 1000;             % Length of signal
t = (0:L-1)*T;        % Time vector
S = 20*sin(150*2*pi*t)+40*cos(250*2*pi*t)*j;%0.2-0.7*cos(2*pi*50*t+20/180*pi) + 0.2*cos(2*pi*100*t+70/180*pi) ;

%% 所需参数
%主要包含:信号分割长度(默认分割8个窗口)，海明窗口，重叠率，N点采样
%默认设置：
% nsc=floor(L/4.5);%海明窗的长度
% nov=floor(nsc/2);%重叠率
% nff=max(256,2^nextpow2(nsc));%N点采样长度
%也可手动设置
nsc=100;%海明窗的长度,即每个窗口的长度
nov=0;%重叠率
nff=128;%N点采样长度 max(256,2^nextpow2(nsc))
%% 手动实现STFT
h=hamming(nsc, 'periodic');%计算海明窗的数值，给窗口内的信号加权重
coln = 1+fix((L-nsc)/(nsc-nov));%信号被分成了多少个片段
%如果nfft为偶数，则S的行数为(nfft/2+1)，如果nfft为奇数，则行数为(nfft+1)/2
%因为matlab的FFT结果是对称的，只需要一半
rown=nff/2+1;
STFT_X=zeros(rown,coln);%初始化最终结果
%对每个片段进行fft变换
index=1;%当前片段第一个信号位置在原始信号中的索引
for i=1:coln
    %提取当前片段信号值,并用海明窗进行加权
    temp_S=S(index:index+nsc-1).*h';
    %进行N点FFT变换
    temp_X=fft(temp_S,nff);
    %取一半
    STFT_X(:,i)=temp_X(1:rown)';
    %将索引后移
    index=index+(nsc-nov);
end

%% matlab自带函数
spectrogram(S,hamming(nsc, 'periodic'),nov,nff,Fs);
title('spectrogram函数画图')
[spec_X,spec_f,spec_t]=spectrogram(S,hamming(nsc, 'periodic'),nov,nff,Fs);
%减法，看看差距
% plot(abs(spec_X)-abs(STFT_X))

%% 画频谱图
% 海明窗口的均值
K = sum(hamming(nsc, 'periodic'))/nsc;
%获取幅值(除以采样长度即可，后面再决定哪几个乘以2)，并依据窗口的海明系数缩放
STFT_X = abs(STFT_X)/nsc/K;
% correction of the DC & Nyquist component
if rem(nff, 2)                     % 如果是奇数，说明第一个是奈奎斯特点，只需对2:end的数据乘以2
    STFT_X(2:end, :) = STFT_X(2:end, :).*2;
else                                % 如果是偶数，说明首尾是奈奎斯特点，只需对2:end-1的数据乘以2
    STFT_X(2:end-1, :) = STFT_X(2:end-1, :).*2;
end

% convert amplitude spectrum to dB (min = -120 dB)
%将幅值转换成分贝有函数是ydb=mag2db(y)，这里直接算
STFT_X = 20*log10(STFT_X + 1e-6);

%时间轴
STFT_t=(nsc/2:(nsc-nov):nsc/2+(coln-1)*(nsc-nov))/Fs;
%频率轴
STFT_f=(0:rown-1)*Fs./nff;
% plot the spectrogram
figure
surf(STFT_f, STFT_t,  STFT_X')
shading interp
axis tight
box on
view(0, 90)
set(gca, 'FontName', 'Times New Roman', 'FontSize', 14)
xlabel('Frequency, Hz')
ylabel('Time, s')
title('Amplitude spectrogram of the signal')

handl = colorbar;
set(handl, 'FontName', 'Times New Roman', 'FontSize', 14)
ylabel(handl, 'Magnitude, dB')

----
//由于gcc编译器不支持数组直接初始化，因此这里使用g++编译器。
//g++ test.c WindowFun.c  fftComplex.c -o test -lm
---
#include "fftComplex.h"
#include "WindowFun.h"
#include <stdio.h>
#include <sys/time.h>

void printArr(double arr[],int n)
{
    for(int i = 0; i<n; ++i) {
        printf("%d %lf\n",i,arr[i]);
    }
}


void timeSubtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
}

/*函数名：dotMultiply
 *说明：对输入数组input区间为[start,start+step]的元素进行权重计算
 *输入：    input  
		   weight  
		   start
		   step
 *输出：输出数组output
 *其它：数据以复数为单位进行计算
 */
void dotMultiply(const complexType* input,const  double* weight,const int start,
											  const  int step, complexType *output)
{
    for(int i = 0; i < step; ++i){
        output[i].Real = input[start+i].Real * weight[i];
		output[i].Imag = input[start+i].Imag * weight[i];
    }
}

/*函数名：fftStft
 *说明：短时傅里叶变换
 *输入：数组inputSignal  Signal vector
		   Len         Signal  length
 *         nsc         Number SeCtion 海明窗的长度,即每个窗口的长度
 *         nov         Number OverLap 重叠率
 *         nff          Number SeCtion FFT N点采样长度
 *         fs          Frequency of Sample 采样率
 *输出：输出数组outputSignal(后续处理输出的数组) ,complexType **outputSignal
 *其它：数据以复数为单位进行计算
 */
void fftStft(const complexType* inputSignal,const int Len,const int nsc,const int nov,const int nff,const int Fs)
{
	double *hammingWeight;
    int ret = hammingWin(nsc, &hammingWeight); //计算海明窗的数值，给窗口内的信号加权重
	
	int coln =1+ floor((Len-nsc)/(nsc-nov)) ;//信号被分成了多少个片段 
	int rown = nff/2+1;					   //存储FFT前一半计算结果
	
	complexType STFT_X[rown][coln]={0};
	complexType temp_S[nff]={0};		  // 
	complexType temp_X[nff]={0};		  // 
	

#if 1	
	int index = 0;						  //信号数据的步进索引
	for(int i=0;i<coln;++i)
	{
		memset(temp_S, 0, sizeof(complexType)*nff);  //temp_S nsc !=nff
		dotMultiply(inputSignal,hammingWeight,index,nsc,temp_S);
		//output(temp_S,nff);
		fft(temp_S,nff,temp_X);                   //调用快速傅里叶变换  
		//printf("temp_X\n");
		//output(temp_S,nff);
		//output(temp_X,nff);
		//存储FFT前一半计算结果
		for(int j=0;j<rown;++j)
		{
			//将temp_X中的1-rown个元素放到中第i列（也是rown个元素）
			STFT_X[j][i].Real = temp_X[j].Real;
			STFT_X[j][i].Imag = temp_X[j].Imag;
		}
		/*		
		printf("STFT_X[j][i]----%d\n",i);
		for(int j=0;j<rown;++j)
		{
			printf("%d %lf %lf\n",j,STFT_X[j][i].Real,STFT_X[j][i].Imag);
		}
		*/
		index = index +(nsc-nov);
	}
    printf("rown=%d coln=%d index=%d\n",rown,coln,index);
	
	//计算hamming窗权重因子的均值
	double hammingWeightSum = 0;
	for(int i=0;i<nsc;++i)
	{
		hammingWeightSum += hammingWeight[i];
	}
	double hammingWeightMean = hammingWeightSum/nsc;
	printf("k=%lf\n",hammingWeightMean);

	//对每一列求abs
	for(int i=0;i<coln;++i)
	{
		for(int j=0;j<rown;++j)
		{
			//对该列中每个复数STFT_X[j][i] 进行模值运算，虚部为0（也是rown个元素）
			STFT_X[j][i].Real = sqrt(STFT_X[j][i].Real*STFT_X[j][i].Real + 
									 STFT_X[j][i].Imag*STFT_X[j][i].Imag)/nsc/hammingWeightMean;
			STFT_X[j][i].Imag = 0;
		}
		//printf("abs STFT_X[j][i]----%d\n",i);
		//for(int j=0;j<rown;++j)
		//{
		//	printf("%lf\n",STFT_X[j][i].Real);
		//}
	}
	//
	if( (nff&1) )  //nff%2
	{
		// 如果是奇数，说明第一个是奈奎斯特点，只需对2:end的数据乘以2
		//对每一列[1,rown-1]*2
		for(int i=0;i<coln;++i)
		{
			for(int j=1;j<rown;++j)
			{
				STFT_X[j][i].Real = STFT_X[j][i].Real *2;
			}
		}
	} else{
		//如果是偶数，说明首尾是奈奎斯特点，只需对2:end-1的数据乘以2
		//对每一列[1,rown-2]*2
		for(int i=0;i<coln;++i)
		{
			for(int j=1;j<rown-1;++j)
			{
				STFT_X[j][i].Real = STFT_X[j][i].Real *2;
			}
		}
	}
	
	//将幅值转换成分贝有函数是ydb=mag2db(y)，这里直接算
	for(int i=0;i<coln;++i)
	{
		for(int j=0;j<rown;++j)
		{
			STFT_X[j][i].Real = 20*log10(STFT_X[j][i].Real + 1e-6);
		}
		printf("abs log STFT_X[j][i]----%d\n",i);
		for(int j=0;j<rown;++j)
		{
			printf("%lf\n",STFT_X[j][i].Real);
		}
	}
	//output(sinTable,N);
#endif

	free(hammingWeight);	
}
int main() 
{
  
	int Fs = 1000; 		//Sampling frequency
	double T = 1.0/Fs;  //Sampling period
	int L = 1000;       // Length of signal
	complexType sinTable[L]={0};
	for(int n=0;n<L;++n){
		sinTable[n].Real = 20*sin(150*2*PI*n/Fs);//输入序列的实部和虚部 (double)n
		sinTable[n].Imag = 40*cos(250*2*PI*n/Fs);
	}
	//printf("sinTable\n");
	//output(sinTable,N);

	int nsc = 100; //海明窗的长度,即每个窗口的长度
	int nov = 0; //重叠率
	int nff = 128; //N点采样长度
	
	fftStft(sinTable,L,nsc,nov,nff,Fs);
	
	
 	printf("hello wrold");
    return 0;
}
