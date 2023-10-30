#ifndef BASECLASS_H
#define BASECLASS_H

class Input
{
};
class Output
{
};

class BaseClass
{
public:
    BaseClass() {}
    virtual ~BaseClass() {}

protected:
    virtual int BeforeProcess(Input *input, Output *output); // 基类需重载函数
    virtual void PrintHelloWorld();                          // 普通函数
};

#endif // BASECLASS_H