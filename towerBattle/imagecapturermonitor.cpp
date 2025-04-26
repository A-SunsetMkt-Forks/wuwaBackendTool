#include "imagecapturermonitor.h"

ImageCapturerMonitor::ImageCapturerMonitor(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}


void ImageCapturerMonitor::stop(){
    m_isBusy.store(0);
}


bool ImageCapturerMonitor::isBusy(){
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void ImageCapturerMonitor::on_start_monitor(const ImageCapturer* imageCapturer){
    if(imageCapturer == nullptr){
        m_isBusy.store(0);
        qCritical() << QString("无法监视游戏画面 指针为空");
        return;
    }

    m_isBusy.store(1);
    TowerBattleDataManager& towerBattleDataManager = TowerBattleDataManager::Instance();
    double tickRate = towerBattleDataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;
    while(isBusy()){
        cv::Mat getLastCapImg = towerBattleDataManager.getLastCapImg();
        emit updateGameMonitorStatus(imageCapturer->isBusy(), getLastCapImg);
        QThread::msleep(sleepMs);
    }


}

