#ifndef FACTORY_H
#define FACTORY_H
#include <iostream>
#include <string>

/********************************产品类***********************************/
//抽象产品类AbstractProduct(并非真正意义上的抽象类，含有纯虚函数才是抽象类)
class AbstractSportProduct
{
public:
	AbstractSportProduct(){}
	~AbstractSportProduct(){}
    
	//虚函数
	virtual void play(){}
};

//具体产品类Basketball
class Basketball :public AbstractSportProduct
{
public:
	Basketball(){}
	~Basketball(){}

	//具体实现方法
	void play();
};

//具体产品类Football
class Football :public AbstractSportProduct
{
public:
	Football(){}
	~Football(){}

	//具体实现方法
	void play();
};

/********************************工厂类***********************************/
//抽象工厂类
class AbstractFactory{
public:
	//纯虚函数
	virtual AbstractSportProduct *getSportProduct() = 0;
};

//具体工厂类BasketballFactory
class BasketballFactory:public AbstractFactory
{
public:
	BasketballFactory(){}
	~BasketballFactory(){}

	AbstractSportProduct *getSportProduct();
};

//具体工厂类FootballFactory
class FootballFactory:public AbstractFactory 
{
public:
	FootballFactory(){}
	~FootballFactory(){}

	AbstractSportProduct *getSportProduct();
};

#endif // FACTORY_H
