参考：https://www.cnblogs.com/mildsim/p/4067374.html
抽选中三个比较重要的窗函数，然后对其测试。后面如果有需要可以继续添加。
缺点：函数内部通过malloc申请内存，记得在函数调用出free释放内存。

WindowFun.h
---
#ifndef WINDOW_FUN_H
#define WINDOW_FUN_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.1415926

int    gaussianWin(unsigned int N, double alpha, double **w);
int    hammingWin(unsigned int N, double **w);
int    hannWin(unsigned int N, double **w);

#endif


WindowFun.c
---
#include "WindowFun.h"

/*
 *函数名：gaussianWin
 *说明：计算gaussianWin窗函数
 *输入：
 *输出：
 *返回：
 *调用：
 *其它：用过以后，需要手动释放掉*w的内存空间
 *      调用示例：ret = gaussianWin(99,2.5, &w); 
 */
int    gaussianWin(unsigned int N, double alpha, double **w)
{
    unsigned int n;
    double k, beta, theta;
    double *ret;

    ret = (double *)malloc(N * sizeof(double));
    if(ret == NULL)
        return -1;
    
    for(n =0; n < N; n++)
    {
        if((N % 2) == 1)
        {
            k = n - (N - 1)/2;
            beta = 2 * alpha * (double)k / (N - 1);  
        }
        else
        {
            k = n - (N)/2;
            beta = 2 * alpha * (double)k / (N - 1);  
        }
        
        theta = pow(beta, 2);
        *(ret + n) = exp((-1) * (double)theta / 2);
    }

    *w = ret;

    return 0;
}

/*
 *函数名：hammingWin
 *说明：计算hammingWin窗函数
 *输入：
 *输出：
 *返回：
 *调用：
 *其它：用过以后，需要手动释放掉*w的内存空间
 *        调用示例：ret = hammingWin(99, &w); 
 */
int    hammingWin(unsigned int N, double **w)
{
    unsigned int n;
    double *ret;
    ret = (double *)malloc(N * sizeof(double));
    if(ret == NULL)
        return -1;

    for(n = 0; n < N; n++)
    {
        *(ret + n) = 0.54 - 0.46 * cos (2 * PI *  ( double )n / ( N - 1 ) );
    }

    *w = ret;

    return 0;
}

/*
 *函数名：hannWin
 *说明：计算hannWin窗函数
 *输入：
 *输出：
 *返回：
 *调用：
 *其它：用过以后，需要手动释放掉*w的内存空间
 *        调用示例：ret = hannWin(99, &w); 
 */
int    hannWin(unsigned int N, double **w)
{
    unsigned int n;
    double *ret;
    ret = (double *)malloc(N * sizeof(double));
    if(ret == NULL)
        return -1;

    for(n = 0; n < N; n++)
    {
        *(ret + n) = 0.5 * ( 1 - cos( 2 * PI * (double)n / (N - 1)));
    }

    *w = ret;

    return 0;
}


test.c
//gcc test.c WindowFun.c -o test -lm
#include "WindowFun.h"
#include <stdio.h>

void printArr(double arr[],int n)
{
    for(int i = 0; i<n; ++i) {
        printf("%lf\n",arr[i]);
    }
}
int main() 
{
    double *w;
    unsigned int N = 100;

    //int ret = hannWin(100, &w); 
    int ret = hammingWin(100, &w); 
    printArr(w,100);
    free(w);

    printf("hello wrold\n");
    return 0;
}
