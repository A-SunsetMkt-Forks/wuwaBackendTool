#include "ultimatejudger.h"

UltimateJudger::UltimateJudger(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}


void UltimateJudger::stop(){
    m_isBusy.store(0);
}


bool UltimateJudger::isBusy() const {
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}


void UltimateJudger::on_start_ultimateJudge(){
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
        const cv::Rect ultimateRoi = {1173, 613, 68, 63};
        // 每个角色的判断依据都不一样
        if(selectCharactor == TowerBattleDataManager::Charactor::UNDEFINED){
            // 无需判断 0号角色占位而已
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Sanhua){
            cv::Mat ultimate;
            if(!currentTeamRes[selectCharactor].contains("ultimate")){
                qCritical() << QString("ERROR, Sanhua's ultimate res is lost");
                QThread::msleep(sleepMs);
                continue;
            }
            else{
                ultimate = currentTeamRes[selectCharactor]["ultimate"].clone();
            }

            cv::Mat lastCapImg = dataManager.getLastCapImg();
            if(lastCapImg.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            const cv::Mat ultimateRoiMat = lastCapImg(ultimateRoi).clone();
            double sensitivity = 0.8;
            double simiUltimate;
            int x, y;
            if(Utils::findPic(ultimateRoiMat, ultimate, sensitivity, x, y, simiUltimate)){
                // 找到大招图标
                dataManager.setResonanceLiberationReady(simiUltimate);
                QThread::msleep(sleepMs);
                continue;
            }
            else{
                dataManager.setResonanceLiberationReady(simiUltimate);
                QThread::msleep(sleepMs);
                continue;
            }


        }
        else{
            QThread::msleep(sleepMs);
            continue;
        }

    }

    m_isBusy.store(0);
    return;

}
