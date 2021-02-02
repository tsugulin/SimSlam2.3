// 統計処理を行った後にメッセージを消滅するノード

#include "Terminate.h"
Define_Module(Terminate);

void Terminate::handleMessage(cMessage *msg)
{
    simtime_t_cref d = simTime() - msg->getCreationTime();
    EV << "Received " << msg->getName() << ", leadtime: " << d << "sec" << endl;
    stats.collect( d );
    times.record( d );
    delete msg;
}

void Terminate::finish()
{
    EV << "Total jobs Count:              " << stats.getCount() << endl;
    EV << "Total jobs Min leadtime:       " << stats.getMin() << endl;
    EV << "Total jobs Avg leadtime:       " << stats.getMean() << endl;
    EV << "Total jobs Max leadtime:       " << stats.getMax() << endl;
    EV << "Total jobs Standard deviation: " << stats.getStddev() << endl;
}
