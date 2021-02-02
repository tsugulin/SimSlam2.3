// �O�H�����烁�b�Z�[�W���󂯎��C�w�莞�ԁiomenetpp.ini��productionTime�ɂĎw��j�o�ߌ�Ɍ�H���ɑ���
// Resouce���W���[������̗v���ɂ�菈���̒��f/�ĊJ���s���D�������̃��b�Z�[�W��Await�m�[�h�̃L���[�ɕۊǂ���

#include "Goon.h"
Define_Module(Goon);
#include "Await.h"

void Goon::initialize()
{
    // �ϐ��̏�����
    onProduction = false;
    onInterruption = false;

    // ���\�[�X�̃|�C���^���擾
    cModule *mod = gate("in")->getPreviousGate()->getOwnerModule();
    setupNode = check_and_cast<Await *>(mod);
}

void Goon::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // ����������
        proctime.collect(simTime() - msg->getTimestamp());      // ���v���ɕۊ�
        send(msg, "out");                                       // ��H���ɑ���
        onProduction = false;                                   // ���Y�I��
    }
    else {
        // �O�H������͂������b�Z�[�W�̏ꍇ
        onProduction = true;                                // ���Y�J�n
        msg->setTimestamp(simTime());                       // ���[�h�^�C���̊J�n���Ԃ��Z�b�g
        onProduction = true;                                // ���YON
        expectedTime = simTime() + par("productionTime");   // �\�莞��
        curMsg = msg;
        scheduleAt(expectedTime, msg);                      // ���Y�J�n
    }
}

//�@�����𒆒f�D�c�莞�Ԃ��L�^����
void Goon::suspendProduction()
{
    Enter_Method("suspend");
    if ( onProduction ) {
        // ���Y���̏ꍇ
        onProduction = false;                       // ���Y��~
        onInterruption = true;                      // ���荞�ݒ�
        cancelEvent(curMsg);                        // ���Y���L�����Z��
        setupNode->push();                          // �҂��s��̐擪�Ƀ��b�Z�[�W��߂�
        suspendedTime = simTime();                  // ���f�J�n����
        remainingTime = expectedTime - simTime();   // �c�莞��
    }
}

//�@�������ĊJ�D�c��̏������s��
void Goon::resumeProduction()
{
    Enter_Method("resume");
    if ( onInterruption ) {
        // ���荞�ݒ��̏ꍇ
        onInterruption = false;                     // ���荞�݂�����
        onProduction = true;                        // ���Y�ĊJ�t���O
        setupNode->pop();                           // �҂��s�񂩂烁�b�Z�[�W�����o��
        expectedTime = simTime() + remainingTime;   // �\�莞��
        curMsg->setTimestamp(curMsg->getTimestamp() + simTime() - suspendedTime);  // �J�n���Ԃ�␳
        scheduleAt(expectedTime, curMsg);           // ���Y�ĊJ
    }
}

//�@�m�[�h�̓��v����\��
void Goon::finish()
{
    EV << "Process Utilization:  " << proctime.getSum() / simTime() << endl;
    EV << "Process Min ProcTime: " << proctime.getMin() << endl;
    EV << "Process Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Process Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation:   " << proctime.getStddev() << endl;

    proctime.recordAs("Process ProcTime");
}

