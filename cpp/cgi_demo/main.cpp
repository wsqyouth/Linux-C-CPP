#include "FinalClass.h"

int main()
{
    FinalClass fc;
    Comm::Input input;
    Comm::Output output;
    fc.Process(&input, &output);
    return 0;
}

/*
总结:
该demo主要学习cpp中类派生、基类中虚函数重载功能。如果在基类中添加共用函数,则子类可复用,从而满足SOLID原则
抽离为基类、中间类、子类,以更好的进行学习，同时使用makefile进行工程管理
*/