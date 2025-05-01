#include "resonanceskilljudger.h"

ResonanceSkillJudger::ResonanceSkillJudger(QObject *parent) : QObject(parent)
{

    m_isBusy.store(0);

}

void ResonanceSkillJudger::stop(){
    m_isBusy.store(0);
}

bool ResonanceSkillJudger::isBusy() const {
    if(m_isBusy.load() == 1){
        return true;
    }
    else{
        return false;
    }
}


void ResonanceSkillJudger::on_start_resonance_skill_recognition(){
    m_isBusy.store(1);

    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    // 获取当前配队
    QVector<TowerBattleDataManager::Charactor> team = dataManager.getCurrentTeamVec();
    double tickRate = dataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;

    // 获取此配队所有成员的美术资源
    QMap<TowerBattleDataManager::Charactor, QMap<QString, cv::Mat>> currentTeamRes;
    for(auto charactor : team){
        QMap<QString, cv::Mat> thisCharactorRes;
        thisCharactorRes = dataManager.getArtResByName(charactor);
        currentTeamRes[charactor] = thisCharactorRes;
    }

    while(isBusy()){
        int charactorIdx = dataManager.getCurrentIndex();
        TowerBattleDataManager::Charactor selectCharactor = team[charactorIdx];

        const cv::Rect resonanceSkillRoi = {1037, 619, 53, 53};
        // 每个角色的判断依据都不一样
        if(selectCharactor == TowerBattleDataManager::Charactor::UNDEFINED){
            // 无需判断 0号角色占位而已
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Sanhua){
            cv::Mat resonanceSkillMat;
            if(!currentTeamRes[selectCharactor].contains("skill")){
                qCritical() << QString("ERROR, Sanhua's resonance skill res is lost");
                QThread::msleep(sleepMs);
                continue;
            }

            resonanceSkillMat = currentTeamRes[selectCharactor]["skill"].clone();
            cv::Mat lastCapImg = dataManager.getLastCapImg();
            if(lastCapImg.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            SanHua(lastCapImg(resonanceSkillRoi).clone(), resonanceSkillMat);
            QThread::msleep(sleepMs);
            continue;
        }
        else{
            QThread::msleep(sleepMs);
            continue;
        }

    }


    m_isBusy.store(0);
    return;

}

void ResonanceSkillJudger::SanHua(const cv::Mat& resonanceSkillRoiMat, const cv::Mat& target){
    double sensitivity = 0.8;
    double simi;
    int x, y;
    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    if(!Utils::findPic(resonanceSkillRoiMat, target, sensitivity, x, y, simi)){
        // 无法找到回路
        dataManager.setResonanceSkillReady(simi);
        return;
    }
    else{
        dataManager.setResonanceSkillReady(simi);
        return;
    }
}
