#ifndef CREATE_H_
#define CREATE_H_

#include <omnetpp.h>
using namespace omnetpp;

class Create : public cSimpleModule
{
  private:
    cMessage *beatMsg;
    cStdDev leadtime;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif /* CREATE_H_ */
