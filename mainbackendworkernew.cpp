#include "mainbackendworkernew.h"

MainBackendWorkerNew::MainBackendWorkerNew(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);
    initEchoSetName2IconMap();
    initEntryName2IconMap();
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


void MainBackendWorkerNew::initEntryName2IconMap(){
    entryName2entryIconMap.clear();
    entryNameVector = {
        "havocDMG", "fusionDMG", "HPratio", "energyRegen", "aeroDMG",
        "electroDMG", "defenceRatio", "attackRatio", "spectroDMG", "heal",
        "criticalDMG", "criticalRatio", "glacioDMG"
    };

    // 初始化翻译映射
    entryNameTranslationMap.insert("havocDMG", "湮灭伤害");
    entryNameTranslationMap.insert("fusionDMG", "热熔伤害");
    entryNameTranslationMap.insert("HPratio", "生命百分比");
    entryNameTranslationMap.insert("energyRegen", "共鸣效率");
    entryNameTranslationMap.insert("aeroDMG", "气动伤害");
    entryNameTranslationMap.insert("electroDMG", "导电伤害");
    entryNameTranslationMap.insert("defenceRatio", "防御百分比");
    entryNameTranslationMap.insert("attackRatio", "攻击百分比");
    entryNameTranslationMap.insert("spectroDMG", "衍射伤害");
    entryNameTranslationMap.insert("heal", "治疗效果加成");
    entryNameTranslationMap.insert("criticalDMG", "暴击伤害");
    entryNameTranslationMap.insert("criticalRatio", "暴击率");
    entryNameTranslationMap.insert("glacioDMG", "冷凝伤害");

    // 遍历 QVector 加载图片
    for (const QString &key : entryNameVector) {
        cv::Mat icon = cv::imread(
            QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI(), key).toLocal8Bit().toStdString(),
            cv::IMREAD_UNCHANGED
        );

        // 加入到 QMap 中
        entryName2entryIconMap[key] = icon;

        if (icon.empty()) {
            QString filePath = QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg(key);
            qWarning() << QString("Failed to load %1 bmp file, -> %2").arg(filePath).arg(key);
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
                if(!lockOnePageEcho(lockEchoSetting)){
                    if(isLockEchoStop(true, false, QString("锁定%1页面声骸时出错").arg(iPage), lockEchoSetting)) return;
                }

                qInfo() << QString("翻页");
                for(int j = 0; j < 4 & isBusy(); j++){
                    this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + echoRowMargin, topLeftEchoROI.x, topLeftEchoROI.y, echoRowMargin, 20);
                    //this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + echoRowMargin, topLeftEchoROI.x, topLeftEchoROI.y, 20, 20);
                    Sleep(1000);
                    this->dragWindowClient3(Utils::hwnd, topLeftEchoROI.x, topLeftEchoROI.y + 12, topLeftEchoROI.x, topLeftEchoROI.y, 12, 20);
                }
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

void MainBackendWorkerNew::onStartNormalBoss(const NormalBossSetting &normalBossSetting){
    m_isRunning.store(1);

    qInfo() << QString("MainBackendWorkerNew::onStartNormalBoss");

    // 初始化句柄
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        emit normalBossDone(false, QString("未能初始化鸣潮窗口句柄"), normalBossSetting);
        m_isRunning.store(0);
        return;
    }

    bool isWuwaRunning = Utils::isWuwaRunning();
    if(!isWuwaRunning){
        emit normalBossDone(false, QString("未能初始化鸣潮窗口句柄"), normalBossSetting);
        m_isRunning.store(0);
        return;
    }

    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 每一步都需要执行该检查 封装为匿名函数
    auto isNormalBossStop = [&](bool isAbort, bool isNormalEnd, const QString& msg, const NormalBossSetting &normalBossSetting) {
        bool isWuwaRunning = Utils::isWuwaRunning();
        if(!isWuwaRunning){
            // 鸣潮已经被关闭 异常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit normalBossDone(false, QString("鸣潮已经被关闭"), normalBossSetting);
            m_isRunning.store(0);
            return true;
        }

        if(!isBusy()){
            // 用户中止 正常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit normalBossDone(true, QString("用户停止"), normalBossSetting);
            m_isRunning.store(0);
            return true;
        }
        else{
            if(isAbort){
                // 根据输入参数决定是否正常结束
                AttachThreadInput(currentThreadId, threadId, FALSE);
                emit normalBossDone(isNormalEnd, msg, normalBossSetting);
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
        pickUpNormalBossEcho = 0;  // 初始拾取声骸计数为 0
        QString errMsg;
        while(isBusy()){
            QList<NormalBossEnum> allBossList = normalBossSetting.bossSettings.keys();
            for(NormalBossEnum thisBoss : allBossList){
                if(normalBossSetting.isBossEnabled(thisBoss)){
                    // 回到主界面 通过索拉指南 残像探寻 进入boss
                    bool isPrepared = normalBossPreperation(normalBossSetting, errMsg);
                    if(!isPrepared){
                        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), 0, 0,errMsg);
                        if(isNormalBossStop(true, false, errMsg, normalBossSetting)) return;
                    }

                    if(!isBusy()){
                        if(isNormalBossStop(true, false, QString("用户中止"), normalBossSetting)) return;
                    }

                    // 要刷这个boss的
                    QString oneBossErr;
                    bool oneBossLoopDone = oneBossLoop(normalBossSetting, thisBoss, oneBossErr);
                    if(!oneBossLoopDone){
                        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), 0, 0, oneBossErr);
                        qWarning() << QString("刷 %1 出现问题 : %2").arg(normalBossSetting.Enum2QString(thisBoss)).arg(oneBossErr);
                    }

                    if(!isBusy()){
                        if(isNormalBossStop(true, false, QString("用户中止"), normalBossSetting)) return;
                    }
                }
            }
        }
    }
    else{
        if(isNormalBossStop(true, false, QString("未能将线程附加到鸣潮窗口"), normalBossSetting)) return;
    }

    if(isNormalBossStop(true, true, QString("脚本运行结束"), normalBossSetting)) return;
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

bool MainBackendWorkerNew::normalBossPreperation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    qInfo() << QString("normalBossPreperation");

    // 进入主界面
    if(backToMain() == false){
        errMsg = "连续4次 ESC 未能找到背包按钮";
        return false;
    }
    qInfo() << QString("成功进入主界面 开始找索拉指南");

    // 检查用户是否打断
    if(!isBusy()) return true;

    // 进入索拉指南
    if(enterSolaGuide() == false){
        errMsg = "未能在esc菜单内找到索拉指南";
        return false;
    }
    qInfo() << QString("成功进入索拉指南，开始找残像探寻");

    // 检查用户是否打断
    if(!isBusy()) return true;

    // 进入残像探寻
    if(enterEchoList() == false){
        errMsg = "未能在索拉指南找到残像探寻";
        return false;
    }
    qInfo() << QString("成功进入残像探寻 预备工作结束");

    return true;
}

bool MainBackendWorkerNew::backToMain(){
    qInfo() << QString("尝试恢复到主界面");

    cv::Mat missionImg = cv::imread(QString("%1/mission.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isFindBagFinal = false;  // 是否找到背包图片
    for(int i = 0; i < 4; i++){
        double similarity;
        int x, y, timeCostMs;
        bool isFind = loopFindPic(missionImg, 0.8, defaultMaxWaitMs, 250, "未能找到背包按钮 尝试esc后 继续寻找", similarity, x, y, timeCostMs);
        if(!isFind){
            Utils::keyPress(Utils::hwnd, VK_ESCAPE, 1);
        }
        else{
            isFindBagFinal = true;
            break;
        }
    }

    return isFindBagFinal;
}

bool MainBackendWorkerNew::enterSolaGuide(){
    qInfo() << QString("尝试进入到索拉指南");

    Utils::keyPress(Utils::hwnd, VK_ESCAPE, 1);
    cv::Mat solaGuideImg = cv::imread(QString("%1/solaGuideInEsc.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    double similarity;
    int x, y, timeCostMs;
    bool isFind = loopFindPic(solaGuideImg, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能在esc菜单内找到索拉指南", similarity, x, y, timeCostMs);
    Sleep(250);  //找到了图标 此时游戏不一定响应 所以延迟点击

    if(isFind) Utils::clickWindowClientArea(Utils::hwnd, x + solaGuideImg.cols / 2, y + solaGuideImg.rows / 2);

    return isFind;
}

bool MainBackendWorkerNew::enterEchoList(){
    qInfo() << QString("尝试进入到残像探寻");
    cv::Mat echoListUncheckedImg = cv::imread(QString("%1/echoListUnchecked.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    double similarity;
    int x, y, timeCostMs;
    bool isFind = loopFindPic(echoListUncheckedImg, 0.7, defaultMaxWaitMs, defaultRefreshMs, "未能在索拉指南找到残像探寻", similarity, x, y, timeCostMs);

    if(isFind) {
        cv::Mat echoListChecked = cv::imread(QString("%1/echoListChecked.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        cv::Mat echoBossListDefault = cv::imread(QString("%1/echoBossListDefault.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        // 至多再点击4次 进不去就说明有问题了
        for(int i = 0; i < 4 && isBusy(); i++){
            Utils::moveMouseToClientArea(Utils::hwnd, x + echoListUncheckedImg.cols / 2, y + echoListUncheckedImg.rows / 2);
            Utils::clickWindowClientArea(Utils::hwnd, x + echoListUncheckedImg.cols / 2, y + echoListUncheckedImg.rows / 2);

            int echoListx, echoListy;
            bool isTrueClicked = loopFindPic(echoBossListDefault, 0.8, defaultMaxWaitMs, defaultRefreshMs, "找到了残像探寻图标 但点击没反应", similarity, echoListx, echoListy, timeCostMs);
            if(isTrueClicked){
                return true;
            }

            Sleep(250);
        }
        if(!isBusy()){
            return true;  // 用户打断
        }
        return false;  // 点了两次残像探寻都进不去
    }
    else{
        return false;
    }
}

bool MainBackendWorkerNew::oneBossLoop(const NormalBossSetting &normalBossSetting, const NormalBossEnum& bossName, QString& errMsg){
    qInfo() << QString("干boss %1").arg(NormalBossSetting::Enum2QString(bossName));

    // 不同boss的前置准备工作不同
    switch (bossName) {
    case NormalBossEnum::Crownless:
        if(!crownLessPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::DragonOfDirge:
        if(!dragonOfDirgePreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::SentryConstruct:
        if(!sentryConstructPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    default:
        qWarning() << QString("未针对boss %1 做战斗适配").arg(NormalBossSetting::Enum2QString(bossName));
        return false;
    }

    if(!isBusy()){
        return true;
    }

    // 保证已经锁定了boss
    // 战斗代码


    // 拾取声骸  记得计数+1

    return true;
}

bool MainBackendWorkerNew::dragonOfDirgePreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    /*
    // 现在是在残像探寻列表
    // 移动到boss列表
    Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListPos.x, scrollEchoListPos.y);

    QElapsedTimer timer;
    timer.start();
    // 匹配、滚动；最多允许时间 ?S 必须能在残像探寻找到叹息古龙icon 否则认为失败
    int maxWaitMs = 10 * 1000;
    cv::Mat dragonOfDirgeIcon = cv::imread(QString("%1/dragonOfDirge.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;
    double similarity;
    bool findDragonOfDirgeIcon = false;
    while(timer.elapsed() < maxWaitMs && isBusy()){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        findDragonOfDirgeIcon = Utils::findPic(capImg, dragonOfDirgeIcon, 0.8, x, y, similarity);
        if(!findDragonOfDirgeIcon){
            this->dragWindowClient3(Utils::hwnd, scrollEchoListsStartPos.x, scrollEchoListsStartPos.y, \
                                    scrollEchoListsEndPos.x, scrollEchoListsEndPos.y, \
                                    20, 20);
            Sleep(500);
            Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListsWaitPos.x, scrollEchoListsWaitPos.y);
        }
        else{
            break;  //找到了icon
        }
    }
    // 用户打断 正常返回
    if(!isBusy()){
        return true;
    }

    if(!findDragonOfDirgeIcon){
        // 超时 异常
        errMsg = QString("超时, %1 ms后仍未在残像探寻列表找到 %2 的icon").arg(maxWaitMs).arg("叹息古龙");
        qWarning() << errMsg;
        return false;
    }

    // 点击找到的icon
    Utils::clickWindowClientArea(Utils::hwnd, x + dragonOfDirgeIcon.cols/2, y + dragonOfDirgeIcon.rows/2);
    cv::Mat confirm = cv::imread(QString("%1/dragonOfDirgeConfirm.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int timeCostMs = 0;
    if(!loopFindPic(confirm, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到叹息之龙确认信息", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未确认点击 %2 的icon").arg(defaultMaxWaitMs).arg("叹息古龙");
        qWarning() << errMsg;
        return false;
    }

    // 尝试点击探测
    cv::Mat detect = cv::imread(QString("%1/detect.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(detect, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到叹息古龙的探测", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍找到 %2 的探测").arg(defaultMaxWaitMs).arg("叹息古龙");
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + detect.cols/2, y + detect.rows/2);

    // 尝试点击快速旅行
    cv::Mat fastTravel = cv::imread(QString("%1/fastTravel.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(fastTravel, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到叹息之龙的快速旅行", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未找到 %2 的快速旅行").arg(defaultMaxWaitMs).arg("叹息古龙");
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + fastTravel.cols/2, y + fastTravel.rows/2);

    // 不断找背包 找到说明加载完毕  这里加载可能时间稍长 允许10s
    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(bagImg, 0.8, 10000, defaultRefreshMs, "传送叹息之龙 加载超时", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未完成 %2 的传送游戏加载").arg(10000).arg("叹息古龙");
        qWarning() << errMsg;
        return false;
    }
    */

    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "dragonOfDirge", "叹息之龙", errMsg);

    // 叹息之龙相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();

    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("dragonOfDirgeTitle.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        int x, y;
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("已经锁定 %1").arg("叹息之龙");
            Sleep(500);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        return true;
    }

    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
    return isTraced;
}


bool MainBackendWorkerNew::crownLessPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    /*
    // 现在是在残像探寻列表
    // 移动到boss列表
    Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListPos.x, scrollEchoListPos.y);

    QElapsedTimer timer;
    timer.start();
    // 匹配、滚动；最多允许时间 ?S 必须能在残像探寻找到无冠者icon 否则认为失败
    int maxWaitMs = 10 * 1000;
    cv::Mat crownLessIcon = cv::imread(QString("%1/crownLess.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;
    double similarity;
    bool findIcon = false;
    while(timer.elapsed() < maxWaitMs && isBusy()){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        findIcon = Utils::findPic(capImg, crownLessIcon, 0.8, x, y, similarity);
        if(!findIcon){
            this->dragWindowClient3(Utils::hwnd, scrollEchoListsStartPos.x, scrollEchoListsStartPos.y, \
                                    scrollEchoListsEndPos.x, scrollEchoListsEndPos.y, \
                                    20, 20);
            Sleep(500);
            Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListsWaitPos.x, scrollEchoListsWaitPos.y);
        }
        else{
            break;  //找到了icon
        }
    }
    // 用户打断 正常返回
    if(!isBusy()){
        return true;
    }

    if(!findIcon){
        // 超时 异常
        errMsg = QString("超时, %1 ms后仍未在残像探寻列表找到 %2 的icon").arg(maxWaitMs).arg("无冠者");
        qWarning() << errMsg;
        return false;
    }

    // 点击找到的icon
    Utils::clickWindowClientArea(Utils::hwnd, x + crownLessIcon.cols/2, y + crownLessIcon.rows/2);
    cv::Mat confirm = cv::imread(QString("%1/crownlessConfirm.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int timeCostMs = 0;
    if(!loopFindPic(confirm, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到无冠者确认信息", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未确认点击 %2 的icon").arg(defaultMaxWaitMs).arg("无冠者");
        qWarning() << errMsg;
        return false;
    }

    // 尝试点击探测
    cv::Mat detect = cv::imread(QString("%1/detect.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(detect, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到无冠者的探测", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍找到 %2 的探测").arg(defaultMaxWaitMs).arg("无冠者");
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + detect.cols/2, y + detect.rows/2);

    // 尝试点击快速旅行
    cv::Mat fastTravel = cv::imread(QString("%1/fastTravel.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(fastTravel, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到无冠者的快速旅行", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未找到 %2 的快速旅行").arg(defaultMaxWaitMs).arg("无冠者");
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + fastTravel.cols/2, y + fastTravel.rows/2);

    // 不断找背包 找到说明加载完毕  这里加载可能时间稍长 允许10s
    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(bagImg, 0.8, 10000, defaultRefreshMs, "传送无冠者加载超时", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未完成 %2 的传送游戏加载").arg(10000).arg("无冠者");
        qWarning() << errMsg;
        return false;
    }

    return true;
    */

    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "crownless", "无冠者", errMsg);

    return isGeneralPrepared;
}

bool MainBackendWorkerNew::sentryConstructPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    /*
    // 现在是在残像探寻列表
    // 移动到boss列表
    Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListPos.x, scrollEchoListPos.y);

    QElapsedTimer timer;
    timer.start();
    // 匹配、滚动；最多允许时间 ?S 必须能在残像探寻找到异构武装icon 否则认为失败
    int maxWaitMs = 10 * 1000;
    cv::Mat crownLessIcon = cv::imread(QString("%1/sentryConstruct.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;
    double similarity;
    bool findIcon = false;
    while(timer.elapsed() < maxWaitMs && isBusy()){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        findIcon = Utils::findPic(capImg, crownLessIcon, 0.8, x, y, similarity);
        if(!findIcon){
            this->dragWindowClient3(Utils::hwnd, scrollEchoListsStartPos.x, scrollEchoListsStartPos.y, \
                                    scrollEchoListsEndPos.x, scrollEchoListsEndPos.y, \
                                    20, 20);
            Sleep(500);
            Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListsWaitPos.x, scrollEchoListsWaitPos.y);
        }
        else{
            break;  //找到了icon
        }
    }
    // 用户打断 正常返回
    if(!isBusy()){
        return true;
    }

    if(!findIcon){
        // 超时 异常
        errMsg = QString("超时, %1 ms后仍未在残像探寻列表找到 %2 的icon").arg(maxWaitMs).arg("异构武装");
        qWarning() << errMsg;
        return false;
    }

    // 点击找到的icon
    Utils::clickWindowClientArea(Utils::hwnd, x + crownLessIcon.cols/2, y + crownLessIcon.rows/2);
    cv::Mat confirm = cv::imread(QString("%1/sentryConstructConfirm.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int timeCostMs = 0;
    if(!loopFindPic(confirm, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到无冠者确认信息", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未确认点击 %2 的icon").arg(defaultMaxWaitMs).arg("异构武装");
        qWarning() << errMsg;
        return false;
    }

    // 尝试点击探测
    cv::Mat detect = cv::imread(QString("%1/detect.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(detect, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到异构武装的探测", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍找到 %2 的探测").arg(defaultMaxWaitMs).arg("异构武装");
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + detect.cols/2, y + detect.rows/2);


    // 尝试点击快速旅行
    cv::Mat fastTravel = cv::imread(QString("%1/fastTravel.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(fastTravel, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到异构武装的快速旅行", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未找到 %2 的快速旅行").arg(defaultMaxWaitMs).arg("异构武装");
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + fastTravel.cols/2, y + fastTravel.rows/2);

    // 不断找背包 找到说明加载完毕  这里加载可能时间稍长 允许10s
    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(bagImg, 0.8, 10000, defaultRefreshMs, "传送异构武装加载超时", similarity, x, y, timeCostMs)){
        errMsg = QString("超时, %1 ms后仍未完成 %2 的传送游戏加载").arg(10000).arg("异构武装");
        qWarning() << errMsg;
        return false;
    }

    return true;
    */

    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "sentryConstruct", "异构武装", errMsg);

    return isGeneralPrepared;
}


bool MainBackendWorkerNew::echoList2bossPositionPreparation(const NormalBossSetting &normalBossSetting, \
                                                            const QString& bossEnName, \
                                                            const QString& bossChName, QString& errMsg){
    // 现在是在残像探寻列表
    // 移动到boss列表
    Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListPos.x, scrollEchoListPos.y);

    QElapsedTimer timer;
    timer.start();
    // 匹配、滚动；最多允许时间 ?S 必须能在残像探寻找到boss icon 否则认为失败
    int maxWaitMs = 10 * 1000;
    cv::Mat bossIcon = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg(bossEnName).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;
    double similarity;
    bool findBossIcon = false;
    while(timer.elapsed() < maxWaitMs && isBusy()){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        findBossIcon = Utils::findPic(capImg, bossIcon, 0.9, x, y, similarity);
        if(!findBossIcon){
            this->dragWindowClient3(Utils::hwnd, scrollEchoListsStartPos.x, scrollEchoListsStartPos.y, \
                                    scrollEchoListsEndPos.x, scrollEchoListsEndPos.y, \
                                    20, 20);
            Sleep(500);
            Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListsWaitPos.x, scrollEchoListsWaitPos.y);
        }
        else{
            break;  //找到了icon
        }
    }
    // 用户打断 正常返回
    if(!isBusy()){
        return true;
    }

    if(!findBossIcon){
        // 超时 异常
        errMsg = QString("超时, %1 ms后仍未在残像探寻列表找到 %2 的icon").arg(maxWaitMs).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }

    // 点击找到的icon
    Utils::clickWindowClientArea(Utils::hwnd, x + bossIcon.cols/2, y + bossIcon.rows/2);
    cv::Mat confirm = cv::imread(QString("%1/%2Confirm.bmp").arg(Utils::IMAGE_DIR_EI()).arg(bossEnName).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int timeCostMs = 0;
    if(!loopFindPic(confirm, 0.7, defaultMaxWaitMs, defaultRefreshMs, "未能找到boss确认信息", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍未确认点击 %2 的icon").arg(defaultMaxWaitMs).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }

    // 尝试点击探测
    cv::Mat detect = cv::imread(QString("%1/detect.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(detect, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到boss的探测", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍找到 %2 的探测").arg(defaultMaxWaitMs).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + detect.cols/2, y + detect.rows/2);

    // 尝试点击快速旅行
    cv::Mat fastTravel = cv::imread(QString("%1/fastTravel.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(fastTravel, 0.8, defaultMaxWaitMs, defaultRefreshMs, "未能找到boss的快速旅行", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍未找到 %2 的快速旅行").arg(defaultMaxWaitMs).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + fastTravel.cols/2, y + fastTravel.rows/2);

    // 不断找背包 找到说明加载完毕  这里加载可能时间稍长 允许10s
    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(bagImg, 0.8, 10000, defaultRefreshMs, "传送boss 加载超时", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍未完成 %2 的传送游戏加载").arg(10000).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }

    return true;
}

bool MainBackendWorkerNew::lockOnePageEcho(const LockEchoSetting& lockEchoSetting){
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
    cv::Mat lockTrueMat = cv::imread(
        QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("lockTrue.bmp").toLocal8Bit().toStdString(),
        cv::IMREAD_UNCHANGED
    );
    cv::Mat lockFalseMat = cv::imread(
        QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("lockFalse.bmp").toLocal8Bit().toStdString(),
        cv::IMREAD_UNCHANGED
    );
    cv::Mat discardTrueMat = cv::imread(
        QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("discardTrue.bmp").toLocal8Bit().toStdString(),
        cv::IMREAD_UNCHANGED
    );
    cv::Mat discardFalseMat = cv::imread(
        QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("discardFalse.bmp").toLocal8Bit().toStdString(),
        cv::IMREAD_UNCHANGED
    );

    cv::Mat cost1Mat = cv::imread(
        QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("cost1.bmp").toLocal8Bit().toStdString(),
        cv::IMREAD_UNCHANGED
    );
    cv::Mat cost3Mat = cv::imread(
        QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("cost3.bmp").toLocal8Bit().toStdString(),
        cv::IMREAD_UNCHANGED
    );
    cv::Mat cost4Mat = cv::imread(
        QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("cost4.bmp").toLocal8Bit().toStdString(),
        cv::IMREAD_UNCHANGED
    );


    for (int i = 0; i < maxColNum && isBusy(); i++) {
        for (int j = 0; j < maxRowNum && isBusy(); j++) {
            capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            int x, y;
            // 声骸卡片 矩形位置
            cv::Rect echoRect = cv::Rect(topLeftEchoROI.x + i * echoColMargin,
                                         topLeftEchoROI.y + j * echoRowMargin,
                                         topLeftEchoROI.width,
                                         topLeftEchoROI.height);

            // 声骸卡片 左下角的套装图标
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
                //cv::Mat thisEchoRectRoiImg = capImg(echoSetRect).clone();
                cv::Mat thisEchoRectRoiImg = capImg(echoRect).clone();
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
                               .arg(j + 1)
                               .arg(i + 1)
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
                              .arg(j + 1)
                              .arg(i + 1)
                              .arg(echoSetNameTranslationMap[bestEchoSetName])
                              .arg(maxSimilarity);
                continue;  // 不进行词条判断
            }

            // 左键单击 选中当前声骸
            Utils::moveMouseToClientArea(Utils::hwnd, echoRect.x + echoRect.width / 2,  echoRect.y + echoRect.height / 2);
            Sleep(100);
            Utils::clickWindowClientArea(Utils::hwnd, echoRect.x + echoRect.width / 2,  echoRect.y + echoRect.height / 2);
            Sleep(500);
            capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            // 先判断是什么COST
            maxSimilarity = 0.0;
            QString costIdx;  // cost1 cost3
            if(!lockEchoSetting.echoSetName2singleSetting.contains(bestEchoSetName)){
                qWarning() << QString("输入参数不支持 %1 套装").arg(bestEchoSetName);
                continue;
            }
            // 当前套装的参数
            SingleEchoSetting singleEchoSetting = lockEchoSetting.echoSetName2singleSetting[bestEchoSetName];
            // 判断是否锁定
            bool isLock;
            double isLockTrueSimilarity = 0.0;
            double isLockFalseSimilarity = 0.0;
            Utils::findPic(capImg(isLockROI).clone(), lockTrueMat, 0.7, x, y, isLockTrueSimilarity);
            Utils::findPic(capImg(isLockROI).clone(), lockFalseMat, 0.7, x, y, isLockFalseSimilarity);
            if(isLockTrueSimilarity == 0.0 || isLockFalseSimilarity == 0.0){
                qWarning() << QString("%1 行  %2 列声骸无法判定锁定状态").arg(j+1).arg(i+1);
                continue;
            }


            // 判断是否丢弃
            bool isDiscard;
            double isDiscardTrueSimilarity = 0.0;
            double isDiscardFalseSimilarity = 0.0;
            Utils::findPic(capImg(isDiscardROI).clone(), discardTrueMat, 0.7, x, y, isDiscardTrueSimilarity);
            Utils::findPic(capImg(isDiscardROI).clone(), discardFalseMat, 0.7, x, y, isDiscardFalseSimilarity);
            if(isDiscardTrueSimilarity == 0.0 || isDiscardFalseSimilarity == 0.0){
                qWarning() << QString("%1 行  %2 列声骸无法判定丢弃状态").arg(j+1).arg(i+1);
                continue;
            }


            if(isLockTrueSimilarity > isLockFalseSimilarity){
                isLock = true;
            }
            else{
                isLock = false;
            }

            if(isDiscardTrueSimilarity > isDiscardFalseSimilarity){
                isDiscard = true;
            }
            else{
                isDiscard = false;
            }

            if(isLock && isDiscard){
                if(isLockTrueSimilarity > isDiscardTrueSimilarity){
                    isLock = true;
                    isDiscard = false;
                }
                else{
                    isLock = false;
                    isDiscard = true;
                }
            }

            // 判断是COST？
            int cost;
            double isCost1Similarity = 0.0;
            double isCost3Similarity = 0.0;
            double isCost4Similarity = 0.0;
            Utils::findPic(capImg(costROI).clone(), cost1Mat, 0.7, x, y, isCost1Similarity);
            Utils::findPic(capImg(costROI).clone(), cost3Mat, 0.7, x, y, isCost3Similarity);
            Utils::findPic(capImg(costROI).clone(), cost4Mat, 0.7, x, y, isCost4Similarity);
            if(isCost1Similarity == 0.0 || isCost3Similarity == 0.0 || isCost4Similarity == 0.0){
                qWarning() << QString("%1 行  %2 列声骸无法判定COST").arg(i+1).arg(j+1);
                continue;
            }
            else if(isCost1Similarity > isCost3Similarity && isCost1Similarity > isCost4Similarity){
                cost = 1;
            }
            else if(isCost3Similarity > isCost1Similarity && isCost3Similarity > isCost4Similarity){
                cost = 3;
            }
            else if(isCost4Similarity > isCost1Similarity && isCost4Similarity > isCost3Similarity){
                cost = 4;
            }
            else{
                qWarning() << QString("%1 行  %2 列声骸无法判定COST").arg(j+1).arg(i+1);
                //continue;
            }

            // 根据输入参数 是否要判断此声骸的主词条
            QString thisEchoMsg = QString("%1行%2列声骸_之前锁定 %3_之前丢弃%4_这是COST%5").arg(j+1).arg(i+1).arg(isLock?"是": "否").arg(isDiscard?"是": "否").arg(cost);

            if(singleEchoSetting.isLockJudge && isLock ||
                    singleEchoSetting.isDiscardedJudge && isDiscard ||
                    singleEchoSetting.isNormalJudge && (!isDiscard && !isLock)){
                // 如果设置要求和实际判断结果符合
                QVector<QString> entryList = singleEchoSetting.cost2EntryMap[cost];
                if(entryList.empty()){
                    // 没有词条需要判断
                    continue;
                }

                bool isFoundEntry = false;
                // 最优先判断暴击伤害 容易和暴击率混淆
                double entrySimilarity = 0.0;
                cv::Mat criticalDMGIcon = entryName2entryIconMap["criticalDMG"].clone();
                if(entryList.contains("criticalDMG") && Utils::findPic(capImg, criticalDMGIcon, 0.8, x, y, entrySimilarity)){
                    qInfo() << QString("%1. 套装所属 %2。需要暴击伤害词条，锁定. 相似系数%3").arg(thisEchoMsg).arg(bestEchoSetName).arg(entrySimilarity);
                    isFoundEntry = true;
                    if(!isLock){
                        Utils::keyPress(Utils::hwnd, 'C', 1);
                        Sleep(300);
                    }
                    continue;
                }

                for(auto entry : entryList){
                    cv::Mat entryIcon = entryName2entryIconMap[entry].clone();
                    if(Utils::findPic(capImg, entryIcon, 0.8, x, y, entrySimilarity)){
                        qInfo() << QString("%1. 套装所属 %2。需要%3词条，锁定. 相似系数%4").arg(thisEchoMsg).arg(bestEchoSetName).arg(entry).arg(entrySimilarity);
                        isFoundEntry = true;
                        if(!isLock){
                            Utils::keyPress(Utils::hwnd, 'C', 1);
                            Sleep(300);
                        }
                        break;
                    }
                    else{
                        qInfo() << QString("%1 套装所属 %2。需要%3词条，相似系数不够. 相似系数%4").arg(thisEchoMsg).arg(bestEchoSetName).arg(entry).arg(entrySimilarity);
                    }
                }

                if(!isFoundEntry){
                    qInfo() << QString("%1. 套装所属 %2。没有找到要的词条，丢弃").arg(thisEchoMsg).arg(bestEchoSetName);
                    if(!isDiscard){
                        Utils::keyPress(Utils::hwnd, 'Z', 1);
                        Sleep(300);
                    }
                }
            }
            Sleep(100);
        }

        if(!isBusy()){
            return true;
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

bool MainBackendWorkerNew::loopFindPic(const cv::Mat& templateImg, const double& requireSimilarity, \
                 const int &maxWaitMs, const int &refreshMs, const QString& ifFailedMsg, double& similarity, \
                 int& x, int& y, int& timeCostMs){

    QElapsedTimer timer;
    timer.start();

    similarity = 0.0;
    x = -1;
    y = -1;
    timeCostMs = 0;

    // 验证输入参数的有效性
    if (templateImg.empty() || requireSimilarity <= 0.0 || requireSimilarity > 1.0) {
        qWarning() << "Invalid input parameters for loopFindPic.";
        return false;
    }

    cv::Mat capImg;
    while (timer.elapsed() < maxWaitMs) {
        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(capImg.empty()){
            // 等待下一次轮询
            Sleep(refreshMs);
            continue;
        }

        // 每次循环尝试匹配模板
        int tempX = -1, tempY = -1;
        double tempSimilarity = 0.0;

        if (Utils::findPic(capImg, templateImg, requireSimilarity, tempX, tempY, tempSimilarity)) {
            // 成功匹配，更新结果并返回
            x = tempX;
            y = tempY;
            similarity = tempSimilarity;
            timeCostMs = timer.elapsed();
            return true;
        }

        // 更新匹配相似度
        similarity = std::max(similarity, tempSimilarity);

        if(!isBusy()){
            qWarning() << "break by user. Similarity:" << similarity;
            return false;
        }
        // 等待下一次轮询
        Sleep(refreshMs);
    }

    const bool isSave = false;
    if(isSave){
        // 超时未找到，保存调试图像
        Utils::saveDebugImg(capImg, cv::Rect(), x, y, ifFailedMsg);
    }

    // 返回超时状态
    timeCostMs = timer.elapsed();
    qWarning() << "Failed to find template within the specified time limit. Similarity:" << similarity;
    return false;

}
