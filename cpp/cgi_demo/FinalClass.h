#ifndef FINALCLASS_H
#define FINALCLASS_H

#include "IntermediateClass.h"

class FinalClass : public IntermediateClass
{
public:
    FinalClass();
    ~FinalClass();

    void Process(Input *input, Output *output);

protected:
    virtual int BeforeProcess(Input *input, Output *output) override;
};

#endif // FINALCLASS_H