#include "resonancecircuitjudgemonitor.h"

ResonanceCircuitJudgeMonitor::ResonanceCircuitJudgeMonitor(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}

void ResonanceCircuitJudgeMonitor::stop(){
    m_isBusy.store(0);
}

bool ResonanceCircuitJudgeMonitor::isBusy() {
    if(m_isBusy.load() == 1){
        return true;
    }
    else{
        return false;
    }

}


void ResonanceCircuitJudgeMonitor::on_start_monitor(const ResonanceCircuitJudger* judger){
    m_isBusy.store(1);

    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    double tickRate = dataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;
    QVector<TowerBattleDataManager::Charactor> team = dataManager.getCurrentTeamVec();

    while(isBusy()){
        int charactorIdx = dataManager.getCurrentIndex();
        TowerBattleDataManager::Charactor selectCharactor = team[charactorIdx];

        // 每个角色的判断依据都不一样
        if(selectCharactor == TowerBattleDataManager::Charactor::UNDEFINED){
            // 无需判断 0号角色占位而已
            double val = dataManager.getResonanceCircuit();
            emit updateResonanceCircuitStatus(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Sanhua){
            double val = dataManager.getResonanceCircuit();
            emit updateResonanceCircuitStatus(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Shorekeeper){
            double val = dataManager.getResonanceCircuit();
            emit updateResonanceCircuitStatus(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else{
            QThread::msleep(sleepMs);
            continue;
        }


        QThread::msleep(sleepMs);
    }


}
