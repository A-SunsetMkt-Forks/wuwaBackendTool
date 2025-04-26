#include "imagecapturer.h"

ImageCapturer::ImageCapturer(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}

void ImageCapturer::stop(){
    m_isBusy.store(0);
}


bool ImageCapturer::isBusy() const {
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void ImageCapturer::on_start_capturer(){
    m_isBusy.store(1);
    TowerBattleDataManager& towerBattleDataManager = TowerBattleDataManager::Instance();
    double frameRate = towerBattleDataManager.getCapFrameRate();
    double sleepMs = 1000.0 / frameRate;
    bool isWuwaInit = Utils::initWuwaHwnd();
    if(!isWuwaInit){
        m_isBusy.store(0);
        qWarning() << QString("未能绑定鸣潮窗口句柄");
        return;
    }

    while(isBusy()){
        QImage image = Utils::captureWindowToQImage(Utils::hwnd);
        cv::Mat cvMat = Utils::qImage2CvMat(image);
        towerBattleDataManager.setLastCapImg(cvMat);

        QThread::msleep(sleepMs);
    }


}

