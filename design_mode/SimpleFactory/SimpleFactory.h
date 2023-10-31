#ifndef SIMPLEFACTORY_H
#define SIMPLEFACTORY_H
#include <iostream>
#include <string>

//抽象产品类AbstractProduct(并非真正意义上的抽象类，含有纯虚函数才是抽象类)
class AbstractSportProduct
{
public:
	AbstractSportProduct(){}
	~AbstractSportProduct(){}

	//虚函数
	virtual void printName(){}
	virtual void play(){}
};

//具体产品类Basketball
class Basketball :public AbstractSportProduct
{
public:
	Basketball(){}
	~Basketball(){}

	//具体实现方法
	void printName();	
	void play();
};

//具体产品类Football
class Football :public AbstractSportProduct
{
public:
	Football(){}
	~Football(){}

	//具体实现方法
	void printName();
	void play();
};

//工厂类
class Factory
{
public:
	Factory(){}
	~Factory(){}

	AbstractSportProduct *getSportProduct(std::string productName);
};

#endif // TSIMPLEFACTORY_H
