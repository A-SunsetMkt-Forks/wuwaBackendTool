#include "ultimatejudgemonitor.h"

UltimateJudgeMonitor::UltimateJudgeMonitor(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}

void UltimateJudgeMonitor::stop(){
    m_isBusy.store(0);
}


bool UltimateJudgeMonitor::isBusy() const {
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void UltimateJudgeMonitor::on_start_monitor(){
    m_isBusy.store(1);

    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    double tickRate = dataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;

    while(isBusy()){
        double isReady = dataManager.isResonanceLiberationReady();
        emit updateResonanceLiberationReady(isReady);

        QThread::msleep(sleepMs);
        continue;

    }
    m_isBusy.store(0);
    return;
}

