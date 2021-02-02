// �O�H�����烁�b�Z�[�W���󂯎��ƃ��\�[�X���l�����C�w�莞�Ԃ̌o�ߌ�Ɍ�H���Ƀ��b�Z�[�W�𑗂�

#include "Preempt.h"
Define_Module(Preempt);
#include "Resource.h"

void Preempt::initialize()
{
    // ���\�[�X�̃|�C���^���擾
    cModule *mod = gate("res")->getNextGate()->getOwnerModule();
    rsc = check_and_cast<Resource *>(mod);
}

void Preempt::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // �Z���t���b�Z�[�W=���Y������
        simtime_t_cref d = simTime() - msg->getTimestamp();     // ��������
        proctime.collect(d);                                    // ���v���ɕۊ�
        send(msg, "out");                                       // ��H���ɑ���
    }
    else {
        // �O�H������͂������b�Z�[�W�̏ꍇ
        if ( rsc->request(msg->getName(), msg->getId()) ) {         // TOOL���\�[�X��v��
            msg->setTimestamp(simTime());                           // ���[�h�^�C���̊J�n���Ԃ��Z�b�g
            scheduleAt(simTime() + par("productionTime"), msg);     // ���Y�I�����SelfMessage�𑗂�
        }
    }
}

//�@�m�[�h�̓��v����\��
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
