#include "FinalClass.h"
using namespace std;

/*
int main()
{
    FinalClass fc;
    Comm::Input input;
    Comm::Output output;
    fc.Process(&input, &output);
    return 0;
}
*/

// 抽象工厂类
class AbstractFactory
{
public:
    virtual BaseClass *create() = 0; // 纯虚函数
    virtual ~AbstractFactory() {}    // 虚析构函数
};

// IntermediateFactory是AbstractFactory的一个具体子类，它创建IntermediateClass的实例
class IntermediateFactory : public AbstractFactory
{
public:
    BaseClass *create() override
    {
        return new IntermediateClass();
    }
};

// FinalFactory也是AbstractFactory的一个具体子类，它创建FinalClass的实例
class FinalFactory : public AbstractFactory
{
public:
    BaseClass *create() override
    {
        return new FinalClass();
    }
};

// BaseProcessSvr类
class BaseProcessSvr
{
public:
    BaseProcessSvr(const string &name) : name(name) {}

    void addFactory(AbstractFactory *factory)
    {
        this->factory = factory;
    }

    int run(int argc, char **argv)
    {
        BaseClass *base = factory->create();
        base->doSomething();
        delete base;
        return 0;
    }

private:
    string name;
    AbstractFactory *factory;
};

int main(int argc, char **argv)
{
    BaseProcessSvr svr("SRV_NAME");
    svr.addFactory(new IntermediateFactory());
    svr.run(argc, argv);

    svr.addFactory(new FinalFactory());
    return svr.run(argc, argv);
}

/*
总结:
该demo主要学习cpp中类派生、基类中虚函数重载功能。如果在基类中添加共用函数,则子类可复用,从而满足SOLID原则
抽离为基类、中间类、子类,以更好的进行学习，同时使用makefile进行工程管理
抽象工厂模式: 具体工厂继承自抽象工厂,产生单独的实例, 利用多态
*/