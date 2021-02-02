#ifndef RELEASE_H_
#define RELEASE_H_

#include <omnetpp.h>
using namespace omnetpp;

class Resource;

class Release : public cSimpleModule
{
private:
  Resource *rsc;
  cStdDev proctime;

protected:
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  virtual void finish();
};

#endif /* RELEASE_H_ */
