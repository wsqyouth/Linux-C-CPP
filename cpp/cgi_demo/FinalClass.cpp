#include "FinalClass.h"
#include <iostream>

FinalClass::FinalClass()
{
    InitCgiConfig(1, 2, "example_key"); // Initialize the base class members with the desired values
}

FinalClass::~FinalClass() {}

void FinalClass::Process(Input *input, Output *output)
{
    BeforeProcess(input, output);

    std::string appid = "example_appid";
    uint32_t adq_type, adq_uid;
    genTypeAndStr(appid, adq_type, adq_uid);
    std::cout << "In FinalClass::Process, adq_type: " << adq_type << ", adq_uid: " << adq_uid << std::endl;

    PrintHelloWorld();
}

int FinalClass::BeforeProcess(Input *input, Output *output)
{
    int iRet = IntermediateClass::BeforeProcess(input, output);

    if (iRet != 0)
    {
        return iRet;
    }

    std::cout << "FinalClass::BeforeProcess called" << std::endl;

    return 0;
}