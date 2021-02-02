#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <omnetpp.h>
using namespace omnetpp;

class Await;
class Goon;

class Resource : public cSimpleModule
{
private:
    std::string ownerType;
    long owner;
    long interruption;
    Await *setupNode;
    Goon *processNode;

protected:
  virtual void initialize();

public:
  virtual bool request(std::string name, long id);
  virtual bool release(std::string name, long id);
};

#endif /* RESOURCE_H_ */