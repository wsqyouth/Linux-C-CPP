#include "BaseClass.h"
#include <iostream>

int BaseClass::BeforeProcess(Comm::Input *input, Comm::Output *output)
{
    std::cout << "BaseClass::BeforeProcess from BaseClass!" << std::endl;
    return 0;
}

void BaseClass::PrintHelloWorld()
{
    std::cout << "Hello, World from BaseClass!" << std::endl;
}
