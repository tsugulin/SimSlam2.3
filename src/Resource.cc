// リソースの確保と解放を制御する

#include "Resource.h"
Define_Module(Resource);
#include "Await.h"
#include "Goon.h"
#include "Release.h"

void Resource::initialize()
{
    // 変数を初期化
    owner = 0;              // リソースの所有者をクリア
    interruption = 0;
    ownerType = "product";

    // AwaitノードとGoonノードのポインタを取得
    cModule *mod1 = gate("from_setup")->getPreviousGate()->getOwnerModule();
    setupNode = check_and_cast<Await *>(mod1);
    cModule *mod2 = gate("from_process")->getPreviousGate()->getOwnerModule();
    processNode = check_and_cast<Goon *>(mod2);
}

// リソースの獲得を行う
// リソースが未使用ならば所有者として要求者種類(productまたはaccident)とメッセージIDを登録してtrueを戻す
// リソースが使用中かつ所有者種別がproductかつ要求者種別がaccidentの場合にはリソースを横取りし，SetupノードとGoonノードに処理中断を要求する
// リソースを獲得できなかった場合はfalseを戻す

bool Resource::request(std::string name, long id)
{
    Enter_Method("request");
    bool ret_code = false;  // 戻り値を初期化
    if ( owner == 0 ) {
        // リソースが利用可能な場合
        owner = id;         // もしリソースが未使用であれば要求者が利用可能とする
        ownerType = name;   // productまたはaccident
        checkpoint = simTime();     // リソース確保時刻を記録
        ret_code = true;    // リソース獲得に成功
    }
    else {
        // リソースが使用中の場合
        if ( ownerType == "product" ) {
            if ( name == "accident" ) {
                // 割り込みでリソースを獲得する場合
                interruption = owner;   // 前オーナを保存
                owner = id;             // リソースを横取り
                ownerType = name;       // accident
                ret_code = true;        // リソース割り込みに成功
                setupNode->suspendSetup();          // セットアップを中断する
                processNode->suspendProduction();   // 生産を中断する
            }
        }
    }
    return ret_code;    // リソース獲得できればtrueを戻す
}

// リソースの解放を行う
// 要求者のメッセージIDが所有者と同じだが割込中でない場合は所有者の情報をクリアした後，Setupノードに次のセットアップを要求してtrueを戻す
// 要求者のメッセージIDが所有者と同じだが割込中の場合はリソースの所有者情報を元に戻してSetupノードとGoonノードに処理再開を要求した後にtrueを戻す
// リソースを解放できなかった場合はfalseを戻す

bool Resource::release(std::string name, long id)
{
    Enter_Method("release");
    bool ret_code = false;      // 戻り値を初期化
    if ( owner == id ) {
        // 所有者が解放する場合
        ret_code = true;            // リソース解放に成功
        if ( interruption == 0 ) {
            //　割り込み中でない場合
            owner = 0;                  // リソースを解放
            ownerType = "";
            proctime.collect(simTime() - checkpoint);     // 経過時間を記録
            setupNode->startSetup();    // 次のセットアップを開始する
        }
        else {
            // 割り込み中の場合
            owner = interruption;       // 前オーナーに戻す
            ownerType = "product";
            interruption = 0;
            setupNode->resumeSetup();           // 割込元のセットアップを再開する
            processNode->resumeProduction();    // 割込元の生産を再開する
        }
    }
    return ret_code;    // リソース解放できればtrueを戻す
}

//　ノードの統計情報を表示
void Resource::finish()
{
    EV << "Resouce Utilization: " << proctime.getSum() / simTime() << endl;
}
