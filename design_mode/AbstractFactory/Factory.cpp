#include "Factory.h"

/********************************产品类方法***********************************/
//Basketball方法
void Basketball::play(){
	printf("play Basketball\n");
}

//Football方法
void Football::play(){
	printf("play Football\n");
}

/********************************工厂类方法***********************************/
//BasketballFactory方法
AbstractSportProduct *BasketballFactory::getSportProduct()
{
	return new Basketball();
}

//FootballFactory方法
AbstractSportProduct *FootballFactory::getSportProduct()
{
	return new Football();
}
