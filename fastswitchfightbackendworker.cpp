#include "fastswitchfightbackendworker.h"

FastSwitchFightBackendWorker::FastSwitchFightBackendWorker(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);

}

bool FastSwitchFightBackendWorker::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void FastSwitchFightBackendWorker::stopWorker(){
    m_isRunning.store(0);
    qInfo() << QString("FastSwitchFightBackendWorker::startFight 速切人战斗结束");
}

void FastSwitchFightBackendWorker::startFight(){
    qInfo() << QString("FastSwitchFightBackendWorker::startFight 速切人战斗开始");
}
