#include "wuwahwndwatcher.h"

WuwaHWNDWatcher::WuwaHWNDWatcher(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);
}

bool WuwaHWNDWatcher::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void WuwaHWNDWatcher::stopWorker(){
    m_isRunning.store(0);
    qInfo() << QString("WuwaHWNDWatcher 结束线程");
}

void WuwaHWNDWatcher::startWatcher(){
    m_isRunning.store(1);
    qInfo() << QString("WuwaHWNDWatcher 启动线程");

    while(isBusy()){
        // 鸣潮已经没了 结束
        if(!Utils::isWuwaRunning()){
            qWarning() << QString("检测到鸣潮已经关闭");
            emit stopMainBackendWorker();
            m_isRunning.store(0);
            return;
        }

        // 月卡判断
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        cv::Mat monthCard = cv::imread(QString("%1/月卡判断.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        qDebug() << QString("月卡判断 capImg type %1, size(%2, %3). monthCard type %4, size(%5, %6)")
                    .arg(QString::fromStdString(cv::typeToString( capImg.type()))).arg(capImg.cols).arg(capImg.rows)
                    .arg(QString::fromStdString(cv::typeToString( monthCard.type() ))).arg(monthCard.cols).arg(monthCard.rows);
        int x, y;
        bool isFindMonthCard = Utils::findPic(capImg, monthCard, 0.9, x, y);   // 479, 488, 632, 627
        if(isFindMonthCard){
            qInfo() << QString("领取月卡奖励");
            for(int i = 0; i < 5; i++){
                Sleep(200);
                Utils::clickWindowClientArea(Utils::hwnd, 627, 697);
            }
        }

        // 跳过BOSS领取奖励
        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        bool isFindGetBond = Utils::findPic(capImg, \
                                              cv::imread(QString("%1/领取奖励.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED), \
                                              0.7, x, y);   // 262, 201, 420, 280
        if(isFindGetBond){
            qInfo() << QString("领取boss奖励");
            for(int i = 0; i < 5; i++){
                Sleep(200);
                Utils::clickWindowClientArea(Utils::hwnd, 957, 234);
                Utils::clickWindowClientArea(Utils::hwnd, 957, 234);
                Sleep(200);
                Utils::clickWindowClientArea(Utils::hwnd, 1033, 143);
                Utils::clickWindowClientArea(Utils::hwnd, 1033, 143);
            }
        }

        qDebug() << QString("脚本持续运行......");
        Sleep(m_loopMsecs);
    }
}
