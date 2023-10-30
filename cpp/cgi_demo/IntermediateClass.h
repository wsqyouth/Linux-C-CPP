#ifndef INTERMEDIATECLASS_H
#define INTERMEDIATECLASS_H

#include "BaseClass.h"
#include <string>

class IntermediateClass : public BaseClass
{
public:
    IntermediateClass();
    virtual ~IntermediateClass();

    int genTypeAndStr(const std::string &appid, uint32_t &adq_type, uint32_t &adq_uid);
    void InitCgiConfig(const uint32_t gdt_modid, const uint32_t gdt_cmdid, const std::string &gdt_key);

protected:
    virtual int BeforeProcess(Comm::Input *input, Comm::Output *output);
    virtual void PrintHelloWorld() override;

private:
    uint32_t m_gdt_modid = 0;
    uint32_t m_gdt_cmdid = 0;
    char m_gdt_key[128];
};

#endif // INTERMEDIATECLASS_H
