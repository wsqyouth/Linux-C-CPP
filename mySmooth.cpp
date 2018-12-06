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
