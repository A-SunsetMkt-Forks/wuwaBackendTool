#include "mainbackendworkernew.h"

QAtomicInt MainBackendWorkerNew::lastMode = 0;

MainBackendWorkerNew::MainBackendWorkerNew(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);
    initEchoSetName2IconMap();
    initEntryName2IconMap();

    // 初始化后台战斗线程
    m_fightBackendWorkerNew.moveToThread(&m_fightThread);
    m_fightThread.start();
    connect(this, &MainBackendWorkerNew::startFight, &this->m_fightBackendWorkerNew, &FightBackendWorkerNew::onStartFight);

    // 初始化后台监控游戏重启线程
    //QThread m_watcherThread;
    //WuwaWatcher m_watcher;
    m_watcher.moveToThread(&m_watcherThread);
    m_watcherThread.start();
    connect(this, &MainBackendWorkerNew::startWatcher, &this->m_watcher, &WuwaWatcher::startWatcher);
    connect(&this->m_watcher, &WuwaWatcher::rebootGame, this, &MainBackendWorkerNew::onRebootGame);

}

MainBackendWorkerNew::~MainBackendWorkerNew()
{
    m_fightBackendWorkerNew.stopWorker();
    m_fightThread.quit();
    m_fightThread.wait();

    m_watcher.stopWorker();
    m_watcherThread.quit();
    m_watcherThread.wait();

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
    m_fightBackendWorkerNew.stopWorker();
    m_watcher.stopWorker();

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
    isFirstSkipMonthCard = false;
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
            skipMonthCard();
            // 尝试进入背包
            if(!enterBagInterface()){
                skipMonthCard();
                if(!enterBagInterface()){
                    if(isLockEchoStop(true, false, QString("未能找到背包图标"), lockEchoSetting)) return;
                }

            }
            // 检查用户是否打断
            if(isLockEchoStop(false, true, QString("脚本运行结束"), lockEchoSetting)) return;

            // 尝试进入声骸页
            if(!enterEchoInterface()){
                skipMonthCard();
                if(!enterEchoInterface()){
                    if(isLockEchoStop(true, false, QString("未能找到声骸图标"), lockEchoSetting)) return;
                }
            }
            // 检查用户是否打断
            if(isLockEchoStop(false, true, QString("脚本运行结束"), lockEchoSetting)) return;

            // 核心代码 处理当前页面的所有声骸
            for(int iPage = 0; iPage < 10 && isBusy(); iPage++){
                if(!lockOnePageEcho(lockEchoSetting)){
                    skipMonthCard();
                    if(!lockOnePageEcho(lockEchoSetting)){
                        if(isLockEchoStop(true, false, QString("锁定%1页面声骸时出错").arg(iPage), lockEchoSetting)) return;
                    }
                }

                qInfo() << QString("翻页");
                for(int j = 0; j < 4 && isBusy(); j++){
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
    MainBackendWorkerNew::lastMode.store(1);

    emit startWatcher();

    isFirstSkipMonthCard = false;
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
        switchEchoListPos = cv::Point(-1, -1);  // 残像探寻图像坐标初始化为不可用
        while(isBusy()){
            QList<NormalBossEnum> allBossList = normalBossSetting.bossSettings.keys();
            for(NormalBossEnum thisBoss : allBossList){
                if(normalBossSetting.isBossEnabled(thisBoss)){
                    skipMonthCard();   // 返回false以后 跳过月卡 再尝试一次 如果仍然失败 则退出
                    // 回到主界面 通过索拉指南 残像探寻 进入boss
                    bool isPrepared = normalBossPreperation(normalBossSetting, errMsg);
                    if(!isPrepared){
                        skipMonthCard();
                        if(!normalBossPreperation(normalBossSetting, errMsg)){
                            Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), 0, 0, "boss准备工作失败" + errMsg);
                            if(isNormalBossStop(true, false, errMsg, normalBossSetting)) return;
                        }
                    }

                    if(!isBusy()){
                        if(isNormalBossStop(true, false, QString("用户中止"), normalBossSetting)) return;
                    }

                    // 要刷这个boss的
                    QString oneBossErr;
                    bool oneBossLoopDone = oneBossLoop(normalBossSetting, thisBoss, oneBossErr);
                    if(!oneBossLoopDone){
                        //skipMonthCard();
                        //if(!oneBossLoopDone){ }

                        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), 0, 0, "boss单次刷取失败" + oneBossErr);
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

void MainBackendWorkerNew::onStartSpecialBoss(const SpecialBossSetting &specialBossSetting){
    m_isRunning.store(1);
    MainBackendWorkerNew::lastMode.store(2);

    emit startWatcher();

    isFirstSkipMonthCard = false;
    qInfo() << QString("MainBackendWorkerNew::onStartSpecialBoss");

    // 初始化句柄
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        emit specialBossDone(false, QString("未能初始化鸣潮窗口句柄"), specialBossSetting);
        m_isRunning.store(0);
        return;
    }

    bool isWuwaRunning = Utils::isWuwaRunning();
    if(!isWuwaRunning){
        emit specialBossDone(false, QString("未能初始化鸣潮窗口句柄"), specialBossSetting);
        m_isRunning.store(0);
        return;
    }

    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 每一步都需要执行该检查 封装为匿名函数
    auto isSpecialBossStop = [&](bool isAbort, bool isNormalEnd, const QString& msg, const SpecialBossSetting &specialBossSetting) {
        bool isWuwaRunning = Utils::isWuwaRunning();
        if(!isWuwaRunning){
            // 鸣潮已经被关闭 异常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit specialBossDone(false, QString("鸣潮已经被关闭"), specialBossSetting);
            m_isRunning.store(0);
            return true;
        }

        if(!isBusy()){
            // 用户中止 正常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit specialBossDone(true, QString("用户停止"), specialBossSetting);
            m_isRunning.store(0);
            return true;
        }
        else{
            if(isAbort){
                // 根据输入参数决定是否正常结束
                AttachThreadInput(currentThreadId, threadId, FALSE);
                emit specialBossDone(isNormalEnd, msg, specialBossSetting);
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
        switchEchoListPos = cv::Point(-1, -1);  // 残像探寻图像坐标初始化为不可用
        while(isBusy()){
            if(!isBusy())  {
                if(isSpecialBossStop(true, false, QString("重新挑战boss失败 %1").arg(errMsg), specialBossSetting)) return;
            }

            skipMonthCard();   // 返回false以后 跳过月卡 再尝试一次 如果仍然失败 则退出

            // 准备工作
            switch (specialBossSetting.boss) {
            case SpecialBossSetting::SpecialBoss::Rover:
                //其实是dreamless 无妄者
                if(!dreamlessPreparation(specialBossSetting, errMsg)){
                    skipMonthCard();   // 返回false以后 跳过月卡 再尝试一次 如果仍然失败 则退出
                    if(!dreamlessPreparation(specialBossSetting, errMsg)){
                        qWarning() << QString("无妄者 准备工作失败 %1").arg(errMsg);
                        if(isSpecialBossStop(true, false, QString("无妄者 准备工作失败 %1").arg(errMsg), specialBossSetting)) return;
                    }
                    continue;
                }

                if(!specialBossFightPickupEcho(specialBossSetting, "dreamless", errMsg)){
                    qWarning() << QString("无妄者 战斗失败 %1").arg(errMsg);
                }

                break;

            case SpecialBossSetting::SpecialBoss::Hecate:
                if(!hecatePreparation(specialBossSetting, errMsg)){
                    skipMonthCard();   // 返回false以后 跳过月卡 再尝试一次 如果仍然失败 则退出
                    if(!hecatePreparation(specialBossSetting, errMsg)){
                        qWarning() << QString("赫卡忒 准备工作失败 %1").arg(errMsg);
                        if(isSpecialBossStop(true, false, QString("赫卡忒 准备工作失败 %1").arg(errMsg), specialBossSetting)) return;
                    }
                    continue;
                }

                if(!specialBossFightPickupEcho(specialBossSetting, "hecate", errMsg)){
                    qWarning() << QString("赫卡忒 战斗失败 %1").arg(errMsg);
                }

                break;

            case SpecialBossSetting::SpecialBoss::Jue:
                if(!juePreparation(specialBossSetting, errMsg)){
                    skipMonthCard();   // 返回false以后 跳过月卡 再尝试一次 如果仍然失败 则退出
                    if(!juePreparation(specialBossSetting, errMsg)){
                        qWarning() << QString("角 准备工作失败 %1").arg(errMsg);
                        if(isSpecialBossStop(true, false, QString("角 准备工作失败 %1").arg(errMsg), specialBossSetting)) return;
                    }
                    continue;
                }

                if(!specialBossFightPickupEcho(specialBossSetting, "jue", errMsg)){
                    qWarning() << QString("角 战斗失败 %1").arg(errMsg);
                }

                break;

            default:
                qWarning() << "boss 未适配";
                if(isSpecialBossStop(true, false, "boss 未适配", specialBossSetting)) return;
                break;


            }

            if(!isBusy())  {
                if(isSpecialBossStop(true, false, QString("重新挑战boss失败 %1").arg(errMsg), specialBossSetting)) return;
            }

            // 重新挑战
            if(!repeatBattle(specialBossSetting, errMsg)){
                skipMonthCard();   // 返回false以后 跳过月卡 再尝试一次 如果仍然失败 则退出
                if(!repeatBattle(specialBossSetting, errMsg)){
                    qWarning() << QString("重新挑战boss失败 %1").arg(errMsg);
                    if(isSpecialBossStop(true, false, QString("重新挑战boss失败 %1").arg(errMsg), specialBossSetting)) return;
                }
                continue;
            }

            if(!isBusy())  {
                if(isSpecialBossStop(true, false, QString("重新挑战boss失败 %1").arg(errMsg), specialBossSetting)) return;
            }

            Sleep(500);
        }

        if(!isBusy())  {
            if(isSpecialBossStop(true, false, QString("重新挑战boss失败 %1").arg(errMsg), specialBossSetting)) return;
        }
    }
    else{
        if(isSpecialBossStop(true, false, QString("未能将线程附加到鸣潮窗口"), specialBossSetting)) return;
    }

    if(isSpecialBossStop(true, true, QString("脚本运行结束"), specialBossSetting)) return;
    return;


}

// 响应后台监控的要求 重启游戏
void MainBackendWorkerNew::onRebootGame(){
    // #####  不保真  需要测试
    QString exePath = QDir::toNativeSeparators(Utils::EXE_PATH());
    exePath.replace("\\", "/");

    qInfo() << QString("收到重启要求 重启鸣潮进程 %1").arg(exePath);
    QFileInfo fileInfo(exePath);
    if (!fileInfo.exists() || !fileInfo.isExecutable()) {
        qWarning() << "鸣潮 Executable does not exist or is not executable:" << exePath;
        return;
    }
    QString workingDir = fileInfo.absolutePath();
    QString exeFileName = fileInfo.fileName();

    qInfo() << QString("10s 后重启游戏 工作路径 %1   游戏文件名 %2").arg(workingDir).arg(exeFileName);
    // 等待 直到窗体不再存在
    int maxWaitWuwaQuitMs = 60000;
    int waitMs = 0;
    const int sleepPeriod = 200;
    while(waitMs < maxWaitWuwaQuitMs){
        Sleep(sleepPeriod);
        if(!Utils::isWuwaRunning()){
            break;
        }
    }

    if(waitMs >= maxWaitWuwaQuitMs){
        qWarning() << QString("收到重启请求 但是检测到鸣潮始终未关闭");
        return;
    }

    Sleep(5000);  // 等10秒 确保关闭了
    // 1. 转换为 std::wstring
    std::wstring wExePath  = exePath.toStdWString();
    std::wstring wWorkDir  = workingDir.toStdWString();

    // 2. 调试输出
    qDebug() << "Try to create process from exePath:" << exePath
             << "with working directory:" << workingDir;

    // 3. 调用自定义的 myCreateProcess
    bool ok = Utils::myCreateProcess(wExePath, wWorkDir);
    if (!ok) {
        qWarning() << "Failed to launch process:" << exePath;
        return;
    }
    qInfo() << "Launch wuwa process succeeded:" << exePath;

    const int maxWaitInitHwnd = 20000;
    int waitInitHwnd = 0;
    bool isInitWuwaHwnd = false;
    while(waitInitHwnd < maxWaitInitHwnd){
        // 最大等待300S 点击 进入游戏
        isInitWuwaHwnd = Utils::initWuwaHwnd();
        if(isInitWuwaHwnd){
            break;
        }
        Sleep(1000);
        waitInitHwnd += 1000;
    }

    if(!isInitWuwaHwnd){
        qWarning() << QString("尝试等待20s 仍未获取到有效HWND");
        return;
    }

    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    if(AttachThreadInput(currentThreadId, threadId, TRUE)) {
        // 激活窗口
        SendMessage(Utils::hwnd, WM_ACTIVATE, WA_ACTIVE, 0);

        // 最大等待300s 找到背包
        const int maxWaitLinkStartMs = 300000;
        int waitLinkStartMs = 0;
        int x, y, timeCost;
        double similarity;
        bool isFoundLinkStart = false;
        cv::Mat linkStart = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("linkStart").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        while(waitLinkStartMs < maxWaitLinkStartMs){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if(Utils::findPic(capImg, linkStart, 0.8, x, y)){
                isFoundLinkStart = true;
                break;
            }

            Sleep(1000);
            waitLinkStartMs += 1000;
        }

        if(!isFoundLinkStart){
            qWarning() << QString("重启鸣潮后 未能找到 点击连接 按钮");
            Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), 0, 0, "重启鸣潮后 未能找到 点击连接 按钮");
            AttachThreadInput(currentThreadId, threadId, FALSE);
            return;
        }

        // 点击按钮 没响应继续点
        Utils::clickWindowClientArea(Utils::hwnd, x + linkStart.cols / 2 , y + linkStart.rows / 2);
        qInfo() << QString("找到了点击连接 按钮，点击它");
        bool isEnterGame = false;
        for(int i = 0; i < 10; i++){
            int xOut, yOut;
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if(!Utils::findPic(capImg, linkStart, 0.8, xOut, yOut)){
                // 找不到了才说明对了
                isEnterGame = true;
                break;
            }
            Utils::clickWindowClientArea(Utils::hwnd, x + linkStart.cols / 2 , y + linkStart.rows / 2);
            qInfo() << QString("找到了点击连接 按钮，点击它");
            Sleep(250);
        }

        if(!isEnterGame){
            qWarning() << QString("重启鸣潮后 找到了点击连接 按钮，点击它N次仍未进入游戏");
            Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), 0, 0, "重启鸣潮后 找到了点击连接 按钮，点击它N次仍未进入游戏");
            AttachThreadInput(currentThreadId, threadId, FALSE);
            return;
        }

        const int maxWaitBagMs = 30000;
        int waitBagMs = 0;
        bool isFoundBag = false;
        cv::Mat bag = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("bag").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        while(waitBagMs < maxWaitBagMs){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if(Utils::findPic(capImg, bag, 0.65, x, y)){
                isFoundBag = true;
                break;
            }

            Sleep(1000);
            waitBagMs += 1000;
        }

        if(!isFoundBag){
            qWarning() << QString("重启鸣潮后 点击了进入游戏 但是30秒后未发现背包");
            Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), 0, 0, "重启鸣潮后 点击了进入游戏 但是30秒后未发现背包");
            AttachThreadInput(currentThreadId, threadId, FALSE);
            return;
        }

        qInfo() << QString("重启完毕 找到了背包 要求UI继续脚本");
    }
    else{
        qWarning() << QString("重启游戏后 无法激活窗体发送指令");
        return;
    }

    AttachThreadInput(currentThreadId, threadId, FALSE);
    emit askUIstart(MainBackendWorkerNew::lastMode.load());
    return ;
}

bool MainBackendWorkerNew::enterBagInterface() {
    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    cv::Mat bagIcon = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;

    // 检测背包按钮
    bool isDetectBag = Utils::findPic(capImg(searchBagROI).clone(), bagIcon, 0.7, x, y);

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
            if (Utils::findPic(capImg(searchBagROI).clone(), bagIcon, 0.7, x, y)) {
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
    //Utils::saveDebugImg(capImg, bagRoiRect, bagRoiRect.x + bagRoiRect.width / 2, bagRoiRect.y + bagRoiRect.height / 2, "找到了背包");
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

    if(Utils::findPic(capImg(searchEchoIconROI).clone(), echoYellowImg, 0.8, x, y)){
        cv::Rect echoIconRoi = cv::Rect(x + searchEchoIconROI.x, y + searchEchoIconROI.y, echoWhiteImg.cols, echoWhiteImg.rows);
        //Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
        Sleep(500);
        Utils::clickWindowClientArea(Utils::hwnd, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2);
        Sleep(500);
        Utils::saveDebugImg(capImg, echoIconRoi, echoIconRoi.x + echoIconRoi.width / 2, echoIconRoi.y + echoIconRoi.height / 2, "进入背包后找到了声骸图标 点击黄色图标");
        //Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
        return true;
    }

    if(Utils::findPic(capImg(searchEchoIconROI).clone(), echoWhiteImg, 0.8, x, y)){
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
        errMsg = "连续7次 ESC 未能找到背包按钮";
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
    // 检查用户是否打断
    if(!isBusy()) return true;

    qInfo() << QString("成功进入残像探寻 预备工作结束");

    return true;
}

bool MainBackendWorkerNew::backToMain(){
    qInfo() << QString("尝试恢复到主界面");

    cv::Mat missionImg = cv::imread(QString("%1/mission.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isFindBagFinal = false;  // 是否找到背包图片
    for(int i = 0; i < 7 && isBusy(); i++){
        double similarity;
        int x, y, timeCostMs;
        bool isFind = loopFindPic(bagImg, 0.75, defaultMaxWaitMs, 250, "未能找到背包按钮 尝试esc后 继续寻找", similarity, x, y, timeCostMs);
        if(!isBusy()){
            return true;
        }

        if(!isFind){
            Utils::keyPress(Utils::hwnd, VK_ESCAPE, 1);
        }
        else{
            isFindBagFinal = true;
            break;
        }
    }
    if(!isBusy()){
        return true;
    }

    return isFindBagFinal;
}

bool MainBackendWorkerNew::enterSolaGuide(){
    qInfo() << QString("尝试进入到索拉指南");

    Sleep(1000);
    Utils::keyPress(Utils::hwnd, VK_F2, 1);
    Sleep(1000);
    return true;
}

bool MainBackendWorkerNew::enterEchoList(){
    qInfo() << QString("尝试进入到残像探寻");
    if(switchEchoListPos.x >=0 && switchEchoListPos.y >=0){
        qInfo() << QString("点击之前记录的残像探寻坐标 %1 %2").arg(switchEchoListPos.x).arg(switchEchoListPos.y);
        // 之前记录过残像探寻的坐标 直接点击
        double similarity;
        int x, y, timeCostMs;
        Utils::moveMouseToClientArea(Utils::hwnd, switchEchoListPos.x, switchEchoListPos.y);
        Sleep(250);
        Utils::clickWindowClientArea(Utils::hwnd, switchEchoListPos.x, switchEchoListPos.y);
        Sleep(250);
        cv::Mat echoBossListDefault = cv::imread(QString("%1/echoBossListDefault.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        bool isTrueClicked = loopFindPic(echoBossListDefault, 0.8, defaultMaxWaitMs, defaultRefreshMs, "找到了残像探寻图标 但点击没反应", similarity, x, y, timeCostMs);
        if(!isBusy()){
            return true;
        }

        if(isTrueClicked){
            return true;
        }

    }
    else{
        qInfo() << QString("重新寻找残像探寻坐标");
        // 重新找残像探寻的图标位置
        cv::Mat echoListUncheckedImg = cv::imread(QString("%1/echoListUnchecked.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        double similarity;
        int x, y, timeCostMs;
        bool isFind = loopFindPic(echoListUncheckedImg, 0.7, defaultMaxWaitMs, defaultRefreshMs, "未能在索拉指南找到残像探寻", similarity, x, y, timeCostMs);
        if(!isBusy()){
            return true;
        }
        if(isFind) {
            cv::Mat echoListChecked = cv::imread(QString("%1/echoListChecked.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
            cv::Mat echoBossListDefault = cv::imread(QString("%1/echoBossListDefault.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
            // 至多再点击4次 进不去就说明有问题了
            for(int i = 0; i < 4 && isBusy(); i++){
                Utils::moveMouseToClientArea(Utils::hwnd, x + echoListUncheckedImg.cols / 2, y + echoListUncheckedImg.rows / 2);
                Sleep(250);
                Utils::clickWindowClientArea(Utils::hwnd, x + echoListUncheckedImg.cols / 2, y + echoListUncheckedImg.rows / 2);
                Sleep(250);
                int echoListx, echoListy;
                bool isTrueClicked = loopFindPic(echoBossListDefault, 0.8, defaultMaxWaitMs, defaultRefreshMs, "找到了残像探寻图标 但点击没反应", similarity, echoListx, echoListy, timeCostMs);
                if(!isBusy()){
                    break;
                }

                if(isTrueClicked){
                    switchEchoListPos = cv::Point(x + echoListUncheckedImg.cols / 2, y + echoListUncheckedImg.rows / 2);
                    qInfo() << QString("记录残像探寻坐标 %1 %2").arg(switchEchoListPos.x).arg(switchEchoListPos.y);
                    return true;
                }

                Sleep(500);
            }
            if(!isBusy()){
                switchEchoListPos = cv::Point(-1, -1);
                return true;  // 用户打断
            }
            switchEchoListPos = cv::Point(-1, -1);
            return false;  // 点了两次残像探寻都进不去
        }
        else{
            switchEchoListPos = cv::Point(-1, -1);
            return false;
        }
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

    case NormalBossEnum::Lorelei:
        if(!loreleiPreparation(normalBossSetting, errMsg)){
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

    case NormalBossEnum::FallacyOfNoReturn:
        if(!fallacyOfNoReturnPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::FeilianBeringal:
        if(!feilianBeringalPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::ImpermanenceHeron:
        if(!impermanenceHeronPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::InfernoRider:
        if(!infernoRiderPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::LampylumenMyriad:
        if(!lampylumenMyriadPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::MechAbomination:
        if(!mechAbominationPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::MourningAix:
        if(!mourningAixPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::TempestMephis:
        if(!tempestMephisPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::ThunderingMephis:
        if(!thunderingMephisPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::BellBorneGeochelone:
        if(!bellBorneGeochelonePreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::NightmareCrownless:
        if(!nightmareCrownlessPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::NightmareFeilianBeringal:
        if(!nightmareFeilianBeringalPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::NightmareImpermanenceHeron:
        if(!nightmareImpermanenceHeronPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::NightmareInfernoRider:
        if(!nightmareInfernoRiderPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::NightmareMourningAix:
        if(!nightmareMourningAixPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::NightmareTempestMephis:
        if(!nightmareTempestMephisPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;

    case NormalBossEnum::NightmareThunderingMephis:
        if(!nightmareThunderingMephisPreparation(normalBossSetting, errMsg)){
            return false;
        }
        break;


    default:
        qWarning() << QString("BOSS %1 is not finished coding yet").arg(NormalBossSetting::Enum2QString(bossName));
        return false;
    }

    if(!isBusy()){
        return true;
    }

    // 保证已经锁定了boss
    // 战斗代码
    QElapsedTimer timer;
    timer.start();
    emit startFight();

    // 每隔250ms判断boss状态
    // 能检测到背包图标 检测不到bosstitle 说明boss 死了 停止战斗
    // 能检测到背包图标 检测得到bosstitle 说明boss还活着 继续锤 恢复战斗
    // 检测不到背包图标 检测不到bosstitle 说明在放大招 暂停战斗
    cv::Mat bossTitle = cv::imread(QString("%1/%2Title.bmp").arg(Utils::IMAGE_DIR_EI()).arg(NormalBossSetting::Enum2QString(bossName)).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int titleX, titleY;
    double similarityTitle;

    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int bagX, bagY;
    double similarityBag;
    int voteBossDead = 0;
    int voteIamDead = 0;
    while(isBusy() && timer.elapsed() < maxFightMs){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        bool isFindTitle = Utils::findPic(capImg, bossTitle, 0.6, titleX, titleY, similarityTitle);
        bool isFindBag = Utils::findPic(capImg, bagImg, 0.7, bagX, bagY, similarityBag);

        if(isFindBag && isFindTitle){
            voteBossDead = 0;
            voteIamDead = 0;
            m_fightBackendWorkerNew.resumeWorker();
        }
        else if(isFindBag && !isFindTitle){
            voteBossDead = voteBossDead + 1;
            voteIamDead = 0;
            m_fightBackendWorkerNew.pauseWorker();
            if(voteBossDead > 10){
                m_fightBackendWorkerNew.stopWorker();
                Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);  // 回正视角 面向声骸
                //Utils::saveDebugImg(capImg, cv::Rect(), 0 ,0, QString("认为boss死亡_背包匹配度_%1__boss标题匹配度_%2").arg(similarityBag).arg(similarityTitle));
                Sleep(1000);
                qInfo() << QString("战斗完成");
                break;
            }
        }
        else if(!isFindBag && !isFindTitle){
            voteIamDead = voteIamDead + 1;
            voteBossDead = 0;
            m_fightBackendWorkerNew.pauseWorker();
            if(voteIamDead > 40){
                // 大招播放动画最长也就2秒
                // 可能需要复苏 + 跳过月卡
                skipMonthCard();
                //revive();

                if(!revive()){
                    qWarning() << QString("复活失败");
                    m_fightBackendWorkerNew.stopWorker();
                    return false;
                }
                else{
                    qInfo() << QString("复活成功");
                    m_fightBackendWorkerNew.stopWorker();
                    return true;
                }


                //m_fightBackendWorkerNew.stopWorker();
                //return true;
            }
        }
        else{
            //
        }

        Sleep(50);
    }
    m_fightBackendWorkerNew.stopWorker();
    // 可能需要复苏 + 跳过月卡
    skipMonthCard();

    if(!isBusy()){
        m_fightBackendWorkerNew.stopWorker();
        return true;
    }

    if(timer.elapsed() >= maxFightMs){
        m_fightBackendWorkerNew.stopWorker();
        return true;
    }

    // 拾取声骸  记得计数+1
    // 切换3号位 防止椿不会动
    Utils::keyPress(Utils::hwnd, '3', 1);
    revive();
    cv::Mat absorbMat = cv::imread(QString("%1/absorb.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    Sleep(50);
    int absorbX, absorbY;
    double absorbSimilarity;
    bool isAbsorb = false;
    for(int i = 0; i < 15 && isBusy() && !isAbsorb; i++){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, absorbMat, 0.8, absorbX, absorbY, absorbSimilarity)){
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(1500);
            //先停下来 再次判断 点击准确
            isAbsorb = true;
            capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if(Utils::findPic(capImg, absorbMat, 0.8, absorbX, absorbY, absorbSimilarity)){
                // ALT 左键 ALT松
                Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
                Sleep(500);
                Utils::moveMouseToClientArea(Utils::hwnd, absorbX + absorbMat.cols / 2, absorbY + absorbMat.rows / 2 );
                Sleep(500);
                //Utils::saveDebugImg(capImg, cv::Rect(), absorbX + absorbMat.cols / 2, absorbY + absorbMat.rows / 2, "pickUpEcho");
                Utils::clickWindowClientArea(Utils::hwnd, absorbX + absorbMat.cols / 2, absorbY + absorbMat.rows / 2 );
                Sleep(500);
                Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
                Sleep(3000);
                break;
            }
            else{
                Sleep(250);
                return true;
            }
        }
        Sleep(250);
    }
    if(!isBusy()){
        //Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
        //Sleep(250);
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(isAbsorb) {pickUpNormalBossEcho = pickUpNormalBossEcho + 1;}
    qInfo() << QString("pickUpEchoNumber %1").arg(pickUpNormalBossEcho);

    return true;
}

bool MainBackendWorkerNew::dragonOfDirgePreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){

    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "dragonOfDirge", "叹息之龙", errMsg);
    if(!isGeneralPrepared){
        return false;
    }
    // 叹息之龙相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();

    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("dragonOfDirgeTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        int x, y;
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("已经锁定 %1").arg("叹息之龙");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}


bool MainBackendWorkerNew::crownLessPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){

    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "crownless", "无冠者", errMsg);
    if(!isGeneralPrepared){
        return false;
    }
    // 无冠者相对复杂 一边向前走10秒 一边找声弦按钮 找到了 停下来 按，等5秒 锁boss 失败就退了
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();

    cv::Mat soundString = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("soundString").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("crownlessTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isFoundSoundString = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        int x, y, timeCostMs;
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, soundString, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            // 找到boss 声弦了
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            isFoundSoundString = true;
            Sleep(250);

            // F
            Utils::keyPress(Utils::hwnd, 'F', 1);

            if(loopFindPic(bossTitle, 0.8, 5*1000, 250, "触发了声弦，但没等到无冠者", similarity, x, y, timeCostMs)){
                if(!isBusy()){
                    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                    Sleep(250);
                    return true;
                }

                Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
                qInfo() << QString("已经锁定 %1").arg("无冠者");
                Sleep(250);
                return true;
            }
            else{
                errMsg = "触发了声弦，但没等到无冠者";
                return false;
            }

        }
    }

    // 没找到声弦 要么被用户中断 要么就是没找到
    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isFoundSoundString){
        errMsg = QString("未能触发声弦");
        return false;
    }

    errMsg = QString("理论上无冠者永远无法到达这个逻辑分支");
    return false;
}

bool MainBackendWorkerNew::loreleiPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "lorelei", "罗蕾莱", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    int x, y, timeCostMs;
    double similarity;

    // 如果提示需要等待 则暂时略过
    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("loreleiTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat neetWait = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("loreleiNeedWait").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat sit = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("loreleiSit2").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isFindNeetWait = Utils::findPic(capImg, neetWait, 0.75, x, y, similarity);
    if(isFindNeetWait){
        // 需要找到座位坐下去 先忽略非夜晚的情况
        return false;
    }
    else{
        // 向前走 N秒
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

        // 如果跑了 N秒没找到 认为失败 跳过
        const int maxRunFindBossMs = 10*1000;
        const int detectBossPeroidMs = 500;

        QElapsedTimer timer;
        timer.start();

        bool isTraced = false;
        while(timer.elapsed() < maxRunFindBossMs && isBusy()){
            int x, y;
            double similarity;
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
                if(!isBusy()){
                    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                    Sleep(250);
                    return true;
                }
                Sleep(250);
                // 找到boss title了
                isTraced = true;

                // 有点远 稍微走多一步
                for(int i = 0; i < 5 && isBusy(); i++){
                    Sleep(500);
                }
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
                qInfo() << QString("已经锁定 %1").arg("罗蕾莱");

                if(!isBusy()){
                    return true;
                }
                Sleep(250);
                break;
            }

            Sleep(detectBossPeroidMs);
        }

        if(!isBusy()){
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            return true;
        }

        if(!isTraced){
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
        }
        return isTraced;
    }

}

bool MainBackendWorkerNew::sentryConstructPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){

    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "sentryConstruct", "异构武装", errMsg);
    if(!isGeneralPrepared){
        return false;
    }
    // 异构武装往前冲7秒 然后站着判断 超过7秒没出title 就退了
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 14 && isBusy(); i++){
        Sleep(500);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    // 站着 最多等7秒
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
    Sleep(250);
    double similarity;
    int x, y, timeCostMs;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("sentryConstructTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(bossTitle, 0.8, 7*1000, 500, "未能令boss异构武装出现", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }

        return false;
    }
    else{
        // 鼠标中键 锁定
        Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
        qInfo() << QString("已经锁定 %1").arg("异构武装");
        Sleep(250);
        return true;
    }

}

bool MainBackendWorkerNew::fallacyOfNoReturnPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "fallacyOfNoReturn", "无归的谬误", errMsg);
    if(!isGeneralPrepared){
        return false;
    }
    // 往前冲8秒 然后站着判断 超过8秒没出title 就退了
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 16 && isBusy(); i++){
        Sleep(500);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    // 站着 最多等8秒
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
    double similarity;
    int x, y, timeCostMs;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("fallacyOfNoReturnTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(bossTitle, 0.8, 8*1000, 500, "fallacyOfNoReturn_isnot_shown_up", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            //Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            return true;
        }
        qWarning() << QString("can't find fallacyOfNoReturn's title");
        //Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        return false;
    }
    else{
        //Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        // 等待7.5秒再锁定
        for(int i = 0; i < 15 && isBusy(); i++){
            Sleep(500);
        }

        if(!isBusy()) return true;

        // 鼠标中键 锁定
        Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
        qInfo() << QString("lock %1").arg("无归的谬误");
        Sleep(250);
        return true;
    }
}

bool MainBackendWorkerNew::feilianBeringalPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "feilianBeringal", "飞廉之猩", errMsg);
    if(!isGeneralPrepared){
        return false;
    }
    // 往前冲8秒  然后略微向右前方偏移
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 10 && isBusy(); i++){
        Sleep(500);
        if(i % 5 == 0){
            Utils::sendKeyToWindow(Utils::hwnd, 'D', WM_KEYDOWN);
            Sleep(300);
            Utils::sendKeyToWindow(Utils::hwnd, 'D', WM_KEYUP);
        }
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);

    if(!isBusy()){
        Sleep(250);
        return true;
    }

    // 斜向左前跑 4秒判断
    const int maxRunFindBossMs = 6*1000;
    const int detectBossPeroidMs = 500;

    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("feilianBeringalTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;

    QElapsedTimer timer;
    timer.start();

    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    Utils::sendKeyToWindow(Utils::hwnd, 'A', WM_KEYDOWN);
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        int x, y;
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Utils::sendKeyToWindow(Utils::hwnd, 'A', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'A', WM_KEYUP);
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("已经锁定 %1").arg("飞廉之猩");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Utils::sendKeyToWindow(Utils::hwnd, 'A', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Utils::sendKeyToWindow(Utils::hwnd, 'A', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;

    /*
    // 旧 傻跑
    // 然后向左前方
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    Utils::sendKeyToWindow(Utils::hwnd, 'A', WM_KEYDOWN);
    for(int i = 0; i < 6 && isBusy(); i++){
        Sleep(500);
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
    Utils::sendKeyToWindow(Utils::hwnd, 'A', WM_KEYUP);

    if(!isBusy()){
        Sleep(250);
        return true;
    }
    */

}

bool MainBackendWorkerNew::impermanenceHeronPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "impermanenceHeron", "无常凶鹭", errMsg);
    if(!isGeneralPrepared){
        return false;
    }
    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();

    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("impermanenceHeronTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        int x, y;
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("已经锁定 %1").arg("无常凶鹭");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::infernoRiderPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "infernoRider", "燎照之骑", errMsg);
    if(!isGeneralPrepared){
        return false;
    }
    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();

    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("infernoRiderTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        int x, y;
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            // 找到boss title了
            isTraced = true;

            for(int i = 0; i < 6 && isBusy(); i++){
                Sleep(500);
            }
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇燎照之骑");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::lampylumenMyriadPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "lampylumenMyriad", "辉萤军势", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("lampylumenMyriadTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("辉萤军势");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLocklampylumenMyriad");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::mechAbominationPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "mechAbomination", "聚械机偶", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // W按住 500ms后跳一次 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    Sleep(300);
    Utils::keyPress(Utils::hwnd, VK_SPACE, 1);
    for(int i = 0; i < 24 && isBusy(); i++){
        if(i==7){
            // 平A落地
            Utils::clickWindowClient(Utils::hwnd);
        }
        Sleep(500);
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);

    if(!isBusy()){
        Sleep(250);
        return true;
    }

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 14*1000;
    const int detectBossPeroidMs = 250;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("mechAbominationTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Sleep(250);
                return true;
            }

            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("mechAbomination");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Sleep(250);
        return true;
    }

    if(!isTraced){
        // 机械局偶是 角色原地罚站 不需要松开W
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockMourningAix");
        Sleep(250);
    }
    return isTraced;

}

bool MainBackendWorkerNew::mourningAixPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "mourningAix", "哀声鹫", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("mourningAixTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("mourningAix");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockMourningAix");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

// 云闪之麟
bool MainBackendWorkerNew::tempestMephisPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "tempestMephis", "云闪之麟", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("tempestMephisTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("云闪之麟");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockTempestMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}
// 朔雷之麟
bool MainBackendWorkerNew::thunderingMephisPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "thunderingMephis", "朔雷之麟", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("thunderingMephisTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("朔雷之麟");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::bellBorneGeochelonePreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "bellBorneGeochelone", "钟鸣之龟", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // W按住 向前走6秒
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 12 && isBusy(); i++){
        Sleep(500);
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);

    if(!isBusy()){
        Sleep(250);
        return true;
    }


    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 20*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("bellBorneGeocheloneTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Sleep(250);
                return true;
            }

            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("钟鸣之龟");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockBellBorneGeochelone");
        Sleep(250);
    }
    return isTraced;
}


bool MainBackendWorkerNew::nightmareCrownlessPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "nightmareCrownless", "梦魇无冠者", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("nightmareCrownlessTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            // 找到boss title了
            isTraced = true;

            for(int i = 0; i < 5 && isBusy(); i++){
                Sleep(500);
            }
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇无冠者");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::nightmareFeilianBeringalPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "nightmareFeilianBeringal", "梦魇飞廉之猩", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("nightmareFeilianBeringalTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            // 找到boss title了
            isTraced = true;
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇飞廉之猩");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::nightmareImpermanenceHeronPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "nightmareImpermanenceHeron", "梦魇无常凶鹭", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("nightmareImpermanenceHeronTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            // 找到boss title了
            isTraced = true;

            for(int i = 0; i < 5 && isBusy(); i++){
                Sleep(500);
            }
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇无常凶鹭");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::nightmareInfernoRiderPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "nightmareInfernoRider", "梦魇燎照之骑", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("nightmareInfernoRiderTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            // 找到boss title了
            isTraced = true;

            for(int i = 0; i < 5 && isBusy(); i++){
                Sleep(500);
            }
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇燎照之骑");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}


bool MainBackendWorkerNew::nightmareMourningAixPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "nightmareMourningAix", "梦魇哀声鸷", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("nightmareMourningAixTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            // 找到boss title了
            isTraced = true;

            for(int i = 0; i < 5 && isBusy(); i++){
                Sleep(500);
            }
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇哀声鸷");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::nightmareTempestMephisPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "nightmareTempestMephis", "梦魇云闪之鳞", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("nightmareTempestMephisTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            // 找到boss title了
            isTraced = true;

            for(int i = 0; i < 5 && isBusy(); i++){
                Sleep(500);
            }
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇云闪之鳞");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::nightmareThunderingMephisPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg){
    bool isGeneralPrepared = echoList2bossPositionPreparation(normalBossSetting, "nightmareThunderingMephis", "梦魇朔雷之鳞", errMsg);
    if(!isGeneralPrepared){
        return false;
    }

    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);

    // 如果跑了 N秒没找到 认为失败 跳过
    const int maxRunFindBossMs = 10*1000;
    const int detectBossPeroidMs = 500;

    QElapsedTimer timer;
    timer.start();
    int x, y;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("nightmareThunderingMephisTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    bool isTraced = false;
    while(timer.elapsed() < maxRunFindBossMs && isBusy()){
        double similarity;
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, bossTitle, 0.8, x, y, similarity)){
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }

            // 找到boss title了
            isTraced = true;

            for(int i = 0; i < 5 && isBusy(); i++){
                Sleep(500);
            }
            if(!isBusy()){
                Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
                Sleep(250);
                return true;
            }
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(250);
            Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
            qInfo() << QString("locked %1").arg("梦魇朔雷之鳞");
            Sleep(250);
            break;
        }

        Sleep(detectBossPeroidMs);
    }

    if(!isBusy()){
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(!isTraced){
        Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "cannotLockThunderingMephis");
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
    }
    return isTraced;
}

bool MainBackendWorkerNew::dreamlessPreparation(const SpecialBossSetting &specialBossSetting, QString& errMsg){
    // 向前走3秒 然后锁定找title 锁定boss
    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 5 && isBusy(); i++){
        Sleep(500);
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);

    if(!isBusy()){
        return true;
    }

    // 1秒没找到boss的title 认为失败 跳过
    const int maxRunFindBossMs = 1000;
    const int detectBossPeroidMs = 200;
    int timeCost = 0;

    int x, y;
    double similarity;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("dreamlessTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(loopFindPic(bossTitle, 0.8, maxRunFindBossMs, detectBossPeroidMs, "未能找到无妄者的title", similarity, x, y, timeCost)){
        Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
        qInfo() << QString("locked %1").arg("无妄者");
        Sleep(250);
        return true;
    }
    // 可能是没找到或被用户打断
    if(!isBusy()){
        return true;
    }

    // 没找到 记录错误
    Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "未能找到无妄者的title");
    return false;
}

bool MainBackendWorkerNew::hecatePreparation(const SpecialBossSetting &specialBossSetting, QString& errMsg){
    // 向前走3秒 然后锁定找title 锁定boss
    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 4 && isBusy(); i++){
        Sleep(500);
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);

    if(!isBusy()){
        return true;
    }

    // 1秒没找到boss的title 认为失败 跳过
    const int maxRunFindBossMs = 1000;
    const int detectBossPeroidMs = 200;
    int timeCost = 0;

    int x, y;
    double similarity;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("hecateTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(loopFindPic(bossTitle, 0.8, maxRunFindBossMs, detectBossPeroidMs, "未能找到赫卡忒的title", similarity, x, y, timeCost)){
        Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
        qInfo() << QString("locked %1").arg("赫卡忒");
        Sleep(250);
        return true;
    }
    // 可能是没找到或被用户打断
    if(!isBusy()){
        return true;
    }

    // 没找到 记录错误
    Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "未能找到赫卡忒的title");
    return false;
}

bool MainBackendWorkerNew::juePreparation(const SpecialBossSetting &specialBossSetting, QString& errMsg){
    // 向前走3秒 然后锁定找title 锁定boss
    // 相对简单 直接向前冲 W按住 循环判断有无特定boss名字
    /*
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 4 && isBusy(); i++){
        Sleep(500);
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);

    if(!isBusy()){
        return true;
    }
    */

    // 1秒没找到boss的title 认为失败 跳过
    const int maxRunFindBossMs = 3000;
    const int detectBossPeroidMs = 200;
    int timeCost = 0;

    int x, y;
    double similarity;
    cv::Mat bossTitle = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("jueTitle").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(loopFindPic(bossTitle, 0.8, maxRunFindBossMs, detectBossPeroidMs, "未能找到角的title", similarity, x, y, timeCost)){
        Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);
        qInfo() << QString("locked %1").arg("角");
        Sleep(250);
        return true;
    }
    // 可能是没找到或被用户打断
    if(!isBusy()){
        return true;
    }

    // 没找到 记录错误
    Utils::saveDebugImg(Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd)), cv::Rect(), x, y, "未能找到角的title");
    return false;
}


bool MainBackendWorkerNew::specialBossFightPickupEcho(const SpecialBossSetting &specialBossSetting, const QString& bossEnName, QString& errMsg){
    // 保证已经锁定了boss
    // 战斗代码
    QElapsedTimer timer;
    timer.start();
    emit startFight();

    // 每隔250ms判断boss状态
    // 能检测到背包图标 检测不到bosstitle 说明boss 死了 停止战斗
    // 能检测到背包图标 检测得到bosstitle 说明boss还活着 继续锤 恢复战斗
    // 检测不到背包图标 检测不到bosstitle 说明在放大招 暂停战斗
    cv::Mat bossTitle = cv::imread(QString("%1/%2Title.bmp").arg(Utils::IMAGE_DIR_EI()).arg(bossEnName).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int titleX, titleY;
    double similarityTitle;

    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int bagX, bagY;
    double similarityBag;
    int voteBossDead = 0;
    int voteIamDead = 0;
    while(isBusy() && timer.elapsed() < maxFightMs){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        bool isFindTitle = Utils::findPic(capImg, bossTitle, 0.6, titleX, titleY, similarityTitle);
        bool isFindBag = Utils::findPic(capImg, bagImg, 0.7, bagX, bagY, similarityBag);

        if(isFindBag && isFindTitle){
            voteBossDead = 0;
            voteIamDead = 0;
            m_fightBackendWorkerNew.resumeWorker();
        }
        else if(isFindBag && !isFindTitle){
            voteBossDead = voteBossDead + 1;
            voteIamDead = 0;
            m_fightBackendWorkerNew.pauseWorker();
            if(voteBossDead > 10){
                m_fightBackendWorkerNew.stopWorker();
                Utils::middleClickWindowClientArea(Utils::hwnd, 1, 1);  // 回正视角 面向声骸
                //Utils::saveDebugImg(capImg, cv::Rect(), 0 ,0, QString("认为boss死亡_背包匹配度_%1__boss标题匹配度_%2").arg(similarityBag).arg(similarityTitle));
                Sleep(1000);
                qInfo() << QString("战斗完成");
                break;
            }
        }
        else if(!isFindBag && !isFindTitle){
            voteIamDead = voteIamDead + 1;
            voteBossDead = 0;
            m_fightBackendWorkerNew.pauseWorker();
            if(voteIamDead > 40){
                // 大招播放动画最长也就2秒
                // 可能需要复苏 + 跳过月卡
                skipMonthCard();
                // ##### 单刷boss的复苏怎么处理
                if(!reviveSpecialBoss()){
                    qWarning() << QString("复活失败");
                    m_fightBackendWorkerNew.stopWorker();
                    return false;
                }
                else{
                    qInfo() << QString("复活成功");
                    m_fightBackendWorkerNew.stopWorker();
                    return true;
                }


                //m_fightBackendWorkerNew.stopWorker();
                //return true;
            }
        }
        else{
            //
        }

        Sleep(50);
    }
    m_fightBackendWorkerNew.stopWorker();
    // 可能需要复苏 + 跳过月卡
    skipMonthCard();

    if(!isBusy()){
        m_fightBackendWorkerNew.stopWorker();
        return true;
    }

    if(timer.elapsed() >= maxFightMs){
        m_fightBackendWorkerNew.stopWorker();
        return true;
    }

    // 拾取声骸  记得计数+1
    // 切换3号位 防止椿不会动
    Utils::keyPress(Utils::hwnd, '3', 1);
    //reviveSpecialBoss();
    cv::Mat absorbMat = cv::imread(QString("%1/absorb.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    Sleep(50);
    int absorbX, absorbY;
    double absorbSimilarity;
    bool isAbsorb = false;
    for(int i = 0; i < 15 && isBusy() && !isAbsorb; i++){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        if(Utils::findPic(capImg, absorbMat, 0.8, absorbX, absorbY, absorbSimilarity)){
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
            Sleep(1500);
            //先停下来 再次判断 点击准确
            isAbsorb = true;
            capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            if(Utils::findPic(capImg, absorbMat, 0.8, absorbX, absorbY, absorbSimilarity)){
                // ALT 左键 ALT松
                Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
                Sleep(500);
                Utils::moveMouseToClientArea(Utils::hwnd, absorbX + absorbMat.cols / 2, absorbY + absorbMat.rows / 2 );
                Sleep(500);
                //Utils::saveDebugImg(capImg, cv::Rect(), absorbX + absorbMat.cols / 2, absorbY + absorbMat.rows / 2, "pickUpEcho");
                Utils::clickWindowClientArea(Utils::hwnd, absorbX + absorbMat.cols / 2, absorbY + absorbMat.rows / 2 );
                Sleep(500);
                Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
                Sleep(3000);
                break;
            }
            else{
                Sleep(250);
                return true;
            }
        }
        Sleep(250);
    }
    if(!isBusy()){
        //Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
        //Sleep(250);
        Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        Sleep(250);
        return true;
    }

    if(isAbsorb) {pickUpNormalBossEcho = pickUpNormalBossEcho + 1;}
    qInfo() << QString("pickUpEchoNumber %1").arg(pickUpNormalBossEcho);

    return true;
}

bool MainBackendWorkerNew::repeatBattle(const SpecialBossSetting &specialBossSetting, QString& errMsg){
    qInfo() << QString("准备重新挑战");
    cv::Mat repeatChallenge = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("repeat").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat repeatChallengeConfirm = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg("repeat2").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);

    bool isFind = false;
    int x, y;
    double similarity;
    int maxWaitMs = 3 * 1000;
    int timeCost = 0;
    // 找到左上角的重新挑战按键
    isFind = loopFindPic(repeatChallenge, 0.7, maxWaitMs, 250, "未能找到重新挑战按钮", similarity, x, y, timeCost);  // 从0.8 降低为0.7  防止jue找不到
    if(!isBusy()){
        return true;
    }

    if(!isFind){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        errMsg = QString("未能在左上角找到重新挑战按钮");
        Utils::saveDebugImg(capImg, cv::Rect(), 0,0, "未能在左上角找到重新挑战按钮");
        return false;
    }

    // 点击它
    Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + repeatChallenge.cols /2, y + repeatChallenge.rows /2 );
    Sleep(1000);
    Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);

    if(!isBusy()){
        return true;
    }

    // 找到黑色重新挑战按键
    isFind = loopFindPic(repeatChallengeConfirm, 0.9, maxWaitMs, 250, "未能找到重新挑战按钮", similarity, x, y, timeCost);
    if(!isBusy()){
        return true;
    }

    if(!isFind){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        errMsg = QString("未能在右下角找到重新挑战按钮");
        Utils::saveDebugImg(capImg, cv::Rect(), 0,0, "未能在右下角找到重新挑战按钮");
        return false;
    }

    // 点击它
    Utils::clickWindowClientArea(Utils::hwnd, x + repeatChallengeConfirm.cols /2, y + repeatChallengeConfirm.rows /2 );
    Sleep(1000);

    if(!isBusy()){
        return true;
    }

    return true;

}

bool MainBackendWorkerNew::echoList2bossPositionPreparation(const NormalBossSetting &normalBossSetting, \
                                                            const QString& bossEnName, \
                                                            const QString& bossChName, QString& errMsg){

    // 现在是在残像探寻列表
    // 移动到boss列表
    Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListPos.x, scrollEchoListPos.y);
    qInfo() << QString("寻找 %1 图标").arg(bossChName);
    QElapsedTimer timer;
    timer.start();
    // 匹配、滚动；最多允许时间 20S 必须能在残像探寻找到boss icon 否则认为失败
    int maxWaitMs = 20 * 1000;
    cv::Mat bossIcon = cv::imread(QString("%1/%2.bmp").arg(Utils::IMAGE_DIR_EI()).arg(bossEnName).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    int x, y;
    double similarity;
    bool findBossIcon = false;
    while(timer.elapsed() < maxWaitMs && isBusy()){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        //findBossIcon = Utils::findPic(capImg, bossIcon, 0.85, x, y, similarity);
        findBossIcon = Utils::findPic(capImg, bossIcon, 0.88, x, y, similarity);   // 严格区分梦魇 和普通
        if(!findBossIcon){
            this->dragWindowClient3(Utils::hwnd, scrollEchoListsStartPos.x, scrollEchoListsStartPos.y, \
                                    scrollEchoListsEndPos.x, scrollEchoListsEndPos.y, \
                                    20, 20);
            Sleep(500);
            Utils::moveMouseToClientArea(Utils::hwnd, scrollEchoListsWaitPos.x, scrollEchoListsWaitPos.y);
            Sleep(1000);
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
    if(!loopFindPic(confirm, 0.55, defaultMaxWaitMs, defaultRefreshMs, "未能找到boss确认信息", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍未确认点击 %2 的icon").arg(defaultMaxWaitMs).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }
    if(!isBusy()){
        return true;
    }

    // 尝试点击探测
    cv::Mat detect = cv::imread(QString("%1/detect.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(detect, 0.9, defaultMaxWaitMs, defaultRefreshMs, "未能找到boss的探测", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍找到 %2 的探测").arg(defaultMaxWaitMs).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }
    if(!isBusy()){
        return true;
    }
    Sleep(500);
    Utils::clickWindowClientArea(Utils::hwnd, x + detect.cols/2, y + detect.rows/2);

    // 尝试点击快速旅行
    cv::Mat fastTravel = cv::imread(QString("%1/fastTravel.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(fastTravel, 0.9, defaultMaxWaitMs, defaultRefreshMs, "未能找到boss的快速旅行", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍未找到 %2 的快速旅行").arg(defaultMaxWaitMs).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }
    if(!isBusy()){
        return true;
    }
    Sleep(1500);   //多等一下再点快速旅行！ 有的boss可能加载比较慢
    Utils::clickWindowClientArea(Utils::hwnd, x + fastTravel.cols/2, y + fastTravel.rows/2);

    // 不断找背包 找到说明加载完毕  这里加载可能时间稍长 允许15s
    cv::Mat bagImg = cv::imread(QString("%1/bag.bmp").arg(Utils::IMAGE_DIR_EI()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(!loopFindPic(bagImg, 0.8, 20000, defaultRefreshMs, "传送boss 加载超时", similarity, x, y, timeCostMs)){
        if(!isBusy()){
            return true;
        }
        errMsg = QString("超时, %1 ms后仍未完成 %2 的传送游戏加载").arg(10000).arg(bossChName);
        qWarning() << errMsg;
        return false;
    }
    if(!isBusy()){
        return true;
    }

    skipMonthCard();
    // 切换3号位 方便往前走的时候固定成人速度
    Utils::keyPress(Utils::hwnd, '3', 1);
    revive();


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

            if((singleEchoSetting.isLockJudge && isLock) ||
                    (singleEchoSetting.isDiscardedJudge && isDiscard) ||
                    ((singleEchoSetting.isNormalJudge) && (!isDiscard && !isLock)) ){
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
            //qWarning() << "break by user. Similarity:" << similarity;
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
    //qWarning() << "Failed to find template within the specified time limit. Similarity:" << similarity;
    return false;

}

void MainBackendWorkerNew::skipMonthCard(){
    // 获取当前时间
    QTime currentTime = QTime::currentTime();

    // 定义时间范围
    QTime startTime(3, 00); // 03:55
    QTime endTime(4, 5);    // 04:05

    if(isFirstSkipMonthCard){
        // 已经进行了一次月卡跳过了
        // 判断当前时间是否在范围内
        if (!(currentTime >= startTime && currentTime <= endTime) ) {
            return;
        }
    }

    // 必须进行一次判断
    isFirstSkipMonthCard = true;

    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    cv::Mat monthCardImg = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("monthCard.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED );
    double similarity;
    int x, y;
    // 找有没有月卡图案
    if(!Utils::findPic(capImg, monthCardImg, 0.8, x, y, similarity)){
        qWarning() << QString("NO MONTH CARD FOUND %1 < 0.8").arg(similarity);
        return;
    }

    // 点击月卡图案
    Utils::moveMouseToClientArea(Utils::hwnd, x + monthCardImg.cols/2, y + monthCardImg.rows/2);
    Sleep(250);
    Utils::clickWindowClientArea(Utils::hwnd, x + monthCardImg.cols/2, y + monthCardImg.rows/2);
    Sleep(250);
    cv::Mat monthCardReward = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("monthCardReward.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED );
    bool isFoundReward = false;   // 是否找到了
    int timeCostMs = 0;
    while(isBusy() && !isFoundReward && timeCostMs < 2000){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        isFoundReward = Utils::findPic(capImg, monthCardReward, 0.8, x, y);
        timeCostMs += 250;
    }

    // 找到了奖励图标
    if(isFoundReward){
        Utils::moveMouseToClientArea(Utils::hwnd, x + monthCardImg.cols/2, y + monthCardImg.rows/2);
        Sleep(250);
        Utils::clickWindowClientArea(Utils::hwnd, 630, 560);
        Sleep(250);
    }

    return;
}

bool MainBackendWorkerNew::revive(){
    //qInfo() << QString("尝试复苏");
    cv::Mat tpRevive1 = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("tpRevive1.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat tpRevive2 = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("tpRevive2.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat tp = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("fastTravel.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat noTakeReviveMedi = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("noTakeReviveMedi.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    cv::Mat bagImg = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("bag.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);

    //cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    double similarity;
    int x, y;
    int timeCostMs;
    // 等待速切弹出复活药界面
    if(!loopFindPic(noTakeReviveMedi, 0.9, defaultMaxWaitMs, defaultRefreshMs, "复苏等待默认时长后仍未弹出复活药画面", similarity, x, y, timeCostMs)){
        //cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        //qWarning() << "复苏等待默认时长后仍未弹出复活药画面";
        //Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "复苏等待默认时长后仍未弹出复活药画面");
        return false;
    }

    // 关闭复活药界面
    Sleep(defaultRefreshMs * 2);
    Utils::clickWindowClientArea(Utils::hwnd, x + noTakeReviveMedi.cols / 2, y + noTakeReviveMedi.rows / 2);

    if(!loopFindPic(bagImg, 0.8, defaultMaxWaitMs, defaultRefreshMs, "关闭复活药界面仍未找到背包", similarity, x, y, timeCostMs)){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "关闭复活药界面仍未找到背包");
        qWarning() << "关闭复活药界面仍未找到背包";
        return false;
    }

    // 准备按M
    Sleep(defaultRefreshMs * 2);
    int tpRevive = 0;  // 1表示找的是小信标， 2表示找的是中枢信标
    cv::Point tpPos;
    Utils::keyPress(Utils::hwnd, 'M', 1);
    if(!loopFindPic(tpRevive1, 0.8, defaultMaxWaitMs, defaultRefreshMs, "打开地图未发现传送信标小", similarity, x, y, timeCostMs)){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "打开地图未发现传送信标小");
        if(!loopFindPic(tpRevive2, 0.8, defaultMaxWaitMs, defaultRefreshMs, "打开地图未发现传送信标大", similarity, x, y, timeCostMs)){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "打开地图未发现传送信标大");
            qWarning() << "打开地图未发现传送信标大";
            return false;
        }
        tpRevive = 2;
        tpPos = cv::Point(x + tpRevive2.cols / 2, y + tpRevive2.rows / 2);
    }
    tpRevive = 1;
    tpPos = cv::Point(x + tpRevive1.cols / 2, y + tpRevive1.rows / 2);

    Sleep(defaultRefreshMs * 4);
    Utils::clickWindowClientArea(Utils::hwnd, tpPos.x, tpPos.y);

    // 点击后 可能出现两者重叠 需要点多一次
    if(tpRevive == 1){
        Sleep(defaultRefreshMs * 4);
        // 可能重叠了
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        cv::Mat tpRevive1check = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("tpRevive1checkText.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        // 应该能找到 小型信标这几个汉字
        if(!Utils::findPic(capImg, tpRevive1check, 0.8, x, y, similarity)){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "复活信标重叠无法找到小型信标这几个汉字");
            qInfo() << "复活信标重叠无法找到小型信标这几个汉字";
            //return false;
        }
        else{
            // 点击 小型信标汉字
            Sleep(defaultRefreshMs * 4);
            Utils::clickWindowClientArea(Utils::hwnd, x + tpRevive1check.cols/2, y + tpRevive1check.rows/2);
        }

    }


    // 应该能找到快速旅行
    if(!loopFindPic(tp, 0.9, defaultMaxWaitMs, defaultRefreshMs, "点击传送信标未找到快速旅行", similarity, x, y, timeCostMs)){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "点击传送信标未找到快速旅行");
        qWarning() << "点击传送信标未找到快速旅行";
        return false;
    }
    Sleep(defaultRefreshMs * 4);
    Utils::clickWindowClientArea(Utils::hwnd, x + tp.cols / 2, y + tp.rows / 2);

    // 等待找到背包
    if(!loopFindPic(bagImg, 0.8, defaultMaxWaitMs * 10, defaultRefreshMs, "点击快速旅行未找到背包", similarity, x, y, timeCostMs)){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "点击快速旅行未找到背包");
        qWarning() << "点击快速旅行未找到背包";
        return false;
    }

    // 完成复活
    return true;
}


bool MainBackendWorkerNew::reviveSpecialBoss(){
    qInfo() << QString("尝试复苏 特殊boss副本");
    // 停止战斗线程 等死
    m_fightBackendWorkerNew.stopWorker();

    cv::Mat noTakeReviveMedi = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("noTakeReviveMedi.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    double similarity;
    int x, y;
    int timeCostMs;
    // 等待速切弹出复活药界面
    if(!loopFindPic(noTakeReviveMedi, 0.9, defaultMaxWaitMs, defaultRefreshMs, "复苏等待默认时长后仍未弹出复活药画面", similarity, x, y, timeCostMs)){
        //cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        //qWarning() << "复苏等待默认时长后仍未弹出复活药画面";
        //Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "复苏等待默认时长后仍未弹出复活药画面");
        return false;
    }


    // 关闭复活药界面
    Sleep(defaultRefreshMs * 2);
    Utils::clickWindowClientArea(Utils::hwnd, x + noTakeReviveMedi.cols / 2, y + noTakeReviveMedi.rows / 2);

    // 等待 直到看到重新挑战
    cv::Mat reChallenge = cv::imread(QString("%1/%2").arg(Utils::IMAGE_DIR_EI()).arg("reviveSpecialBoss.bmp").toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);

    if(!loopFindPic(reChallenge, 0.9, 20000, defaultRefreshMs, "等了20秒还没被boss打死", similarity, x, y, timeCostMs)){
        //cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        //qWarning() << "复苏等待默认时长后仍未弹出复活药画面";
        //Utils::saveDebugImg(capImg, cv::Rect(), 0, 0, "复苏等待默认时长后仍未弹出复活药画面");
        return false;
    }

    Sleep(defaultRefreshMs * 2);
    Utils::clickWindowClientArea(Utils::hwnd, x + reChallenge.cols / 2, y + reChallenge.rows / 2);
    Sleep(250);

    return true;
}


























