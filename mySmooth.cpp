#include <iostream>
#include <cstring>
using namespace std;
//参考matlab smooth实现，滑动求平均，N一般为奇数

void printArr(double arr[],int n) {
    for(int i=0;i<n;++i)
        cout << arr[i] << " ";
    cout << endl;
}

void smooth(double *input_data, unsigned int len, double *output_data, unsigned int span){
    unsigned int i = 0,j = 0;
    unsigned int pn = 0,n = 0;
    double sum = 0.0;
    
    //保证span为奇数
    if (span % 2 == 1){
        n = (span - 1) / 2;
    } else{
        n = (span - 2) / 2;
    }
    
    for (i = 0; i < len; ++i){
        pn = n;
        
        //pn为左/右间距,(倒数)第一个元素时为0，(倒数)第二个元素时为1
        if (i < n){
            pn = i;
        } else if ((len - 1 - i) < n){
            pn = len - i - 1;
        }
        
        cout << "pn" << pn<<endl;
        sum = 0.0;
        for (j = i - pn; j <= i + pn; ++j){
            sum += input_data[j];
        }
        
        output_data[i] = sum / (pn * 2 + 1);
    }
}

int main()
{
    const int N = 20;
    double arr[N] = {12,13,25,14,15,18,20,22,2,25,28,35,36,38,40,41,42,43,44,50};
    double arrNew[N]={0};
    
    printArr(arr,N);
     
    smooth(arr,N,arrNew,5);
    cout<<"mysmooth:    ";
    printArr(arrNew,N);
     
   
    return 0;
}



附：
%% smooth函数、imfilter滤波、直接用conv2，最简单的低通比如1/9*ones(3)
%详细：
clc;clear;close all;
%--------------------------------------------------------------------------
%                      调用smooth函数进行加噪数据的平滑处理
%--------------------------------------------------------------------------

%*****************产生加噪正弦波信号，绘制加噪波形图*************************
t = linspace(0,2*pi,500)';    % 产生一个从0到2*pi的向量，长度为500
y = 100*sin(t);    % 产生正弦波信号
% 产生500行1列的服从N(0,152)分布的随机数，作为噪声信号
noise = normrnd(0,15,500,1);
y = y + noise;    % 将正弦波信号加入噪声信号
figure;    % 新建一个图形窗口
plot(t,y);    % 绘制加噪波形图
xlabel('t');    % 为X轴加标签
ylabel('y = sin(t) + 噪声');    % 为Y轴加标签


%*************利用移动平均法对加噪信号进行平滑处理，绘制平滑波形图*************
yy1 = smooth(y,30);    % 利用移动平均法对y进行平滑处理
figure;    % 新建一个图形窗口
plot(t,y,'k:');    % 绘制加噪波形图
hold on;
plot(t,yy1,'k','linewidth',3);    % 绘制平滑后波形图
xlabel('t');    % 为X轴加标签
ylabel('moving');    % 为Y轴加标签
legend('加噪波形','平滑后波形');
title('利用移动平均法对加噪信号进行平滑处理');


%*************利用lowess方法对加噪信号进行平滑处理，绘制平滑波形图*************
yy2 = smooth(y,30,'lowess');    % 利用lowess方法对y进行平滑处理
figure;    % 新建一个图形窗口
plot(t,y,'k:');    % 绘制加噪波形图
hold on;
plot(t,yy2,'k','linewidth',3);    % 绘制平滑后波形图
xlabel('t');    % 为X轴加标签
ylabel('lowess');    % 为Y轴加标签
legend('加噪波形','平滑后波形');
title('利用lowess方法对加噪信号进行平滑处理');

%*************利用rlowess方法对加噪信号进行平滑处理，绘制平滑波形图************
yy3 = smooth(y,30,'rlowess');    % 利用rlowess方法对y进行平滑处理
figure;    % 新建一个图形窗口
plot(t,y,'k:');    % 绘制加噪波形图
hold on;
plot(t,yy3,'k','linewidth',3);    % 绘制平滑后波形图
xlabel('t');    % 为X轴加标签
ylabel('rlowess');    % 为Y轴加标签
legend('加噪波形','平滑后波形');
title('利用rlowess方法对加噪信号进行平滑处理');

%*************利用loess方法对加噪信号进行平滑处理，绘制平滑波形图*************
yy4 = smooth(y,30,'loess');    % 利用loess方法对y进行平滑处理
figure;    % 新建一个图形窗口
plot(t,y,'k:');    % 绘制加噪波形图
hold on;
plot(t,yy4,'k','linewidth',3);    % 绘制平滑后波形图
xlabel('t');    % 为X轴加标签
ylabel('loess');    % 为Y轴加标签
legend('加噪波形','平滑后波形');
title('利用loess方法对加噪信号进行平滑处理');

%*************利用sgolay方法对加噪信号进行平滑处理，绘制平滑波形图*************
yy5 = smooth(y,30,'sgolay',3);    % 利用sgolay方法对y进行平滑处理
figure;    % 新建一个图形窗口
plot(t,y,'k:');    % 绘制加噪波形图
hold on;
plot(t,yy5,'k','linewidth',3);    % 绘制平滑后波形图
xlabel('t');    % 为X轴加标签
ylabel('sgolay');    % 为Y轴加标签
legend('加噪波形','平滑后波形');
title('利用sgolay方法对加噪信号进行平滑处理');
