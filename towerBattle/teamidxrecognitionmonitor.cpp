#include "teamidxrecognitionmonitor.h"

TeamIdxRecognitionMonitor::TeamIdxRecognitionMonitor(QObject *parent) : QObject(parent)
{

    m_isBusy.store(0);
}

void TeamIdxRecognitionMonitor::stop(){
    m_isBusy.store(0);
}


bool TeamIdxRecognitionMonitor::isBusy(){
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}


void TeamIdxRecognitionMonitor::on_start_teamIdxRecognitorMonitor(const TeamIdxRecognitor* recognitor){
    m_isBusy.store(1);

    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    double tickRate = dataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;

    while(isBusy()){
        int currentIdx = dataManager.getCurrentIndex();
        emit updateCurrentTeamIdx(currentIdx);

        QThread::msleep(sleepMs);
        continue;

    }

    m_isBusy.store(0);
    return;

}
