#ifndef GOON_H_
#define GOON_H_

#include <omnetpp.h>
using namespace omnetpp;

class Await;

class Goon : public cSimpleModule
{
private:
  bool onProduction;
  bool onInterruption;
  cMessage *curMsg;
  simtime_t expectedTime;
  simtime_t remainingTime;
  simtime_t suspendedTime;
  Await *setupNode;
  cStdDev proctime;

protected:
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  virtual void finish();

public:
  virtual void suspendProduction();
  virtual void resumeProduction();
};

#endif /* GOON_H_ */
