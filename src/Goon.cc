// 前工程からメッセージを受け取り，指定時間（omenetpp.iniのproductionTimeにて指定）経過後に後工程に送る
// Resouceモジュールからの要求により処理の中断/再開を行う．処理中のメッセージはAwaitノードのキューに保管する

#include "Goon.h"
Define_Module(Goon);
#include "Await.h"

void Goon::initialize()
{
    // 変数の初期化
    onProduction = false;
    onInterruption = false;

    // リソースのポインタを取得
    cModule *mod = gate("in")->getPreviousGate()->getOwnerModule();
    setupNode = check_and_cast<Await *>(mod);
}

void Goon::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // 処理完了時
        proctime.collect(simTime() - msg->getTimestamp());      // 統計情報に保管
        send(msg, "out");                                       // 後工程に送る
        onProduction = false;                                   // 生産終了
    }
    else {
        // 前工程から届いたメッセージの場合
        onProduction = true;                                // 生産開始
        msg->setTimestamp(simTime());                       // リードタイムの開始時間をセット
        onProduction = true;                                // 生産ON
        expectedTime = simTime() + par("productionTime");   // 予定時刻
        curMsg = msg;
        scheduleAt(expectedTime, msg);                      // 生産開始
    }
}

//　処理を中断．残り時間を記録する
void Goon::suspendProduction()
{
    Enter_Method("suspend");
    if ( onProduction ) {
        // 生産中の場合
        onProduction = false;                       // 生産停止
        onInterruption = true;                      // 割り込み中
        cancelEvent(curMsg);                        // 生産をキャンセル
        setupNode->push();                          // 待ち行列の先頭にメッセージを戻す
        suspendedTime = simTime();                  // 中断開始時刻
        remainingTime = expectedTime - simTime();   // 残り時間
    }
}

//　処理を再開．残りの処理を行う
void Goon::resumeProduction()
{
    Enter_Method("resume");
    if ( onInterruption ) {
        // 割り込み中の場合
        onInterruption = false;                     // 割り込みを解除
        onProduction = true;                        // 生産再開フラグ
        setupNode->pop();                           // 待ち行列からメッセージを取り出す
        expectedTime = simTime() + remainingTime;   // 予定時刻
        curMsg->setTimestamp(curMsg->getTimestamp() + simTime() - suspendedTime);  // 開始時間を補正
        scheduleAt(expectedTime, curMsg);           // 生産再開
    }
}

//　ノードの統計情報を表示
void Goon::finish()
{
    EV << "Process Utilization:  " << proctime.getSum() / simTime() << endl;
    EV << "Process Min ProcTime: " << proctime.getMin() << endl;
    EV << "Process Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Process Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation:   " << proctime.getStddev() << endl;

    proctime.recordAs("Process ProcTime");
}

