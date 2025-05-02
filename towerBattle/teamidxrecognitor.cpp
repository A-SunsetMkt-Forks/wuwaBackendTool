#include "teamidxrecognitor.h"

TeamIdxRecognitor::TeamIdxRecognitor(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}

void TeamIdxRecognitor::stop(){
    m_isBusy.store(0);
}


bool TeamIdxRecognitor::isBusy(){
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void TeamIdxRecognitor::on_start_teamIdxRecognition(){
    //qInfo() << QString("TeamIdxRecognitor::on_start_teamIdxRecognition");

    m_isBusy.store(1);
    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    double tickRate = dataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;
    cv::Rect numPadRoiRec = dataManager.getNumPadRoi();
    QVector<cv::Mat> numPads = dataManager.getNumPads();

    while(isBusy()){
        cv::Mat lastCapImg = dataManager.getLastCapImg();
        if(lastCapImg.empty()){
            QThread::msleep(sleepMs);
            continue;
        }

        cv::Mat roiImg = lastCapImg(numPadRoiRec).clone();
        double numPad1_simi, numPad2_simi, numPad3_simi;
        int x, y;
        Utils::findPic(roiImg, numPads[1], 0.7, x, y, numPad1_simi);
        Utils::findPic(roiImg, numPads[2], 0.7, x, y, numPad2_simi);
        Utils::findPic(roiImg, numPads[3], 0.7, x, y, numPad3_simi);
        //qInfo() << QString("numPad1_simi %1, numPad2_simi %2, numPad3_simi %3").arg(numPad1_simi).arg(numPad2_simi).arg(numPad3_simi);

        //if(numPad1_simi*numPad1_simi + numPad2_simi*numPad2_simi + numPad3_simi*numPad3_simi <= 0.0){
        if(numPad1_simi < 0.7 && numPad2_simi < 0.7 && numPad3_simi < 0.7){
            dataManager.setCurrentIndex(0);
            QThread::msleep(sleepMs);
            continue;
        }


        if(numPad1_simi < numPad2_simi && numPad1_simi < numPad3_simi){
            //找不到1 就是选中了1
            dataManager.setCurrentIndex(1);

        }
        else if(numPad2_simi < numPad3_simi){
            //找不到2 就是选中了2
            dataManager.setCurrentIndex(2);
        }
        else{
            dataManager.setCurrentIndex(3);
        }

        QThread::msleep(sleepMs);
        continue;

    }

    m_isBusy.store(0);
    return;


}

