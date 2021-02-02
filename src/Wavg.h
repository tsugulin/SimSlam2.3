#ifndef WAVG_H_
#define WAVG_H_

#include <omnetpp.h>
using namespace omnetpp;

class Wavg {
private:
    simtime_t start;
    simtime_t checkpoint;
    int size;
    double *length;

public:
    Wavg();
    virtual ~Wavg();
    virtual void init(simtime_t, int);
    virtual void set(simtime_t, int);
    virtual double get(simtime_t);
};

#endif /* WAVG_H_ */
