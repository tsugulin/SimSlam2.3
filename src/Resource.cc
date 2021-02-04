// ���\�[�X�̊m�ۂƉ���𐧌䂷��

#include "Resource.h"
Define_Module(Resource);
#include "Await.h"
#include "Goon.h"
#include "Release.h"

void Resource::initialize()
{
    // �ϐ���������
    owner = 0;              // ���\�[�X�̏��L�҂��N���A
    interruption = 0;
    ownerType = "product";

    // Await�m�[�h��Goon�m�[�h�̃|�C���^���擾
    cModule *mod1 = gate("from_setup")->getPreviousGate()->getOwnerModule();
    setupNode = check_and_cast<Await *>(mod1);
    cModule *mod2 = gate("from_process")->getPreviousGate()->getOwnerModule();
    processNode = check_and_cast<Goon *>(mod2);
}

// ���\�[�X�̊l�����s��
// ���\�[�X�����g�p�Ȃ�Ώ��L�҂Ƃ��ėv���Ҏ��(product�܂���accident)�ƃ��b�Z�[�WID��o�^����true��߂�
// ���\�[�X���g�p�������L�Ҏ�ʂ�product���v���Ҏ�ʂ�accident�̏ꍇ�ɂ̓��\�[�X������肵�CSetup�m�[�h��Goon�m�[�h�ɏ������f��v������
// ���\�[�X���l���ł��Ȃ������ꍇ��false��߂�

bool Resource::request(std::string name, long id)
{
    Enter_Method("request");
    bool ret_code = false;  // �߂�l��������
    if ( owner == 0 ) {
        // ���\�[�X�����p�\�ȏꍇ
        owner = id;         // �������\�[�X�����g�p�ł���Ηv���҂����p�\�Ƃ���
        ownerType = name;   // product�܂���accident
        checkpoint = simTime();     // ���\�[�X�m�ێ������L�^
        ret_code = true;    // ���\�[�X�l���ɐ���
    }
    else {
        // ���\�[�X���g�p���̏ꍇ
        if ( ownerType == "product" ) {
            if ( name == "accident" ) {
                // ���荞�݂Ń��\�[�X���l������ꍇ
                interruption = owner;   // �O�I�[�i��ۑ�
                owner = id;             // ���\�[�X�������
                ownerType = name;       // accident
                ret_code = true;        // ���\�[�X���荞�݂ɐ���
                setupNode->suspendSetup();          // �Z�b�g�A�b�v�𒆒f����
                processNode->suspendProduction();   // ���Y�𒆒f����
            }
        }
    }
    return ret_code;    // ���\�[�X�l���ł����true��߂�
}

// ���\�[�X�̉�����s��
// �v���҂̃��b�Z�[�WID�����L�҂Ɠ��������������łȂ��ꍇ�͏��L�҂̏����N���A������CSetup�m�[�h�Ɏ��̃Z�b�g�A�b�v��v������true��߂�
// �v���҂̃��b�Z�[�WID�����L�҂Ɠ��������������̏ꍇ�̓��\�[�X�̏��L�ҏ������ɖ߂���Setup�m�[�h��Goon�m�[�h�ɏ����ĊJ��v���������true��߂�
// ���\�[�X������ł��Ȃ������ꍇ��false��߂�

bool Resource::release(std::string name, long id)
{
    Enter_Method("release");
    bool ret_code = false;      // �߂�l��������
    if ( owner == id ) {
        // ���L�҂��������ꍇ
        ret_code = true;            // ���\�[�X����ɐ���
        if ( interruption == 0 ) {
            //�@���荞�ݒ��łȂ��ꍇ
            owner = 0;                  // ���\�[�X�����
            ownerType = "";
            proctime.collect(simTime() - checkpoint);     // �o�ߎ��Ԃ��L�^
            setupNode->startSetup();    // ���̃Z�b�g�A�b�v���J�n����
        }
        else {
            // ���荞�ݒ��̏ꍇ
            owner = interruption;       // �O�I�[�i�[�ɖ߂�
            ownerType = "product";
            interruption = 0;
            setupNode->resumeSetup();           // �������̃Z�b�g�A�b�v���ĊJ����
            processNode->resumeProduction();    // �������̐��Y���ĊJ����
        }
    }
    return ret_code;    // ���\�[�X����ł����true��߂�
}

//�@�m�[�h�̓��v����\��
void Resource::finish()
{
    EV << "Resouce Utilization: " << proctime.getSum() / simTime() << endl;
}
