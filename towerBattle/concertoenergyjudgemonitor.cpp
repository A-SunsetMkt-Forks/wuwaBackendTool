#include "concertoenergyjudgemonitor.h"

ConcertoEnergyJudgeMonitor::ConcertoEnergyJudgeMonitor(QObject *parent) : QObject(parent)
{

    m_isBusy.store(0);
}


void ConcertoEnergyJudgeMonitor::stop(){
    m_isBusy.store(0);
}


bool ConcertoEnergyJudgeMonitor::isBusy() const {
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void ConcertoEnergyJudgeMonitor::on_start_monitor(const ConcertoEnergyJudger* judger){
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
            double val = dataManager.getConcertoEnergy();
            emit updateConcertoEnergy(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Sanhua){
            double val = dataManager.getConcertoEnergy();
            emit updateConcertoEnergy(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Camellya){
            double val = dataManager.getConcertoEnergy();
            emit updateConcertoEnergy(val);
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
