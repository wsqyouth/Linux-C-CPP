%跳频信号产生图示
%作为paper信号采用。主要采用该模块产生的信号图示
clear all;
clc;
 
fs=1e7;
ts=1/fs;
deltaf=4e4;
f=[0.5e6 1.7e6 3.2e6 4.4e6 2.1e6 3.8e6 2.6e6 1.3e6];
%  f=[3.04 3.12 3.20 3.08 3.24 3.28 3.16]*1e6;
% f=1e6*[45.0515 45.224 45.136 45.2548];
% fc=20e3;
fc=1e6;
fc1=1e6;
a=3e4;
Nh=8;        %跳数
th=50e-6;    %跳频周期
nsamp=th/ts; %跳频周期/采样周期=500
t=[ts:ts:Nh*nsamp*ts];
te=5e-6;
ns=te/ts; %50
NP=1;
p=0.6;
snr=10;
times=1;
sig=[];
%j=1  信号a持续ts*ns  j=2~10  信号c持续ts*ns,共9个   f(1)频率持续时间10*ts*ns=10*te = 0.5
%j=11 信号b持续ts*ns  j=12~20 信号c持续ts*ns,共9个   f(2)频率持续时间10*ts*ns=10*te = 0.5
%...
%j=71 信号b持续ts*ns  j=72~80 信号c持续ts*ns,共9个   f(8)频率持续时间10*ts*ns=10*te = 0.5
% 长度Nh*nsamp=8*500=80*ns=4000
for  j=1:80
    i=ceil(j/10);
    if j==1
       t=ts:ts:j*ns*ts;
       sig1=cos(2*pi*(f(i)+a*exp(-fc*t).*sin(2*pi*fc1*t)).*t);   %信号a
    elseif j==10*(i-1)+1
        t=(j*ns+1)*ts:ts:(j+1)*ns*ts;
        sig1=cos(2*pi*(f(i)+a*exp(-fc*(t-j*ns*ts)).*sin(2*pi*fc1*(t-j*ns*ts))).*t);%信号b
    else
        t=(j*ns+1)*ts:ts:(j+1)*ns*ts;
        sig1=cos(2*pi*f(i)*t);%信号c
    end
    sig=[sig sig1];
end
aaa = sig;
save cc.txt
%取出部分数据,并加10db噪声
sig=sig(251:3750);
sig=awgn(sig,snr,'measured');
figure(1);
subplot(211);
t=251*ts:ts:(Nh*nsamp-250)*ts;   %251-3500
plot(t,sig);
axis([251*ts,(Nh*nsamp-250)*ts -1.5,1.5]);  
 xlabel('时间/s');
 ylabel('幅值');
 title('跳频信号时域图');
 subplot(212);
 len=length(sig);
 F=[-fs/2:fs/len:fs/2-fs/len]/1e6;  %-fs/2:fs/2
 plot(F,abs(fftshift(fft(sig))));
 title('跳频信号频谱图');
 xlabel('频率/MHz');
 ylabel('频谱幅度');
 
 
 
 
 % % %%%%%%%%%%%短时傅里叶变化%%%%%%%%%%%%
sig=sig';
t=1:length(sig);
N=512;
%设置窗函数1
h1=window('hamming',255);
% h1=ones(501,1);
% h1=gausswin(511);
% 计算短时傅立叶变换
% sig=hilbert(sig);
[S,T,F]=tfrsp(sig,t,N,h1);
figure(2);
contour(T,abs(F(1:N/2)),abs(S(1:N/2,:)));
% title('谱图法');
axis([1,3500 0,0.5]);
xlabel('时域采样点');
ylabel('归一化频率 F');


figure(3);
mesh(T,abs(F),2*abs(S));
axis([1,3500 0,0.5 0,120]);
title('谱图法');
xlabel('时域采样点');
ylabel('归一化频率 F');
zlabel('幅值');

[S(T),F(T)]=max(S);
S(T)=abs(S(T));
F(T)=abs(F(T));
figure(4);
plot(T*ts*1e6,S(T));
title('跳频信号峰值包络y(n)');
xlabel('时间/us');
ylabel('峰值幅值');
 
figure(5);
F1=T*fs/length(T);
S(T)=mean(S(T))-S(T);
plot(F1,abs(fft(S(T))));
axis([0,2e5,0,2.5e4]);
title('y(n)的FFT谱');
xlabel('频率/Hz');
ylabel('FFT幅度');
F=F(T)/N*fs/1e6-fs/N*1e-6;
figure(6);
plot(T*ts*1e6,abs(F));
title('时频脊线');
xlabel('时间/us');
ylabel('频率/MHz');
T_hop=diff(F(T));
 
figure(7);
T1=1:length(T_hop);
plot(T1*ts*1e6,T_hop);
title('跳频时刻');
xlabel('时间/us');
ylabel('幅值');

--------------------------------------
2018-12-23修改：
%跳频信号产生图示
%目标：看懂该代码，采用论文上某种跳频信号，进行自己的参数估计，目前对包络fft计算后的震荡频率推测出的跳频频率有点问题，继续改正
%改进：使用zoom-stft减少计算量 或者 线性回归进行拟合求出跳频周期（该参数是推导其他参数的基础，多研究下）
%作为paper信号采用。主要采用该模块产生的信号图示
clear all;
clc;
close all;
fs=1e7;
ts=1/fs;
deltaf=4e4;
f=[0.5e6 1.7e6 3.2e6 4.4e6 2.1e6 3.8e6 2.6e6 1.3e6];
%  f=[3.04 3.12 3.20 3.08 3.24 3.28 3.16]*1e6;
% f=1e6*[45.0515 45.224 45.136 45.2548];
% fc=20e3;
fc=1e6;
fc1=1e6;
a=3e4;
Nh=8;        %跳数
th=50e-6;    %跳频周期
nsamp=th/ts; %跳频周期/采样周期=500
t=[ts:ts:Nh*nsamp*ts];
te=5e-6;
ns=te/ts; %50
NP=1;
p=0.6;
snr=1;
times=10;
sig=[];
%j=1  信号a持续ts*ns  j=2~10  信号c持续ts*ns,共9个   f(1)频率持续时间10*ts*ns=10*te = 0.5
%j=11 信号b持续ts*ns  j=12~20 信号c持续ts*ns,共9个   f(2)频率持续时间10*ts*ns=10*te = 0.5
%...
%j=71 信号b持续ts*ns  j=72~80 信号c持续ts*ns,共9个   f(8)频率持续时间10*ts*ns=10*te = 0.5
% 长度Nh*nsamp=8*500=80*ns=4000
for  j=1:80
    i=ceil(j/10);
    if j==1
       t=ts:ts:j*ns*ts;
       sig1=cos(2*pi*(f(i)+a*exp(-fc*t).*sin(2*pi*fc1*t)).*t);   %信号a
    elseif j==10*(i-1)+1
        t=(j*ns+1)*ts:ts:(j+1)*ns*ts;
        sig1=cos(2*pi*(f(i)+a*exp(-fc*(t-j*ns*ts)).*sin(2*pi*fc1*(t-j*ns*ts))).*t);%信号b
    else
        t=(j*ns+1)*ts:ts:(j+1)*ns*ts;
        sig1=cos(2*pi*f(i)*t);%信号c
    end
    sig=[sig sig1];
end
aaa = sig;
save cc.txt
%取出部分数据,并加10db噪声
sig=sig(251:3750);
sig=awgn(sig,snr,'measured');
figure(1);
subplot(211);
t=251*ts:ts:(Nh*nsamp-250)*ts;   %251-3500
plot(t,sig);
axis([251*ts,(Nh*nsamp-250)*ts -1.5,1.5]);
xlabel('时间/s');
ylabel('幅值');
title('跳频信号时域图');
subplot(212);
len=length(sig);
F=[-fs/2:fs/len:fs/2-fs/len]/1e6;  %-fs/2:fs/2
plot(F,abs(fftshift(fft(sig))));
title('跳频信号频谱图');
xlabel('频率/MHz');
ylabel('频谱幅度');
 
 
 
 
%%%%%%%%%%%%短时傅里叶变化%%%%%%%%%%%%
sig=sig';
t=1:length(sig);
N=512;
%设置窗函数1
h1=window('hamming',255);
% h1=ones(501,1);
% h1=gausswin(511);
% 计算短时傅立叶变换
% sig=hilbert(sig);
[S,T,F]=tfrsp(sig,t,N,h1);
figure(2);
contour(T,abs(F(1:N/2)),abs(S(1:N/2,:)));
% title('谱图法');
axis([1,3500 0,0.5]);
xlabel('时域采样点');
ylabel('归一化频率 F');


figure(3);
mesh(T,abs(F),2*abs(S));
axis([1,3500 0,0.5 0,120]);
title('谱图法');
xlabel('时域采样点');
ylabel('归一化频率 F');
zlabel('幅值');


[St,Ft]=max(S);
% a row vector containing the maximum element from each column.
% S是行向量,对应每一列的max value，F是对应max index
St=abs(St);
Ft=abs(Ft);
figure(4);
plot(T*ts*1e6,St);
title('跳频信号峰值包络y(n)');
xlabel('时间/us');
ylabel('峰值幅值');

figure(5);
F1=T*fs/length(T);
St = St - mean(St);
plot(F1,abs(fft(St)));
axis([0,2e5,0,2.5e4]);
title('y(n)的FFT谱');
xlabel('频率/Hz');
ylabel('FFT幅度');

F=Ft/N*fs/1e6-fs/N*1e-6;
figure(6);
plot(T*ts*1e6,abs(F));
title('时频脊线');
xlabel('时间/us');
ylabel('频率/MHz');
T_hop=diff(Ft);
 
figure(7);
T1=1:length(T_hop);
plot(T1*ts*1e6,T_hop);
title('跳频时刻');
xlabel('时间/us');
ylabel('幅值');
