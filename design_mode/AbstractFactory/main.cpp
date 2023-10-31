#include "Factory.h"

int main(int argc, char **argv)
{
    AbstractSportProduct *pro = nullptr;
    AbstractFactory *fac = nullptr;

    fac = new BasketballFactory();
    pro = fac->getSportProduct();
    pro->play();

    fac = new FootballFactory();
    pro = fac->getSportProduct();
    pro->play();

    return 0;
}

/*
相较简单工厂模式，工厂方法模式更加符合开闭原则。工厂方法是使用频率最高的设计模式之一，是很多开源框架和API类库的核心模式。

优点：
工厂方法用于创建客户所需产品，同时向客户隐藏某个具体产品类将被实例化的细节，用户只需关心所需产品对应的工厂；
工厂自主决定创建何种产品，并且创建过程封装在具体工厂对象内部，多态性设计是工厂方法模式的关键；
新加入产品时，无需修改原有代码，增强了系统的可扩展性，符合开闭原则。

缺点：
添加新产品时，需要同时添加新的产品工厂，系统中类的数量成对增加，增加了系统的复杂度，更多的类需要编译和运行，增加了系统的额外开销；
工厂和产品都引入了抽象层，客户端代码中均使用的抽象层，增加了系统的抽象层次和理解难度。
*/