#ifndef ADVERTISERINDEX_H
#define ADVERTISERINDEX_H

#include "SkeyCgi.h"

class AdvertiserIndex : public SkeyCgi
{
public:
    AdvertiserIndex();
    ~AdvertiserIndex();

    void Process(Comm::Input *input, Comm::Output *output);

protected:
    virtual int BeforeProcess(Comm::Input *input, Comm::Output *output) override;
};

#endif // ADVERTISERINDEX_H