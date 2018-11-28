本文参考：https://blog.csdn.net/heycwn/article/details/51202506(该代码参考了《算法导论》)
         https://www.cnblogs.com/liujin472/articles/5989303.html
该代码相对于fft_ref.cpp快了4倍左右，风格不再是每个点时复数，而是输入两路信号，但对其做复数FFT。

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
 
const int N = 32;
const float PI = atan(1)*4;  //3.1416;
 
void timeSubtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
}


inline void swap (float &a, float &b)
{
    float t;
    t = a;
    a = b;
    b = t;
}

// 位反转置换 Bit-reversal Permutation 
void bitrp (float xreal [], float ximag [], int n)
{
    
    int i, j, a, b, p;
 
    for (i = 1, p = 0; i < n; i *= 2)
        {
        p ++;
        }
    for (i = 0; i < n; i ++)
        {
        a = i;
        b = 0;
        for (j = 0; j < p; j ++)
            {
            b = (b << 1) + (a & 1);    // b = b * 2 + a % 2;
            a >>= 1;        // a = a / 2;
            }
        if ( b > i)
            {
            swap (xreal [i], xreal [b]);
            swap (ximag [i], ximag [b]);
            }
        }
}
 
void FFT(float xreal [], float ximag [], int n)
{
    // 快速傅立叶变换，将复数 x 变换后仍保存在 x 中，xreal, ximag 分别是 x 的实部和虚部
    float wreal [N / 2], wimag [N / 2], treal, timag, ureal, uimag, arg;
    int m, k, j, t, index1, index2;
    
    bitrp (xreal, ximag, n);
 
    // 计算 1 的前 n / 2 个 n 次方根的共轭复数 W'j = wreal [j] + i * wimag [j] , j = 0, 1, ... , n / 2 - 1
    arg = - 2 * PI / n;
    treal = cos (arg);
    timag = sin (arg);
    wreal [0] = 1.0;
    wimag [0] = 0.0;
    for (j = 1; j < n / 2; j ++)
        {
        wreal [j] = wreal [j - 1] * treal - wimag [j - 1] * timag;
        wimag [j] = wreal [j - 1] * timag + wimag [j - 1] * treal;
        }
 
    for (m = 2; m <= n; m *= 2)
        {
        for (k = 0; k < n; k += m)
            {
            for (j = 0; j < m / 2; j ++)
                {
                index1 = k + j;
                index2 = index1 + m / 2;
                t = n * j / m;    // 旋转因子 w 的实部在 wreal [] 中的下标为 t
                treal = wreal [t] * xreal [index2] - wimag [t] * ximag [index2];
                timag = wreal [t] * ximag [index2] + wimag [t] * xreal [index2];
                ureal = xreal [index1];
                uimag = ximag [index1];
                xreal [index1] = ureal + treal;
                ximag [index1] = uimag + timag;
                xreal [index2] = ureal - treal;
                ximag [index2] = uimag - timag;
                }
            }
        }
}


int main ()
{	 
	struct timeval tvBegin, tvEnd, tvDiff;
    float xreal [N] = {}, ximag [N] = {};
    
 	/* Naive DFT */
	gettimeofday(&tvBegin, NULL);
	for (int i=0; i < 100000; i++) {
		/* Init inputs */
		for (int i=0; i < N; i++) {
			xreal[i] = (float) i;
			ximag[i] = (float) i;
		}
		FFT (xreal, ximag,N);
	}
	gettimeofday(&tvEnd, NULL);

    timeSubtract(&tvDiff, &tvEnd, &tvBegin);
	 
    printf("100000 x Naive: \t %ld.%d\n", tvDiff.tv_sec, tvDiff.tv_usec);
	for (int i = 0; i < N; i ++)
	{
		printf ("%4d    %8.4f    %8.4f \n", i, xreal [i], ximag [i]);
	}
    printf ("================================= \n");

    return 0;
}


----
修改：尽量减少全局数组,同时将原数组拷贝至目的数组，然后以其为基础进行迭代，最终求出目标FFT数组。
修改W为动态开辟内存（主要是因为FFT点数未知）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
 
const int N = 32;
const float PI = atan(1)*4;  //3.1416;
 
void timeSubtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
}


inline void swap (float &a, float &b)
{
    float t;
    t = a;
    a = b;
    b = t;
}

// 位反转置换 Bit-reversal Permutation 
void bitrp (float xreal [], float ximag [], int n)
{
    
    int i, j, a, b, p;
 
	for (i = 1, p = 0; i < n; i *= 2)
	{
		p ++;
	}
	for (i = 0; i < n; i ++)
	{
		a = i;
		b = 0;
		for (j = 0; j < p; j ++)
		{
			b = (b << 1) + (a & 1);    // b = b * 2 + a % 2;
			a >>= 1;				   // a = a / 2;
		}
		if ( b > i)
		{
			swap (xreal [i], xreal [b]);
			swap (ximag [i], ximag [b]);
		}
	}
}
 
void FFT(float xreal_src [], float ximag_src [], int n,float xreal [], float ximag [])
{
	//将源数组内容拷贝至目标数组，然后以其为基础进行迭代，最后输出即为所求
	memcpy(xreal,xreal_src,sizeof(float)*n);
	memcpy(ximag,ximag_src,sizeof(float)*n);
    // 快速傅立叶变换，将复数 x 变换后仍保存在 x 中，xreal, ximag 分别是 x 的实部和虚部
	float* wreal=(float*)malloc(sizeof(float) * n/2);  //动态开辟变换核实部
	float* wimag=(float*)malloc(sizeof(float) * n/2);  //动态开辟变换核的虚部

    float treal, timag, ureal, uimag, arg;
    int m, k, j, t, index1, index2;
    
	bitrp (xreal, ximag, n);

	// 计算 1 的前 n / 2 个 n 次方根的共轭复数 W'j = wreal [j] + i * wimag [j] , j = 0, 1, ... , n / 2 - 1
	arg = - 2 * PI / n;
	treal = cos (arg);
	timag = sin (arg);
	//初始化w数组
	wreal [0] = 1.0;
	wimag [0] = 0.0;
	for (j = 1; j < n / 2; j ++)
	{
		wreal [j] = wreal [j - 1] * treal - wimag [j - 1] * timag;
		wimag [j] = wreal [j - 1] * timag + wimag [j - 1] * treal;
	}

	for (m = 2; m <= n; m *= 2)
	{
		for (k = 0; k < n; k += m)
		{
			for (j = 0; j < m / 2; j ++)
			{
				index1 = k + j;
				index2 = index1 + m / 2;
				t = n * j / m;    // 旋转因子 w 的实部在 wreal [] 中的下标为 t
				treal = wreal [t] * xreal [index2] - wimag [t] * ximag [index2];
				timag = wreal [t] * ximag [index2] + wimag [t] * xreal [index2];
				ureal = xreal [index1];
				uimag = ximag [index1];
				xreal [index1] = ureal + treal;
				ximag [index1] = uimag + timag;
				xreal [index2] = ureal - treal;
				ximag [index2] = uimag - timag;
			}
		}
	}
	free(wreal);
	free(wimag);
}


int main ()
{	 
	struct timeval tvBegin, tvEnd, tvDiff;
    float xreal [N] = {}, ximag [N] = {};
	float yreal [N] = {}, yimag [N] = {};

	/* Init inputs */
	for (int i=0; i < N; i++) {
		xreal[i] = (float) i;
		ximag[i] = (float) 0;
	}
 	/* Naive DFT */
	gettimeofday(&tvBegin, NULL);
	for (int i=0; i < 100000; i++) {
		FFT (xreal, ximag,N,yreal,yimag);
	}
	gettimeofday(&tvEnd, NULL);

    timeSubtract(&tvDiff, &tvEnd, &tvBegin);
	 
    printf("100000 x Naive: \t %ld.%d\n", tvDiff.tv_sec, tvDiff.tv_usec);
	for (int i = 0; i < N; i ++)
	{
		printf ("%4d    %8.4f    %8.4f \n", i, yreal [i], yimag [i]);
	}
    printf ("================================= \n");

    return 0;
}
