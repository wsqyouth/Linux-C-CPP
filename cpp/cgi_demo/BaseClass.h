#ifndef BASECLASS_H
#define BASECLASS_H

// 声明命名空间和其中的类
namespace Comm
{
    class Input
    {
        // Input的成员和方法
    };

    class Output
    {
        // Output的成员和方法
    };
}

class BaseClass
{
public:
    BaseClass() {}
    virtual ~BaseClass() {}

protected:
    virtual int BeforeProcess(Comm::Input *input, Comm::Output *output); // 基类需重载函数
    virtual void PrintHelloWorld();                                      // 普通函数
};

#endif // BASECLASS_H