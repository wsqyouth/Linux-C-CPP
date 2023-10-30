#ifndef ADVERTISERINDEX_H
#define ADVERTISERINDEX_H

#include "SkeyCgi.h"

class AdvertiserIndex : public SkeyCgi
{
public:
    AdvertiserIndex();
    ~AdvertiserIndex();

    void Process(Input *input, Output *output);

protected:
    virtual int BeforeProcess(Input *input, Output *output) override;
};

#endif // ADVERTISERINDEX_H