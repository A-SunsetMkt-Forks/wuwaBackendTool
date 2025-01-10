#include "mainbackendworkernew.h"

MainBackendWorkerNew::MainBackendWorkerNew(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);
}

bool MainBackendWorkerNew::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void MainBackendWorkerNew::stopWorker(){
    m_isRunning.store(0);
    qInfo() << QString("MainBackendWorkerNew 线程结束");
}

void MainBackendWorkerNew::onStartLockEcho(const LockEchoSetting &lockEchoSetting){
    m_isRunning.store(1);

    qInfo() << QString("MainBackendWorkerNew::onStartLockEcho");

    // 初始化句柄
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        emit lockEchoDone(false, QString("未能初始化鸣潮窗口句柄"), lockEchoSetting);
        m_isRunning.store(0);
        return;
    }

    bool isWuwaRunning = Utils::isWuwaRunning();
    if(!isWuwaRunning){
        emit lockEchoDone(false, QString("未能初始化鸣潮窗口句柄"), lockEchoSetting);
        m_isRunning.store(0);
        return;
    }


    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 每一步都需要执行该检查 封装为匿名函数
    auto isLockEchoStop = [&](bool isAbort, bool isNormalEnd, const QString& msg, const LockEchoSetting &lockEchoSetting) {
        bool isWuwaRunning = Utils::isWuwaRunning();
        if(!isWuwaRunning){
            // 鸣潮已经被关闭 异常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit lockEchoDone(false, QString("鸣潮已经被关闭"), lockEchoSetting);
            m_isRunning.store(0);
            return true;
        }

        if(!isBusy()){
            // 用户中止 正常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit lockEchoDone(true, QString("用户停止"), lockEchoSetting);
            m_isRunning.store(0);
            return true;
        }
        else{
            if(isAbort){
                // 根据输入参数决定是否正常结束
                AttachThreadInput(currentThreadId, threadId, FALSE);
                emit lockEchoDone(isNormalEnd, msg, lockEchoSetting);
                m_isRunning.store(0);
                return true;
            }
            else{
                return false;
            }
        }
    };

    // 将线程附加到目标窗口
    if (AttachThreadInput(currentThreadId, threadId, TRUE)) {
        // 激活窗口
        SendMessage(Utils::hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
        Sleep(500);

        while(isBusy()){
            // 尝试进入背包
            if(!enterBagInterface()){
                if(isLockEchoStop(true, false, QString("未能找到背包图标"), lockEchoSetting)) return;
            }
            // 检查用户是否打断
            if(isLockEchoStop(false, true, QString("脚本运行结束"), lockEchoSetting)) return;

            // 尝试进入声骸页
            if(!enterEchoInterface()){
                if(isLockEchoStop(true, false, QString("未能找到声骸图标"), lockEchoSetting)) return;
            }
            // 检查用户是否打断
            if(isLockEchoStop(false, true, QString("脚本运行结束"), lockEchoSetting)) return;

            // 核心代码 处理当前页面的所有声骸
            if(!lockOnePageEcho()){
                if(isLockEchoStop(true, false, QString("锁定某页面声骸时出错"), lockEchoSetting)) return;
            }
            // 检查用户是否打断
            if(isLockEchoStop(false, true, QString("脚本运行结束"), lockEchoSetting)) return;


            break;

        }
    }

    if(isLockEchoStop(true, true, QString("脚本运行结束"), lockEchoSetting)) return;
    return;
}


bool MainBackendWorkerNew::enterBagInterface() {
    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    cv::Mat bagIcon = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;

    // 检测背包按钮
    bool isDetectBag = Utils::findPic(capImg(searchBagROI).clone(), bagIcon, 0.8, x, y);

    if (!isDetectBag) {
        // 最多尝试3次按 ESC
        const int maxTryEscFindBag = 3;
        for (int i = 0; i < maxTryEscFindBag && isBusy(); i++) {
            Utils::keyPress(Utils::hwnd, VK_ESCAPE, 1);
            for (int j = 0; j < 10 && isBusy(); j++) {
                Sleep(300); // 等待动画完成
            }

            if(!isBusy()){
                return true;
            }

            capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if (Utils::findPic(capImg(searchBagROI).clone(), bagIcon, 0.8, x, y)) {
                isDetectBag = true;
                break;
            }
            else{
                Utils::saveDebugImg(capImg, cv::Rect(), -1, -1, "没有找到背包");
            }
        }

        if(!isBusy()){
            return true;
        }

        // 如果尝试3次后仍未找到背包
        if (!isDetectBag) {
            return false;
        }
    }

    // 找到背包后点击
    cv::Rect bagRoiRect = cv::Rect(x + searchBagROI.x, y + searchBagROI.y, bagIcon.cols, bagIcon.rows); // 背包区域
    Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
    Sleep(500);
    Utils::clickWindowClientArea(Utils::hwnd, bagRoiRect.x + bagRoiRect.width / 2, bagRoiRect.y + bagRoiRect.height / 2);
    Sleep(500);
    Utils::saveDebugImg(capImg, bagRoiRect, bagRoiRect.x + bagRoiRect.width / 2, bagRoiRect.y + bagRoiRect.height / 2, "找到了背包");
    Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);

    return true; // 成功进入背包界面
}

bool MainBackendWorkerNew::enterEchoInterface(){
    // 等待背包打开动画
    for (int j = 0; j < 10 && isBusy(); j++) {
        Sleep(300); // 等待动画完成
    }

    if(!isBusy()){
        return true;
    }
    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    cv::Mat echoWhiteImg = cv::imread(QString("%1/bag_echo_white.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat echoYellowImg = cv::imread(QString("%1/bag_echo_yellow.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;

    if(Utils::findPic(capImg(searchEchoIconROI).clone(), echoYellowImg, 0.9, x, y)){
        cv::Rect echoIconRoi = cv::Rect(x + searchEchoIconROI.x, y + searchEchoIconROI.y, echoWhiteImg.cols, echoWhiteImg.rows);
        Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
        Sleep(500);
        Utils::clickWindowClientArea(Utils::hwnd, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2);
        Sleep(500);
        Utils::saveDebugImg(capImg, echoIconRoi, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2, "进入背包后找到了声骸图标 点击黄色图标");
        Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
        return true;
    }

    if(Utils::findPic(capImg(searchEchoIconROI).clone(), echoWhiteImg, 0.9, x, y)){
        cv::Rect echoIconRoi = cv::Rect(x + searchEchoIconROI.x, y + searchEchoIconROI.y, echoWhiteImg.cols, echoWhiteImg.rows);
        Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
        Sleep(500);
        Utils::clickWindowClientArea(Utils::hwnd, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2);
        Sleep(500);
        Utils::saveDebugImg(capImg, echoIconRoi, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2, "进入背包后找到了声骸图标 点击白色图标");
        Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
        return true;
    }
    else{
        qWarning() << QString("进入背包后没有找到声骸图标");
        Utils::saveDebugImg(capImg, cv::Rect(), -1, -1, "进入背包后没有找到声骸图标");
        return false;
    }

}

bool MainBackendWorkerNew::lockOnePageEcho(){
    // 等待声骸打开动画
    for (int j = 0; j < 5 && isBusy(); j++) {
        Sleep(300); // 等待动画完成
    }
    if(!isBusy()){
        return true;
    }

    // 先记录测试所有的框
    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    cv::Mat markImg = capImg.clone();
    for(int i = 0; i < maxColNum; i++){
        for(int j = 0; j < maxRowNum; j++){
            cv::Rect echoRect = cv::Rect(topLeftEchoROI.x + i * echoColMargin, \
                                         topLeftEchoROI.y + j * echoRowMargin, \
                                         topLeftEchoROI.width, \
                                         topLeftEchoROI.height);
            cv::rectangle(markImg, echoRect, cv::Scalar(255, 0, 0), 2);
            cv::Rect echoSetRect = cv::Rect(echoRect.x + echoSetRoi.x, \
                                            echoRect.y + echoSetRoi.y, \
                                            echoSetRoi.width, \
                                            echoSetRoi.height);
            cv::rectangle(markImg, echoSetRect, cv::Scalar(0, 0, 255), 2);

        }
    }
    Utils::saveDebugImg(markImg, cv::Rect(), -1, -1, "声骸格子");



    return true;
}
