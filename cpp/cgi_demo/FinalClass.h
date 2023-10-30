#ifndef FINALCLASS_H
#define FINALCLASS_H

#include "IntermediateClass.h"

class FinalClass : public IntermediateClass
{
public:
    FinalClass();
    ~FinalClass();

    void Process(Comm::Input *input, Comm::Output *output);

protected:
    virtual int BeforeProcess(Comm::Input *input, Comm::Output *output) override;
};

#endif // FINALCLASS_H