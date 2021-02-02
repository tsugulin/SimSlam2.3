#ifndef PREEMPT_H_
#define PREEMPT_H_

#include <omnetpp.h>
using namespace omnetpp;

class Resource;

class Preempt : public cSimpleModule
{
private:
  Resource *rsc;
  cStdDev proctime;

protected:
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  virtual void finish();
};

#endif /* PREEMPT_H_ */
