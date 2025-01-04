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
    qInfo() << QString("NoSwitchFightBackendWorker::startFight 不切人战斗开始");


}
