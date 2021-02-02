#ifndef TERMINATE_H_
#define TERMINATE_H_

#include <omnetpp.h>
using namespace omnetpp;

class Terminate : public cSimpleModule
{
private:
    cStdDev stats;
    cOutVector times;

protected:
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif /* TERMINATE_H_ */
