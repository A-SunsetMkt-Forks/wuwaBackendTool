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
            dataManager.setResonanceCircuit(0);
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
        else if(selectCharactor == TowerBattleDataManager::Charactor::Camellya){
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Shorekeeper){
            cv::Mat resonanceCircuit, resonanceCircuitGray, r00, r02, r04, r06, r08, r10;
            resonanceCircuit = currentTeamRes[selectCharactor]["resonanceCircuit2"].clone();   // 有效的回路
            resonanceCircuitGray = currentTeamRes[selectCharactor]["resonanceCircuit3"].clone();
            r00 = currentTeamRes[selectCharactor]["r00"].clone();
            r02 = currentTeamRes[selectCharactor]["r02"].clone();
            r04 = currentTeamRes[selectCharactor]["r04"].clone();
            r06 = currentTeamRes[selectCharactor]["r06"].clone();
            r08 = currentTeamRes[selectCharactor]["r08"].clone();
            r10 = currentTeamRes[selectCharactor]["r10"].clone();

            cv::Mat lastCapImg = dataManager.getLastCapImg();
            if(lastCapImg.empty()){
                QThread::msleep(sleepMs);
                continue;
            }


            const QVector<cv::Rect> ShorekeeperRoiVec = {{542,666,31,5}, {580, 666, 31, 5}, {618,666,31,5}, {657,666,31,5}, {695,666,31,5}};
            int x, y;
            QVector<double> simiSeries(ShorekeeperRoiVec.size(), 0.0);
            QVector<double> simiGraySeries(ShorekeeperRoiVec.size(), 0.0);
            double thres = 0.8;
            int finalIdx = -1;
            for(int i = 0; i < simiSeries.size(); i++){
                cv::Mat circuitRoiMat = lastCapImg(ShorekeeperRoiVec[i]).clone();
                Utils::findPic(circuitRoiMat, resonanceCircuit, thres, x, y, simiSeries[i]);
                Utils::findPic(circuitRoiMat, resonanceCircuitGray, thres, x, y, simiGraySeries[i]);
                if(simiSeries[i] < simiGraySeries[i]){
                    finalIdx = i;
                    break;
                }
            }

            // finalIdx-1 说明始终都是亮的
            dataManager.setResonanceCircuit(finalIdx < 0 ? 1.0 : (finalIdx - 1)*0.2);
            QThread::msleep(sleepMs);
            continue;

            /*

            // 守共有5段回路
            const cv::Rect ShorekeeperRoi = {540, 664, 205, 10};
            cv::Mat wholeCircuitRoiMat = lastCapImg(ShorekeeperRoi).clone();
            int x, y;
            QVector<double> simiSeries(6, 0.0);
            double thres = 0.99;
            // const QVector<cv::Rect> ShorekeeperRoiVec = {{543,664,30,7}, {581,664,30,7}, {619,664,30,7}, {658,664,30,7}, {716,664,30,7}};
            if(Utils::findPic(wholeCircuitRoiMat, r10, thres, x, y, simiSeries[0])){
                // 完整回路
                dataManager.setResonanceCircuit(1.0);
                QThread::msleep(sleepMs);
                continue;
            }
            else if(Utils::findPic(wholeCircuitRoiMat, r08, thres, x, y, simiSeries[1])){
                dataManager.setResonanceCircuit(0.8);
                QThread::msleep(sleepMs);
                continue;
            }
            else if(Utils::findPic(wholeCircuitRoiMat, r06, thres, x, y, simiSeries[2])){
                dataManager.setResonanceCircuit(0.6);
                QThread::msleep(sleepMs);
                continue;
            }
            else if(Utils::findPic(wholeCircuitRoiMat, r04, thres, x, y, simiSeries[3])){
                dataManager.setResonanceCircuit(0.4);
                QThread::msleep(sleepMs);
                continue;
            }
            else if(Utils::findPic(wholeCircuitRoiMat, r02, thres, x, y, simiSeries[4])){
                dataManager.setResonanceCircuit(0.2);
                QThread::msleep(sleepMs);
                continue;
            }
            else if(Utils::findPic(wholeCircuitRoiMat, r00, thres, x, y, simiSeries[5])){
                dataManager.setResonanceCircuit(0.0);
                QThread::msleep(sleepMs);
                continue;
            }
            else{
                auto maxIt = std::min_element(simiSeries.begin(), simiSeries.end());
                // 计算索引
                int idx = std::distance(simiSeries.begin(), maxIt);
                double circuitRatio = 1.0 - idx*0.2;

                // 找出最像的
                dataManager.setResonanceCircuit(circuitRatio);
                QThread::msleep(sleepMs);
                continue;
            }
            */

        }
        else{
            QThread::msleep(sleepMs);
            continue;
        }


    }



}
