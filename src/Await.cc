// �O�H������̃��b�Z�[�W������Cresouce�u���b�N�̃��\�[�X���l�����ăZ�b�g�A�b�v���ԁiomenetpp.ini��productionTime�ɂĎw��j�o�ߌ�Ɍ�H���Ƀ��b�Z�[�W�𑗂�
// �������\�[�X���l���ł��Ȃ��ꍇ�͑҂��s��Ƀ��b�Z�[�W��ۊǂ��ă��\�[�X�̉����҂�
// Resouce���W���[������̗v���ɂ��Z�b�g�A�b�v�𒆒f/�ĊJ���s��
// Goon���W���[������̗v���ɂ��҂��s��ւ̃��b�Z�[�W�ۊ�/���o�����s��

#include "Await.h"
Define_Module(Await);
#include "Resource.h"

void Await::initialize()
{
    // �ϐ��̏�����
    onSetup = false;
    onInterruption = false;
    remainingTime = 0;
    dummy = new cMessage("dummy");

    // �L���[�̏�����
    queue.setName("queue");     // GUI�ɑ҂��s�񒷂���\�����邽�߂̖��O

    // �҂��s��̕��ϒ������v�Z����N���X��������
    queuelen.init(simTime(), MAX_QUEUE);

    // ���\�[�X�̃|�C���^���擾
    cModule *mod = gate("res")->getNextGate()->getOwnerModule();
    rsc = check_and_cast<Resource *>(mod);
}

void Await::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // ���g�̃��b�Z�[�W�ł���Ό�H���ɑ���
        proctime.collect(simTime() - msg->getTimestamp());      // ���v���ɕۊ�
        send(msg, "out");                                       // ��H���ɑ���
        onSetup = false;                                        // �Z�b�g�A�b�v�I��
    }
    else {
        // �O�H������͂������b�Z�[�W�̏ꍇ
        queuelen.set(simTime(), queue.getLength());     // �҂��s�񕽋ϒ������v�Z
        msg->setTimestamp(simTime());       // �L���[�ؗ��J�n���Ԃ��Z�b�g
        queue.insert(msg);                  // �҂��s��̍Ō���ɂ߃��b�Z�[�W��ǉ�
        startSetup();       // �҂��s��ɗ��܂��Ă��郁�b�Z�[�W����������
    }
}

// �݌ɂ�����΃Z�b�g�A�b�v���J�n
void Await::startSetup(void)
{
    Enter_Method("start");
    if ( !onSetup ) {
        if (queue.getLength() > 0) {
            cMessage *msg = check_and_cast<cMessage *>(queue.front());      // �҂��s��̐擪���b�Z�[�W�����o��
            if ( rsc->request(msg->getName(), msg->getId()) ) {             // TOOL���\�[�X��v��
                waittime.collect(simTime() - msg->getTimestamp());          // �L���[�ؗ����Ԃ̓��v����
                queuelen.set(simTime(), queue.getLength());                 // �҂��s�񕽋ϒ������v�Z
                queue.pop();                                                // �҂��s�񂩂烁�b�Z�[�W�����o��
                msg->setTimestamp(simTime());                               // ���[�h�^�C���̊J�n���Ԃ��Z�b�g
                curMsg = msg;                                               // ���f�����̂��ߌ����b�Z�[�W��ۊ�
                onSetup = true;                                             // �Z�b�g�A�b�vON
                expectedTime = simTime() + par("productionTime");           // �\�莞��
                scheduleAt(expectedTime, msg);                              // �Z�b�g�A�b�v�J�n
            }
        }
    }
}

// resouce�u���b�N�̗v���ɂ��Z�b�g�A�b�v�𒆒f�D�c�莞�Ԃ��L�^����
void Await::suspendSetup()
{
    Enter_Method("suspend");
    if ( onSetup ) {
        // �Z�b�g�A�b�v���̏ꍇ
        onSetup = false;                            // �Z�b�g�A�b�v��~
        onInterruption = true;                      // ���荞�ݒ�
        cancelEvent(curMsg);                        // �Z�b�g�A�b�v���L�����Z��
        queuelen.set(simTime(), queue.getLength()); // �҂��s�񕽋ϒ������v�Z
        if (queue.getLength() > 0) queue.insertBefore(queue.front(), dummy); else queue.insert(dummy);// �҂��s��̐擪�Ƀ��b�Z�[�W��߂�
        suspendedTime = simTime();                  // ���f�J�n����
        remainingTime = expectedTime - simTime();   // �c�莞��
    }
}

// resouce�u���b�N�̗v���ɂ��Z�b�g�A�b�v���ĊJ�D�c��̏������s��
void Await::resumeSetup()
{
    Enter_Method("resume");
    if ( onInterruption ) {
        // ���荞�ݒ��̏ꍇ
        onInterruption = false;                         // ���荞�݂�����
        onSetup = true;                                 // �Z�b�g�A�b�v�ĊJ�t���O
        queuelen.set(simTime(), queue.getLength());     // �҂��s�񕽋ϒ������v�Z
        queue.pop();                                    // �҂��s�񂩂烁�b�Z�[�W�����o��
        expectedTime = simTime() + remainingTime;       // �\�莞��
        curMsg->setTimestamp(curMsg->getTimestamp() + simTime() - suspendedTime);  // �J�n���Ԃ�␳
        scheduleAt(expectedTime, curMsg);               // �Z�b�g�A�b�v�ĊJ
    }
}

//�@Goon���W���[���̗v���ɂ��L���[�̐擪�Ƀ��b�Z�[�W���Z�b�g
void Await::push()
{
    Enter_Method("push");
    if ( !onSetup ) {
        // �Z�b�g�A�b�v���Ŗ������
        queuelen.set(simTime(), queue.getLength());     // �҂��s�񕽋ϒ������v�Z
        if (queue.getLength() > 0) queue.insertBefore(queue.front(), dummy); else queue.insert(dummy);// �҂��s��̐擪�Ƀ��b�Z�[�W��߂�
    }
}

//�@Goon���W���[���̗v���ɂ��L���[���烁�b�Z�[�W��߂�
void Await::pop()
{
    Enter_Method("pop");
    if ( !onSetup ) {
        // �Z�b�g�A�b�v���Ŗ������
        if (queue.getLength() > 0) {
            queuelen.set(simTime(), queue.getLength());     // �҂��s�񕽋ϒ������v�Z
            queue.pop();                                    // �҂��s�񂩂烁�b�Z�[�W�����o��
        }
    }
}

//�@�m�[�h�̓��v����\��
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
