针对较小的工程使用makefile，大的工程一般使用Cmake

需要注意的是：Makefile的依赖需要考虑.h文件的可变性。


myadd.cpp
---
#ifndef _ADD_H_
#define _ ADD_H_

int myadd(int a,int b);

#endif


myadd.h
---
#include "myadd.h"
int myadd(int a,int b) {
	return  a+b;
}


main.cpp
---
#include <iostream>       
#include <string.h>
#include <cstdlib>
#include <vector> 
#include <algorithm>
#include "myadd.h"

int main() {
	std::cout<<"sum:"<<myadd(3,2)<<std::endl;
	return 0;
}


makefile
---
helloworld:helloworld.o myadd.o
	g++ -o helloworld helloworld.o  myadd.o
heloworld.o:helloworld.cpp myadd.h
	g++ -c  -o helloworld.o helloworld.cpp  
myadd.o: myadd.h
	g++ -c -o myadd.o myadd.cpp
  
  
Makefile进一步的优化：
https://www.cnblogs.com/owlman/p/5514724.html
