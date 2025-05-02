#include "resonanceskilljudgemonitor.h"

ResonanceSkillJudgeMonitor::ResonanceSkillJudgeMonitor(QObject *parent) : QObject(parent)
{

    m_isBusy.store(0);

}

void ResonanceSkillJudgeMonitor::stop(){
    m_isBusy.store(0);
}

bool ResonanceSkillJudgeMonitor::isBusy() const {
    if(m_isBusy.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void ResonanceSkillJudgeMonitor::on_start_monitor(const ResonanceSkillJudger* judger){
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
            double val = dataManager.getResonanceSkillReady();
            emit updateResonanceSkillStatus(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Sanhua){
            double val = dataManager.getResonanceSkillReady();
            emit updateResonanceSkillStatus(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Camellya){
            double val = dataManager.getResonanceSkillReady();
            emit updateResonanceSkillStatus(val);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Shorekeeper){
            double val = dataManager.getResonanceSkillReady();
            emit updateResonanceSkillStatus(val);
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

