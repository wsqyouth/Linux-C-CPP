#include "AdvertiserIndex.h"
#include <iostream>

AdvertiserIndex::AdvertiserIndex()
{
    InitCgiConfig(1, 2, "example_key"); // Initialize the base class members with the desired values
}

AdvertiserIndex::~AdvertiserIndex() {}

void AdvertiserIndex::Process(Input *input, Output *output)
{
    BeforeProcess(input, output);

    std::string appid = "example_appid";
    uint32_t adq_type, adq_uid;
    genTypeAndStr(appid, adq_type, adq_uid);
    std::cout << "In AdvertiserIndex::Process, adq_type: " << adq_type << ", adq_uid: " << adq_uid << std::endl;

    PrintHelloWorld();
}

int AdvertiserIndex::BeforeProcess(Input *input, Output *output)
{
    int iRet = SkeyCgi::BeforeProcess(input, output);

    if (iRet != 0)
    {
        return iRet;
    }

    std::cout << "AdvertiserIndex::BeforeProcess called" << std::endl;

    return 0;
}