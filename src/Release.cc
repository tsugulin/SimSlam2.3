// �O�H�����烁�b�Z�[�W���󂯎��C���\�[�X��������Ďw�莞�ԁiomenetpp.ini��intervalTime�ɂĎw��j�o�ߌ�Ɍ�H���ɑ���

#include "Release.h"
Define_Module(Release);
#include "Resource.h"

void Release::initialize()
{
    // ���\�[�X�̃|�C���^���擾
    cModule *mod = gate("res")->getNextGate()->getOwnerModule();
    rsc = check_and_cast<Resource *>(mod);
}

void Release::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // �w�莞�Ԃ��o�߂�����
        proctime.collect(simTime() - msg->getTimestamp());      // ���v���ɕۊ�
        send(msg, "out");                                       // ��H���ɑ���
    }
    else {
        // �O�H������͂������b�Z�[�W�̏ꍇ
        msg->setTimestamp(simTime());                       // ���[�h�^�C���̊J�n���Ԃ��Z�b�g
        rsc->release( msg->getName(), msg->getId() );       // ���\�[�X�̊J��
        scheduleAt(simTime() + par("intervalTime"), msg);   // �I��������SelfMessage�𑗂�
    }
}

//�@�m�[�h�̓��v����\��
void Release::finish()
{
    EV << "Release Min ProcTime: " << proctime.getMin() << endl;
    EV << "Release Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Release Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation:   " << proctime.getStddev() << endl;

    proctime.recordAs("Release ProcTime");
}
