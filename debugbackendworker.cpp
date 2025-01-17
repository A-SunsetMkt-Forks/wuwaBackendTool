#include "debugbackendworker.h"

DebugBackendWorker::DebugBackendWorker(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}

bool DebugBackendWorker::isBusy(){
    if(m_isBusy.load() == 0){
        return false;
    }
    else{
        return true;
    }
}

void DebugBackendWorker::stopWorker(){
    m_isBusy.store(0);
}


void DebugBackendWorker::onStartActivateCap(const QString& imgFilePath, const unsigned int& waitMs){
    qInfo() << QString("准备激活鸣潮并不断截图");

    // 初始化句柄
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        emit activateCapDone(false, QString("未能初始化鸣潮窗口句柄"));
        m_isBusy.store(0);
        return;
    }

    bool isWuwaRunning = Utils::isWuwaRunning();
    if(!isWuwaRunning){
        emit activateCapDone(false, QString("未能初始化鸣潮窗口句柄"));
        m_isBusy.store(0);
        return;
    }

    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 将线程附加到目标窗口
    if (AttachThreadInput(currentThreadId, threadId, TRUE)) {
        isAttached = true;
        // 激活窗口
        SendMessage(Utils::hwnd, WM_ACTIVATE, WA_ACTIVE, 0);

        while(isBusy()){
            // 不断截图保存
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if(capImg.empty()){
                qWarning() << QString("异常！获取的图像为空");
                continue;
            }

            bool isSave = cv::imwrite(imgFilePath.toLocal8Bit().toStdString(), capImg);
            if(!isSave){
                qWarning() << QString("异常！保存图像到 %1 失败").arg(imgFilePath);
                continue;
            }

            Sleep(waitMs);
        }
    }

    AttachThreadInput(currentThreadId, threadId, FALSE);
    isAttached = false;
    emit activateCapDone(true, QString("用户停止"));
    return;
}
