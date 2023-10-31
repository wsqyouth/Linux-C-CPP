#include "SimpleFactory.h"

int main(int argc, char **argv)
{
    Factory *fac = new Factory();
    AbstractSportProduct *product = NULL;

    product = fac->getSportProduct("Basketball");
    product->printName();
    product->play();

    product = fac->getSportProduct("Football");
    product->printName();
    product->play();

    return 0;
}

/*
总结
优点：
工厂类提供创建具体产品的方法，并包含一定判断逻辑，客户不必参与产品的创建过程；
客户只需要知道对应产品的参数即可，参数一般简单好记，如数字、字符或者字符串等。

缺点：
在扩展功能时，需要增加具体的产品类和修改工厂类，所以违背了开闭原则（对扩展开放，对修改关闭）。

参考: https://www.cnblogs.com/lcgbk/p/13819263.html
*/