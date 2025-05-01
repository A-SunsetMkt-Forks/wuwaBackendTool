#include "resonancecircuitjudger.h"

ResonanceCircuitJudger::ResonanceCircuitJudger(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);

}

void ResonanceCircuitJudger::stop(){
    m_isBusy.store(0);
}

bool ResonanceCircuitJudger::isBusy() const {
    if(m_isBusy.load() == 1){
        return true;
    }
    else{
        return false;
    }

}

void ResonanceCircuitJudger::on_start_resonance_recognition(){
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

        // 每个角色的判断依据都不一样
        if(selectCharactor == TowerBattleDataManager::Charactor::UNDEFINED){
            // 无需判断 0号角色占位而已
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Sanhua){
            cv::Mat resonanceCircuit, position;
            if(!currentTeamRes[selectCharactor].contains("resonanceCircuit")){
                qCritical() << QString("ERROR, Sanhua's resonanceCircuit res is lost");
                QThread::msleep(sleepMs);
                continue;
            }
            else{
                resonanceCircuit = currentTeamRes[selectCharactor]["resonanceCircuit"].clone();
            }

            if(!currentTeamRes[selectCharactor].contains("position")){
                qCritical() << QString("ERROR, Sanhua's position res is lost");
                QThread::msleep(sleepMs);
                continue;
            }
            else{
                position = currentTeamRes[selectCharactor]["position"].clone();
            }
            const cv::Rect sanHuaCircuitRoi = {529, 659, 209, 20};

            cv::Mat lastCapImg = dataManager.getLastCapImg();
            if(lastCapImg.empty()){
                QThread::msleep(sleepMs);
                continue;
            }
            // 找到回路存在的位置
            const cv::Mat roi = lastCapImg(sanHuaCircuitRoi);
            double sensitivity = 0.8;
            double simiCircuit, simiCursor;
            int circuitX, circuitY, cursorX, cursorY;
            if(!Utils::findPic(roi, resonanceCircuit, sensitivity, circuitX, circuitY, simiCircuit)){
                // 无法找到回路
                dataManager.setResonanceCircuit(0.0);
                QThread::msleep(sleepMs);
                continue;
            }

            if(!Utils::findPic(roi, position, 0.8, cursorX, cursorY, simiCursor)){
                // 无法找到光标 已经被掩盖起来了
                dataManager.setResonanceCircuit(1);
                QThread::msleep(sleepMs);
                continue;
            }
            else{
                dataManager.setResonanceCircuit(1-simiCursor);
                QThread::msleep(sleepMs);
                continue;
            }


        }
        else{
            QThread::msleep(sleepMs);
            continue;
        }


    }



}
