// 前工程からメッセージを受け取り，リソースを解放して指定時間（omenetpp.iniのintervalTimeにて指定）経過後に後工程に送る

#include "Release.h"
Define_Module(Release);
#include "Resource.h"

void Release::initialize()
{
    // リソースのポインタを取得
    cModule *mod = gate("res")->getNextGate()->getOwnerModule();
    rsc = check_and_cast<Resource *>(mod);
}

void Release::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // 指定時間を経過した後
        proctime.collect(simTime() - msg->getTimestamp());      // 統計情報に保管
        send(msg, "out");                                       // 後工程に送る
    }
    else {
        // 前工程から届いたメッセージの場合
        msg->setTimestamp(simTime());                       // リードタイムの開始時間をセット
        rsc->release( msg->getName(), msg->getId() );       // リソースの開放
        scheduleAt(simTime() + par("intervalTime"), msg);   // 終了時刻にSelfMessageを送る
    }
}

//　ノードの統計情報を表示
void Release::finish()
{
    EV << "Release Min ProcTime: " << proctime.getMin() << endl;
    EV << "Release Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Release Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation:   " << proctime.getStddev() << endl;

    proctime.recordAs("Release ProcTime");
}
