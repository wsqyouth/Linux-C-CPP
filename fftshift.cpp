//模仿matlab的fftshift.cpp进行C语言实现

#include <iostream>
#include <cstring>
using namespace std;

/*
 * 将数组的后半部分移动到前部分，前半部分移动到后半部分
 * https://ww2.mathworks.cn/help/matlab/ref/fftshift.html 
 * 表示例:A={1,2,3,4,5}; B=fftshift(A)={4,5,1,2,3}; C=ifftshift(B) = {3,4,5,1,2}
 */

void printArr(int arr[],int n) {
	for(int i=0;i<n;++i)
		cout << arr[i] << " ";
	cout << endl;
}

/*
 * 将in数组shift之后放到out中
 *空间复杂度O(n),时间复杂度O(n)
 */
void fftshiftInt(int *in, int *out, int dim)
{
    int pivot = ((dim&1) == 0) ? (dim / 2) : ((dim + 1) / 2); //dim % 2 == 0
    int outIndex = 0;
    for(int i=pivot;i<dim;++i)
        out[outIndex++] = in[i];
    for(int i=0;i<pivot;++i)
         out[outIndex++] = in[i];
}

void fftshift1D(int *in,int* out, int dim)
{
	int pivot = ((dim&1) == 0) ? (dim/2):((dim+1)/2);
	int rightHalf = dim - pivot;
	int leftHalf = pivot;
	memcpy(out,in+pivot,sizeof(int)*rightHalf);
	memcpy(out+rightHalf,in,sizeof(int)*leftHalf);
}

/*
 * 将in数组本地进行shift(in place)
 *空间复杂度O(n),时间复杂度O(n)
 */
void fftshift(int *in,int dim)
{
	//in is even
	if( (dim&1) == 0) {
		int pivot = dim/2;
		for(int pstart=0;pstart<pivot;++pstart) {//swap two elements
			int temp = in[pstart+pivot];
			in[pstart+pivot] = in[pstart];
			in[pstart] = temp;
		}
    }else { //in is odd
		int pivot = dim/2;
		int pivotElement = in[pivot];
		for(int pstart=0;pstart<pivot;++pstart) {//swap two elements
			int temp = in[pstart+pivot+1];
			in[pstart+pivot] = in[pstart];
			in[pstart] = temp; //将pivot+1对应的元素与pstart对应元素交换
		}
		in[dim-1] = pivotElement;
	}
}


/*
 * 参考代码
 * 主要使用模板参数的方法，着重比较fftshift和ifftshift的不同
 */
template<typename T>
inline void fftshift1D(T *in,T *out,int dim) {
	int pivot = ( (dim&1)==0 ) ? (dim/2) : ((dim+1)/2); // dim%2==0
	int rightHalf = dim - pivot;
	int leftHalf = pivot;
	memcpy(out,in+(pivot),sizeof(T)*rightHalf);
	memcoy(out+rightHalf,in,sizeof(T)*leftHalf);
}
template<typename T>
inline void ifftshift1D(T *in,T *out,int dim) {
	int pivot = ( (dim&1)==0 ) ? (dim/2) : ((dim-1)/2); // dim%2==0
	int rightHalf = dim - pivot;
	int leftHalf = pivot;
	memcpy(out,in+(pivot),sizeof(T)*rightHalf);
	memcpy(out+rightHalf,in,sizeof(T)*leftHalf);
}

int main() {
	const int N = 5;
	int arr[] = {1,2,3,4,5};
	int arrNew[N] = {0};

	printArr(arr,N);
//	fftshift1D(arr,arrNew,N);//
//	fftshift(arr,N);//in place进行shift
	fftshift1D(arr,arrNew,N);
//	ifftshift1D(arr,arrNew,N);
	printArr(arrNew,N);
	return 0;
}
