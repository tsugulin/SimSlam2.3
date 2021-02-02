// 前工程からのメッセージを受取り，resouceブロックのリソースを獲得してセットアップ時間（omenetpp.iniのproductionTimeにて指定）経過後に後工程にメッセージを送る
// もしリソースを獲得できない場合は待ち行列にメッセージを保管してリソースの解放を待つ
// Resouceモジュールからの要求によりセットアップを中断/再開を行う
// Goonモジュールからの要求により待ち行列へのメッセージ保管/取り出しを行う

#include "Await.h"
Define_Module(Await);
#include "Resource.h"

void Await::initialize()
{
    // 変数の初期化
    onSetup = false;
    onInterruption = false;
    remainingTime = 0;
    dummy = new cMessage("dummy");

    // キューの初期化
    queue.setName("queue");     // GUIに待ち行列長さを表示するための名前

    // 待ち行列の平均長さを計算するクラスを初期化
    queuelen.init(simTime(), MAX_QUEUE);

    // リソースのポインタを取得
    cModule *mod = gate("res")->getNextGate()->getOwnerModule();
    rsc = check_and_cast<Resource *>(mod);
}

void Await::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // 自身のメッセージであれば後工程に送る
        proctime.collect(simTime() - msg->getTimestamp());      // 統計情報に保管
        send(msg, "out");                                       // 後工程に送る
        onSetup = false;                                        // セットアップ終了
    }
    else {
        // 前工程から届いたメッセージの場合
        queuelen.set(simTime(), queue.getLength());     // 待ち行列平均長さを計算
        msg->setTimestamp(simTime());       // キュー滞留開始時間をセット
        queue.insert(msg);                  // 待ち行列の最後尾にめメッセージを追加
        startSetup();       // 待ち行列に溜まっているメッセージを処理する
    }
}

// 在庫があればセットアップを開始
void Await::startSetup(void)
{
    Enter_Method("start");
    if ( !onSetup ) {
        if (queue.getLength() > 0) {
            cMessage *msg = check_and_cast<cMessage *>(queue.front());      // 待ち行列の先頭メッセージを取り出す
            if ( rsc->request(msg->getName(), msg->getId()) ) {             // TOOLリソースを要求
                waittime.collect(simTime() - msg->getTimestamp());          // キュー滞留時間の統計処理
                queuelen.set(simTime(), queue.getLength());                 // 待ち行列平均長さを計算
                queue.pop();                                                // 待ち行列からメッセージを取り出す
                msg->setTimestamp(simTime());                               // リードタイムの開始時間をセット
                curMsg = msg;                                               // 中断処理のため現メッセージを保管
                onSetup = true;                                             // セットアップON
                expectedTime = simTime() + par("productionTime");           // 予定時刻
                scheduleAt(expectedTime, msg);                              // セットアップ開始
            }
        }
    }
}

// resouceブロックの要求によりセットアップを中断．残り時間を記録する
void Await::suspendSetup()
{
    Enter_Method("suspend");
    if ( onSetup ) {
        // セットアップ中の場合
        onSetup = false;                            // セットアップ停止
        onInterruption = true;                      // 割り込み中
        cancelEvent(curMsg);                        // セットアップをキャンセル
        queuelen.set(simTime(), queue.getLength()); // 待ち行列平均長さを計算
        if (queue.getLength() > 0) queue.insertBefore(queue.front(), dummy); else queue.insert(dummy);// 待ち行列の先頭にメッセージを戻す
        suspendedTime = simTime();                  // 中断開始時刻
        remainingTime = expectedTime - simTime();   // 残り時間
    }
}

// resouceブロックの要求によりセットアップを再開．残りの処理を行う
void Await::resumeSetup()
{
    Enter_Method("resume");
    if ( onInterruption ) {
        // 割り込み中の場合
        onInterruption = false;                         // 割り込みを解除
        onSetup = true;                                 // セットアップ再開フラグ
        queuelen.set(simTime(), queue.getLength());     // 待ち行列平均長さを計算
        queue.pop();                                    // 待ち行列からメッセージを取り出す
        expectedTime = simTime() + remainingTime;       // 予定時刻
        curMsg->setTimestamp(curMsg->getTimestamp() + simTime() - suspendedTime);  // 開始時間を補正
        scheduleAt(expectedTime, curMsg);               // セットアップ再開
    }
}

//　Goonモジュールの要求によりキューの先頭にメッセージをセット
void Await::push()
{
    Enter_Method("push");
    if ( !onSetup ) {
        // セットアップ中で無ければ
        queuelen.set(simTime(), queue.getLength());     // 待ち行列平均長さを計算
        if (queue.getLength() > 0) queue.insertBefore(queue.front(), dummy); else queue.insert(dummy);// 待ち行列の先頭にメッセージを戻す
    }
}

//　Goonモジュールの要求によりキューからメッセージを戻す
void Await::pop()
{
    Enter_Method("pop");
    if ( !onSetup ) {
        // セットアップ中で無ければ
        if (queue.getLength() > 0) {
            queuelen.set(simTime(), queue.getLength());     // 待ち行列平均長さを計算
            queue.pop();                                    // 待ち行列からメッセージを取り出す
        }
    }
}

//　ノードの統計情報を表示
void Await::finish()
{
    EV << "Await Jobs Count:   " << waittime.getCount() << endl;
    EV << "Await Queue AVG Length: " << queuelen.get(simTime()) << endl;
    EV << "Await Min WaitTime: " << waittime.getMin() << endl;
    EV << "Await Avg WaitTime: " << waittime.getMean() << endl;
    EV << "Await Max WaitTime: " << waittime.getMax() << endl;
    EV << "Standard deviation: " << waittime.getStddev() << endl;
    EV << "Setup Utilization:  " << proctime.getSum() / simTime() << endl;
    EV << "Setup Min ProcTime: " << proctime.getMin() << endl;
    EV << "Setup Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Setup Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation: " << proctime.getStddev() << endl;

    waittime.recordAs("Await WaitTime");
    proctime.recordAs("Setup ProcTime");
}
