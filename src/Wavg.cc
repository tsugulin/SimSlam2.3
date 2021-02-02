#include "Wavg.h"

Wavg::Wavg()
{
    start = SIMTIME_ZERO;
    checkpoint = start;
    size = 0;
}

Wavg::~Wavg()
{
    if (size > 0) delete[] length;
}

void Wavg::init(simtime_t t, int n)
{
    if (n > 0) {
        start = t;                      // 開始時刻をセット
        checkpoint = start;             // 開始時刻をセット
        size = n;                       // 配列サイズ
        length = new double[size];      // キューの長さをセット
        for (int i = 0;  i <= n;  i++)  // 配列初期化
            length[i] = 0;
    } else {
        EV << "Memory Allocation Error!" << endl;
    }
}

void Wavg::set(simtime_t t, int n)
{
    simtime_t_cref d = t - checkpoint;  // 前回からの経過時間を算出
    checkpoint = t;                     // 経過時間をクリア
    length[n] += d.dbl();               // 平均待ち行列長さを保管
}

double Wavg::get(simtime_t t)
{
    double a = 0;                       // 加重平均値を初期化
    for (int i = 0;  i <= size;  i++)   // 各配列に重みを加算（＝分子）
        a += length[i] * i;
    simtime_t_cref d = t - start;       // 全体の時間を計算（＝分母）
    a = a / d.dbl();                    // 加重平均値を計算（分子÷分母）
    return(a);
}
