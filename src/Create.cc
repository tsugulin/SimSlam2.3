// メッセージを繰り返し生産するノード

#include "Create.h"
Define_Module(Create);

void Create::initialize()
{
    beatMsg = new cMessage("beat");
    scheduleAt(simTime() + par("initialTime"), beatMsg);     // 最初のメッセージを送信
}

void Create::handleMessage(cMessage *msg)
{
    // CREATEノードに届くメッセージは全てbeatMsg
    ASSERT(msg == beatMsg);

    // 次のモジュールにメッセージを送信
    send(new cMessage(par("datatype")), "out");     // productまたはaccident

    //　intervalTimeの期間毎に"heartbeat"メッセージを自身に送信する
    double d = par("intervalTime");
    leadtime.collect(d);
    scheduleAt(simTime() + d, beatMsg);
}

//　ノードの統計情報を表示
void Create::finish()
{
    EV << "Create Jobs Count:   " << leadtime.getCount() << endl;
    EV << "Create Min Leadtime: " << leadtime.getMin() << endl;
    EV << "Create Avg Leadtime: " << leadtime.getMean() << endl;
    EV << "Create Max Leadtime: " << leadtime.getMax() << endl;
    EV << "Standard deviation:  " << leadtime.getStddev() << endl;

    leadtime.recordAs("Create LeadTime");
}
