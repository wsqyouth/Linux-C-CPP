#include "SimpleFactory.h"

// Basketball方法
void Basketball::printName()
{
    printf("Basketball\n");
}
void Basketball::play()
{
    printf("play Basketball\n");
}

// Football方法
void Football::printName()
{
    printf("Football\n");
}
void Football::play()
{
    printf("play Football\n");
}

// 工厂方法
AbstractSportProduct *Factory::getSportProduct(std::string productName)
{
    AbstractSportProduct *pro = NULL;
    if (productName == "Basketball")
    {
        pro = new Basketball();
    }
    else if (productName == "Football")
    {
        pro = new Football();
    }
    return pro;
}
