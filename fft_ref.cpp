参考文章：https://blog.csdn.net/tuwenqi2013/article/details/71772841，重要的是那个图

#include <math.h>  
#include <stdio.h>
#include <stdlib.h>  
#include <sys/time.h>

void timeSubtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
}

/*定义复数类型*/  
typedef struct{  
	double real;  
	double img;  
}complex;  
  
  
#define N 32
complex x[N], *W;	/*输入序列,变换核*/  
int size_x=0;       /*输入序列的大小，在本程序中仅限2的次幂*/  
double PI;          /*圆周率*/  
void fft();         /*快速傅里叶变换*/  
void initW();       /*初始化变换核*/  
void change();      /*变址*/  
void add(complex ,complex ,complex *); /*复数加法*/  
void mul(complex ,complex ,complex *); /*复数乘法*/  
void sub(complex ,complex ,complex *); /*复数减法*/  
void output();	    /*输出快速傅里叶变换的结果*/  
  
  
  
  
int main()  
{  
    int i;          /*输出结果*/  
	struct timeval tvBegin, tvEnd, tvDiff;

    PI=atan(1)*4;  
#if 0
    printf(" 输出DIT方法实现的FFT结果\n");  
    printf("Please input the size of x:\n");
    scanf("%d",&size_x);  
    printf("Please input the data in x[N]:\n");  
    for(i=0;i<size_x;i++)  
    {  
        printf("请输入第%d个序列：",i);  
        scanf("%lf%lf",&x[i].real,&x[i].img);  
    }  
    printf("输出倒序后的序列\n"); 
#endif

	gettimeofday(&tvBegin, NULL);
	size_x  = N;//输入序列的大小  
    for (int i=0; i < 100000; i++) {
		/* Init inputs */
		for (int i=0; i < N; i++) {
		    x[i].real = (float) i;//输入序列的实部和虚部
		    x[i].img = 0.0;
		}
         initW();				  //调用变换核  
		 fft();                   //调用快速傅里叶变换  
    }
    gettimeofday(&tvEnd, NULL);

    timeSubtract(&tvDiff, &tvEnd, &tvBegin);
	 
    printf("100000 x Naive: \t %ld.%d\n", tvDiff.tv_sec, tvDiff.tv_usec);
 
    output();//调用输出傅里叶变换结果函数  
    return 0;  
}  
  
  
  
/*快速傅里叶变换*/  
void fft()  
{  
    int i=0,j=0,k=0,l=0;  
    complex up,down,product;  
    change();  //调用变址函数  
    for(i=0;i< log(size_x)/log(2) ;i++)        /*一级蝶形运算 stage */  
    {     
        l=1<<i;  
        for(j=0;j<size_x;j+= 2*l )     /*一组蝶形运算 group,每组group的蝶形因子乘数不同*/  
        {              
            for(k=0;k<l;k++)        /*一个蝶形运算 每个group内的蝶形运算的蝶形因子乘数成规律变化*/  
            {         
                mul(x[j+k+l],W[size_x*k/2/l],&product);  
                add(x[j+k],product,&up);  
                sub(x[j+k],product,&down);  
                x[j+k]=up;  
                x[j+k+l]=down;  
            }  
        }  
    }  
}  
  
  
  
/*初始化变换核，定义一个变换核，相当于旋转因子WAP*/  
void initW()   
{  
    int i;  
    W=(complex *)malloc(sizeof(complex) * size_x);  //生成变换核  
    for(i=0;i<size_x;i++)  
    {  
        W[i].real=cos(2*PI/size_x*i);   //用欧拉公式计算旋转因子  
        W[i].img=-1*sin(2*PI/size_x*i);  
    }  
}  
  
  
  
/*变址计算，将x(n)码位倒置*/  
void change()        
{  
  complex temp;  
  unsigned short i=0,j=0,k=0;  
  double t;  
  for(i=0;i<size_x;i++)  
  {  
	  k=i;j=0;  
	  t=(log(size_x)/log(2));  
	  while( (t--)>0 )    //利用按位与以及循环实现码位颠倒  
	  {  
		  j=j<<1;  
		  j|=(k & 1);  
		  k=k>>1;  
	  }  
	  if(j>i)    //将x(n)的码位互换  
	  {  
		  temp=x[i];  
		  x[i]=x[j];  
		  x[j]=temp;  
	  }  
  }  
  //output();  
}  
  
  
  
/*输出傅里叶变换的结果*/  
void output()  
{  
    int i;  
    printf("The result are as follows：\n");  
    for(i=0;i<size_x;i++)  
    {  
        printf("%.4f",x[i].real);  
        if(x[i].img>=0.0001)printf("+%.4fj\n",x[i].img);  
        else if(fabs(x[i].img)<0.0001)printf("\n");  
        else printf("%.4fj\n",x[i].img);  
    }  
}  
  
  
void add(complex a,complex b,complex *c)  //复数加法的定义  
{  
    c->real=a.real+b.real;  
    c->img=a.img+b.img;  
}  
  
  
void mul(complex a,complex b,complex *c)  //复数乘法的定义  
{  
    c->real=a.real*b.real - a.img*b.img;  
    c->img=a.real*b.img + a.img*b.real;  
}  
  
  
void sub(complex a,complex b,complex *c)  //复数减法的定义  
{  
    c->real=a.real-b.real;  
    c->img=a.img-b.img;  
}  
