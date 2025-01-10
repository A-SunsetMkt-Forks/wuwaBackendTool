#include "mainbackendworkernew.h"

MainBackendWorkerNew::MainBackendWorkerNew(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);
    initEchoSetName2IconMap();
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

void MainBackendWorkerNew::initEchoSetName2IconMap(){
    echoSet2echoSetIconMap.clear();
    echoSetNameVector = {
        "freezingFrost", // 凝夜白霜  今州冰套
        "moltenRift",    // 熔山裂谷  今州火套
        "voidThunder",   // 彻空冥雷  今州雷套
        "sierraGale",    // 啸谷长风  今州风套
        "celestialLight", // 浮星祛暗  今州光套
        "sunSinkingEclipse", // 沉日劫明  今州暗套
        "rejuvenatingGlow", // 隐世回光 今州奶套
        "moonlitClouds", // 轻云出月 今州共鸣
        "lingeringTunes", // 不绝余音 今州攻击

        "frostyResolve", // 凌冽决断之心 黎纳汐塔冰套
        "eternalRadiance", // 此间永驻之光 黎纳汐塔光套
        "midnightVeil", // 幽夜隐匿之帷 黎纳汐塔暗套
        "empyreanAnthem", // 高天共奏之曲 黎纳汐塔共鸣协同攻击套
        "tidebreakingCourage" // 无惧浪涛之勇 黎纳汐塔共鸣效率增伤套
    };


    // 初始化翻译映射
    echoSetNameTranslationMap.insert("freezingFrost", "凝夜白霜");
    echoSetNameTranslationMap.insert("moltenRift", "熔山裂谷");
    echoSetNameTranslationMap.insert("voidThunder", "彻空冥雷");
    echoSetNameTranslationMap.insert("sierraGale", "啸谷长风");
    echoSetNameTranslationMap.insert("celestialLight", "浮星祛暗");
    echoSetNameTranslationMap.insert("sunSinkingEclipse", "沉日劫明");
    echoSetNameTranslationMap.insert("rejuvenatingGlow", "隐世回光");
    echoSetNameTranslationMap.insert("moonlitClouds", "轻云出月");
    echoSetNameTranslationMap.insert("lingeringTunes", "不绝余音");
    echoSetNameTranslationMap.insert("frostyResolve", "凌冽决断之心");
    echoSetNameTranslationMap.insert("eternalRadiance", "此间永驻之光");
    echoSetNameTranslationMap.insert("midnightVeil", "幽夜隐匿之帷");
    echoSetNameTranslationMap.insert("empyreanAnthem", "高天共奏之曲");
    echoSetNameTranslationMap.insert("tidebreakingCourage", "无惧浪涛之勇");

    // 遍历 QVector 加载图片
    for (const QString &key : echoSetNameVector) {
        cv::Mat icon = cv::imread(
            QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI(), key).toLocal8Bit().toStdString(),
            cv::IMREAD_UNCHANGED
        );

        // 加入到 QMap 中
        echoSet2echoSetIconMap[key] = icon;

        if(icon.empty()){
            qWarning() << QString("failed to load %1 bmp file, -> %2").arg(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI(), key)).arg(key);
        }
    }

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
            for(int iPage = 0; iPage < 10 && isBusy(); iPage++){
                if(!lockOnePageEcho()){
                    if(isLockEchoStop(true, false, QString("锁定%1页面声骸时出错").arg(iPage), lockEchoSetting)) return;
                }

                qInfo() << QString("翻页");
                for(int j = 0; j < 4 & isBusy(); j++){
                    //this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + echoRowMargin, topLeftEchoROI.x, topLeftEchoROI.y, 300, 15);
                    this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + echoRowMargin, topLeftEchoROI.x, topLeftEchoROI.y, echoRowMargin, 20);
                    Sleep(1000);
                    this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + 12, topLeftEchoROI.x, topLeftEchoROI.y, 12, 20);
                }
                // this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + 10, topLeftEchoROI.x, topLeftEchoROI.y, 10, 50);
                /*
                this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + 3*echoRowMargin, topLeftEchoROI.x, topLeftEchoROI.y, 3*echoRowMargin, 100);
                if(isLockEchoStop(false, true, QString("脚本运行结束"), lockEchoSetting)) return;
                Sleep(1000);

                this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + echoRowMargin+5, topLeftEchoROI.x, topLeftEchoROI.y, echoRowMargin, 100);
                if(isLockEchoStop(false, true, QString("脚本运行结束"), lockEchoSetting)) return;
                Sleep(1000);
                */
                // 截屏 和上一页对比 相似度 > 0.95认为已经结束了
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
        //Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
        Sleep(500);
        Utils::clickWindowClientArea(Utils::hwnd, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2);
        Sleep(500);
        Utils::saveDebugImg(capImg, echoIconRoi, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2, "进入背包后找到了声骸图标 点击黄色图标");
        //Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
        return true;
    }

    if(Utils::findPic(capImg(searchEchoIconROI).clone(), echoWhiteImg, 0.9, x, y)){
        cv::Rect echoIconRoi = cv::Rect(x + searchEchoIconROI.x, y + searchEchoIconROI.y, echoWhiteImg.cols, echoWhiteImg.rows);
        //Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
        Sleep(500);
        Utils::clickWindowClientArea(Utils::hwnd, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2);
        Sleep(500);
        Utils::saveDebugImg(capImg, echoIconRoi, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2, "进入背包后找到了声骸图标 点击白色图标");
        //Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
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

    QElapsedTimer onePageTimeCost;
    onePageTimeCost.start();
    capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    for (int i = 0; i < maxColNum; i++) {
        for (int j = 0; j < maxRowNum; j++) {
            int x, y;
            cv::Rect echoRect = cv::Rect(topLeftEchoROI.x + i * echoColMargin,
                                         topLeftEchoROI.y + j * echoRowMargin,
                                         topLeftEchoROI.width,
                                         topLeftEchoROI.height);
            cv::Rect echoSetRect = cv::Rect(echoRect.x + echoSetRoi.x,
                                            echoRect.y + echoSetRoi.y,
                                            echoSetRoi.width,
                                            echoSetRoi.height);

            // 存储相似度最大的套装
            double maxSimilarity = 0.0;
            QString bestEchoSetName;

            // 遍历判断是什么套装
            for (auto echoSetName : echoSet2echoSetIconMap.keys()) {
                double similarity;
                cv::Mat thisEchoRectRoiImg = capImg(echoSetRect).clone();
                if (Utils::findPic(thisEchoRectRoiImg, echoSet2echoSetIconMap[echoSetName], 0.6, x, y, similarity)) {
                    if (similarity > maxSimilarity) {
                        maxSimilarity = similarity;
                        bestEchoSetName = echoSetName;
                    }
                }
            }

            // 如果找到了相似度最大的套装
            if (!bestEchoSetName.isEmpty()) {
                qInfo() << QString("%1 行 %2 列 是 %3 (相似度: %4)")
                               .arg(i + 1)
                               .arg(j + 1)
                               .arg(echoSetNameTranslationMap[bestEchoSetName])
                               .arg(maxSimilarity);
                //markImg(cv::Rect(echoSetRect.x + 20, echoSetRect.y + 20, echoSet2echoSetIconMap[bestEchoSetName].cols, echoSet2echoSetIconMap[bestEchoSetName].rows))= echoSet2echoSetIconMap[bestEchoSetName];
                //cv::putText(markImg, bestEchoSetName.toLocal8Bit().toStdString(), cv::Point(x + echoSetRect.x, y + echoSetRect.y), cv::FONT_HERSHEY_SIMPLEX, 1.0,  cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
                if (!bestEchoSetName.isEmpty() && echoSet2echoSetIconMap.contains(bestEchoSetName)) {
                    cv::Mat icon = echoSet2echoSetIconMap[bestEchoSetName];
                    int startX = echoSetRect.x + 20;
                    int startY = echoSetRect.y + 20;

                    for (int row = 0; row < icon.rows; row++) {
                        for (int col = 0; col < icon.cols; col++) {
                            int targetX = startX + col;
                            int targetY = startY + row;

                            // 检查目标位置是否在 markImg 范围内
                            if (targetX >= 0 && targetX < markImg.cols && targetY >= 0 && targetY < markImg.rows) {
                                // 复制像素值
                                markImg.at<cv::Vec3b>(targetY, targetX) = icon.at<cv::Vec3b>(row, col);
                            }
                        }
                    }
                }
            }
            else{
                qWarning() << QString("%1 行 %2 列 是 %3 (相似度: %4)  未能找到合适的套装图标 ")
                              .arg(i + 1)
                              .arg(j + 1)
                              .arg(echoSetNameTranslationMap[bestEchoSetName])
                              .arg(maxSimilarity);
            }
        }
    }
    qInfo() << QString("一页判断耗时%1 ms").arg(onePageTimeCost.elapsed());
    Utils::saveDebugImg(markImg, cv::Rect(), -1, -1, "声骸格子");
    return true;
}

bool MainBackendWorkerNew::dragWindowClient3(HWND hwnd, int startx, int starty, int endx, int endy, int steps, int stepPauseMs) {
    if (hwnd == nullptr || !IsWindow(hwnd)) {
        qWarning() << "Invalid window handle.";
        return false;
    }

    // 将起始点转换为屏幕坐标
    POINT startPoint = { startx, starty };
    if (!ClientToScreen(hwnd, &startPoint)) {
        qWarning() << "Failed to convert start point to screen coordinates.";
        return false;
    }

    // 将终点转换为屏幕坐标
    POINT endPoint = { endx, endy };
    if (!ClientToScreen(hwnd, &endPoint)) {
        qWarning() << "Failed to convert end point to screen coordinates.";
        return false;
    }

    // 移动鼠标到起始点
    if (!SetCursorPos(startPoint.x, startPoint.y)) {
        qWarning() << "Failed to move mouse to start position.";
        return false;
    }
    Sleep(50); // 缓冲时间

    // 模拟按下鼠标左键
    LPARAM startLParam = MAKELPARAM(startx, starty);
    PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, startLParam);
    Sleep(200); // 模拟按住时间

    // 模拟拖拽过程
    for (int i = 1; i <= steps && isBusy(); ++i) {
        if(i < 3){
            //Sleep(500);
        }
        int intermediateX = startPoint.x + (endPoint.x - startPoint.x) * i / steps;
        int intermediateY = startPoint.y + (endPoint.y - startPoint.y) * i / steps;

        if (!SetCursorPos(intermediateX, intermediateY)) {
            qWarning() << "Failed to move mouse during drag.";
            return false;
        }

        LPARAM intermediateLParam = MAKELPARAM(startx + (endx - startx) * i / steps,
                                               starty + (endy - starty) * i / steps);
        PostMessage(hwnd, WM_MOUSEMOVE, MK_LBUTTON, intermediateLParam);
        Sleep(stepPauseMs); // 每步的延迟  默认50

        if(i == steps){
            Sleep(1000);
        }
    }
    if(!isBusy()){
        return true;
    }

    // 模拟松开鼠标左键
    LPARAM endLParam = MAKELPARAM(endx, endy);
    PostMessage(hwnd, WM_LBUTTONUP, 0, endLParam);
    Sleep(50); // 缓冲时间

    qDebug() << "Simulated drag from (" << startx << ", " << starty << ") to ("
             << endx << ", " << endy << ")";

    return true;
}
