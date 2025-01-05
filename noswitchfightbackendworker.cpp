#include "noswitchfightbackendworker.h"

NoSwitchFightBackendWorker::NoSwitchFightBackendWorker(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);

}

bool NoSwitchFightBackendWorker::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void NoSwitchFightBackendWorker::stopWorker(){
    m_isRunning.store(0);
    qInfo() << QString("NoSwitchFightBackendWorker::startFight 不切人战斗结束");
}

void NoSwitchFightBackendWorker::startFight(){
    m_isRunning.store(1);
    qInfo() << QString("NoSwitchFightBackendWorker::startFight 不切人战斗开始");
    while(isBusy()){
        Utils::keyPress(Utils::hwnd, 'R', 1);
        Sleep(100);
        Utils::keyPress(Utils::hwnd, '1', 1);  // 切人
        Sleep(300);
        Utils::keyPress(Utils::hwnd, 'E', 1);
        Sleep(100);
        Utils::keyPress(Utils::hwnd, 'Q', 1);
        Sleep(100);

        for(int i = 0; i < 10 && isBusy();i++){
            Utils::clickWindowClient(Utils::hwnd);
            Sleep(100);
        }

        if(!isBusy()){
            break;
        }
    }

    return;
}
