fft之前需要做一步倒序操作，这里提供了两种思路。
一种是简单的，一种是雷德算法。
倒序前坐标： 0 1 2 3 4 5 6 7
倒序后坐标： 0 4 2 6 1 5 3 7

雷德算法
一个小算法的感觉。
拿一个0到2^n-1的自然数序列。
比方说
0 1 2 3 4 5 6 7
我们转换为二进制状态，那么这个序列就是
000 001 010 011 100 101 110 111
接下来我们模拟FFT的位置交换，即：
0 1 2 3 4 5 6 7
0 2 4 6 1 3 5 7
0 4 2 6 1 5 3 7
发现最终的序列变为了
000 100 010 110 001 101 011 111
雷德算法就是用于求出这个倒序的数列。
那么开始抄别人的玩意：
由上面的表可以看出，按自然顺序排列的二进制数，其后面一个数总是比其前面一个数大1，即后面一个数是前面一个数在最低位加1并向高位进位而得到的。
而倒位序二进制数的后面一个数是前面一个数在最高位加1并由高位向低位进位而得到。 
I、J都是从0开始，若已知某个倒位序J，要求下一个倒位序数：
应先判断J的最高位是否为0。
这可与k=N/2相比较，因为N/2总是等于100的。
如果k>J，则J的最高位为0，只要把该位变为1（J与k=N/2相加即可），就得到下一个倒位序数；
如果K<=J，则J的最高位为1，可将最高位变为0（J与k=N/2相减即可）。然后还需判断次高位，这可与k=N\4相比较，若次高位为0，
         则需将它变为1（加N\4即可）其他位不变，既得到下一个倒位序数；若次高位是1，则需将它也变为0。然后再判断下一位……
//假设N为2的整数次幂
void RaderReverse(int *arr, int N) {
	int j,k;
	//第一个和最后一个数位置不变，故不处理
	for(int i=1,j=N/2; i<N-1; ++i) {
		//原始坐标小于变换坐标才交换，防止重复
		if(i<j) {
			int temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
		}
		k = N/2; // 用于比较最高位
		while(k <= j) { // 位判断为1
			j = j-k;// 该位变为0
			k = k/2;// 用于比较下一高位
		}
		j = j+k;// 判断为0的位变为1
	}
}

-----

#include <iostream>
using namespace std;

void printArr(int arr[],int n) {
	for(int i=0;i<n;++i)
		cout << arr[i] << " ";
	cout << endl;
}

void bitrp (int xreal [], int ximag [], int n)
{
    // 位反转置换 Bit-reversal Permutation
    int i, j, a, b, p;
 
	for (i = 1, p = 0; i < n; i *= 2)
	{
		p ++;
	}
	cout << p << endl;

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
			cout << "swap:" << i << b << endl;
			swap (xreal [i], xreal [b]);
			swap (ximag [i], ximag [b]);
		}
	}
}
//要求n必须为2的整数次幂
//将/2的地方全部改为为运算
void RaderReverse(int arr[],int n) {
	int j,k;
	//第一个和最后一个数坐标无变化，故不处理
		for(int i=1,j=n>>1;i<n-1;++i) {
		//原始坐标小于变换坐标才交换
		if( i<j) {
			int temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
		}
		k = n>>1;//用于比较最高位
		while(k <= j) { //位判断为1
			j = j-k;//该位变为0
			k = k>>1; //用于比较次高位
		}
		j = j+k; //位判断为0则将该位变为1
	}
		
}
int main() {
	int xreal[]={0,1,2,3,4,5,6,7};
	int ximag[]={0,0,0,0,0,0,0,0};
	//bitrp(xreal,ximag,8);
	RaderReverse(xreal,8);
	printArr(xreal,8);
	return 0;
}
