// 前工程からメッセージを受け取るとリソースを獲得し，指定時間の経過後に後工程にメッセージを送る

#include "Preempt.h"
Define_Module(Preempt);
#include "Resource.h"

void Preempt::initialize()
{
    // リソースのポインタを取得
    cModule *mod = gate("res")->getNextGate()->getOwnerModule();
    rsc = check_and_cast<Resource *>(mod);
}

void Preempt::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // セルフメッセージ=生産完了時
        simtime_t_cref d = simTime() - msg->getTimestamp();     // 処理時間
        proctime.collect(d);                                    // 統計情報に保管
        send(msg, "out");                                       // 後工程に送る
    }
    else {
        // 前工程から届いたメッセージの場合
        if ( rsc->request(msg->getName(), msg->getId()) ) {         // TOOLリソースを要求
            msg->setTimestamp(simTime());                           // リードタイムの開始時間をセット
            scheduleAt(simTime() + par("productionTime"), msg);     // 生産終了後にSelfMessageを送る
        }
    }
}

//　ノードの統計情報を表示
void Preempt::finish()
{
    EV << "Preempt Jobs Count:   " << proctime.getCount() << endl;
    EV << "Preempt Utilization:  " << proctime.getSum() / simTime() << endl;
    EV << "Preempt Min ProcTime: " << proctime.getMin() << endl;
    EV << "Preempt Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Preempt Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation:   " << proctime.getStddev() << endl;

    proctime.recordAs("Prermpt ProcTime");
}
