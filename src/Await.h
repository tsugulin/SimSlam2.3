#ifndef AWAIT_H_
#define AWAIT_H_

#include <omnetpp.h>
using namespace omnetpp;
#include "Wavg.h"
#define MAX_QUEUE 100

class Resource;

class Await : public cSimpleModule
{
private:
  bool onSetup;
  bool onInterruption;
  cMessage *curMsg;
  cMessage *dummy;
  simtime_t expectedTime;
  simtime_t remainingTime;
  simtime_t suspendedTime;
  cQueue queue;
  Resource *rsc;
  cStdDev waittime;
  cStdDev proctime;
  Wavg queuelen;

protected:
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  virtual void finish();

public:
  virtual void startSetup();
  virtual void suspendSetup();
  virtual void resumeSetup();
  virtual void push();
  virtual void pop();
};

#endif /* AWAIT_H_ */
