#include "IntermediateClass.h"
#include <iostream>
#include <cstring> // for std::strncpy

IntermediateClass::IntermediateClass() {}

IntermediateClass::~IntermediateClass() {}

int IntermediateClass::genTypeAndStr(const std::string &appid, uint32_t &adq_type, uint32_t &adq_uid)
{
    std::cout << "IntermediateClass::genTypeAndStr called with appid: " << appid << std::endl;
    adq_type = 123;
    adq_uid = 456;
    return 0;
}

void IntermediateClass::InitCgiConfig(const uint32_t gdt_modid, const uint32_t gdt_cmdid, const std::string &gdt_key)
{
    m_gdt_modid = gdt_modid;
    m_gdt_cmdid = gdt_cmdid;
    std::strncpy(m_gdt_key, gdt_key.c_str(), sizeof(m_gdt_key) - 1);
    m_gdt_key[sizeof(m_gdt_key) - 1] = '\0'; // Ensure null-termination
}

int IntermediateClass::BeforeProcess(Comm::Input *input, Comm::Output *output)
{
    std::cout << "IntermediateClass::BeforeProcess called" << std::endl;
    return 0;
}

void IntermediateClass::PrintHelloWorld()
{
    BaseClass::PrintHelloWorld();
    std::cout << "Hello, World from IntermediateClass!" << std::endl;
}