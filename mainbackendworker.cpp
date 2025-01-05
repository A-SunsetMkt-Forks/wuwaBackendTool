#include "mainbackendworker.h"

QTime MainBackendWorker::rebootCalcStartTime;
QTime MainBackendWorker::rebootCalcEndTime;
QTime MainBackendWorker::dungeonStartTime;
QTime MainBackendWorker::dungeonEndTime;
QAtomicInt MainBackendWorker::isContinueFight;
QTime MainBackendWorker::battlePickUpTime;  // 战斗时按拾取开始时间
QAtomicInt MainBackendWorker::reviveVal; // 复苏变量
QAtomicInt MainBackendWorker::startNoSwitchFight;
QAtomicInt MainBackendWorker::isPickUp;
QAtomicInt MainBackendWorker::rebootCount;
std::atomic<float> MainBackendWorker::absorbThres;
QAtomicInt MainBackendWorker::monthCardJudge;

QAtomicInt MainBackendWorker::pickUpEchoJudgeLeftTarget;  // 捡声骸后判断左边的目标
QAtomicInt MainBackendWorker::pickUpEchoJudgeBossHpBar;  // 捡声骸后判断boss血条
QAtomicInt MainBackendWorker::pickUpEchoJudgeRestartFight;  // 捡声骸后判断boss血条

// 实现部分
MainBackendWorker::MainBackendWorker(QObject* parent)
    : QObject(parent)  // 初始化基类 QObject
{
    m_isRunning.store(0);

    // 启动后台线程
    m_fastSwitchFightBackendWorker.moveToThread(&m_fastSwitchFightBackendThread);
    m_fastSwitchFightBackendThread.start();

    m_noSwitchFightBackendWorker.moveToThread(&m_noSwitchFightBackendThread);
    m_noSwitchFightBackendThread.start();


    connect(this, &MainBackendWorker::startNoSwitchFightWorker, &m_noSwitchFightBackendWorker, &NoSwitchFightBackendWorker::startFight);
    connect(this, &MainBackendWorker::startFastSwitchFightWorker, &m_fastSwitchFightBackendWorker, &FastSwitchFightBackendWorker::startFight);

}

MainBackendWorker::~MainBackendWorker()
{
    // 停止后台线程
    m_fastSwitchFightBackendWorker.stopWorker();
    m_fastSwitchFightBackendThread.quit();
    m_fastSwitchFightBackendThread.wait();

    m_noSwitchFightBackendWorker.stopWorker();
    m_noSwitchFightBackendThread.quit();
    m_noSwitchFightBackendThread.wait();


}

bool MainBackendWorker::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}


void MainBackendWorker::stopWorker(){
    m_isRunning.store(0);
    m_fastSwitchFightBackendWorker.stopWorker();
    m_noSwitchFightBackendWorker.stopWorker();
}

void MainBackendWorker::skipMonthCard(){
    QDateTime dateTime = QDateTime::currentDateTime();

    if(dateTime.time().hour() == 4 && dateTime.time().minute() >= 0 && dateTime.time().minute() <= 5 && MainBackendWorker::monthCardJudge.load() == 0){
        //如果时间在4点00到4点05，并且还没有执行过 月卡跳过
        // 循环5次
        for(int i = 0; i < 5 && isBusy(); i++){
            qInfo() << QString("");
            // 按下ALT
            Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYDOWN);
            Sleep(200);
            for(int jClick = 0; jClick < 2; jClick++){
                Utils::clickWindowClientArea(Utils::hwnd, 627, 602);
                Sleep(200);
            }
            // 松开ALT
            Utils::sendKeyToWindow(Utils::hwnd, VK_MENU, WM_KEYUP);
        }
        if(!isBusy()){
            return;
        }
        MainBackendWorker::monthCardJudge.store(1);
    }
}

void MainBackendWorker::pickUpEchoIsBossDied(){

}

bool MainBackendWorker::quitRover(){
    qInfo() << "退出无妄者";
    for(int i = 0; i < 5 && isBusy(); i++){
        Utils::keyPress(Utils::hwnd, VK_ESCAPE, 1);
        Sleep(100);

        for(int j = 0; j < 5 && isBusy(); j++){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            int x, y;
            bool isFindPic = Utils::findPic(capImg, \
                                            cv::imread(QString("%1/副本打完boss重新挑战.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED), \
                                            0.8, x, y);   // 755 423 959 479
            if(isFindPic){
                Sleep(500);
                break;
            }
        }
        if(!isBusy()){
            return true;
        }

        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        int x, y;
        bool isFindPic = Utils::findPic(capImg, \
                                        cv::imread(QString("%1/副本打完boss重新挑战.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED), \
                                        0.8, x, y);  // 755 423 959 479
        if(isFindPic){
            for(int j = 0; j < 5 && isBusy();j++){
                Sleep(100);
                Utils::clickWindowClientArea(Utils::hwnd, 858, 448);
            }
            if(!isBusy()){
                return true;
            }
        }
        else{
            skipMonthCard();
            for(int j = 0; j < 3 && isBusy(); j++){
                Sleep(300);
                Utils::clickWindowClientArea(Utils::hwnd, 623, 573);
            }
            if(!isBusy()){
                return true;
            }
        }

    }
    if(!isBusy()){
        return true;
    }

    return true;
}

void MainBackendWorker::onStartTest1(){
    // 启动程序 UI线程 发信号后 已经更改为忙碌了
    // 不管任何方式退出 都发送信号要求UI更新状态
    m_isRunning.store(1);

    qDebug() << QString("MainBackendWorker::onStartTest1 start...");

    // 初始化句柄
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        emit startTest1Done(false, QString("未能初始化鸣潮窗口句柄"));
        m_isRunning.store(0);
        return;
    }

    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 将线程附加到目标窗口
    // 测试 ESC 点击活动 ESC 往前走
    if (AttachThreadInput(currentThreadId, threadId, TRUE)) {
        // 激活窗口
        SendMessage(Utils::hwnd, WM_ACTIVATE, WA_ACTIVE, 0);   // 只要这行可以 只在一开始弹出 后续放到后台也可以传
        Sleep(1000);
        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 切换2人物
        Utils::keyPress(Utils::hwnd, '2', 1);

        // 按下ESC
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYDOWN);
        Sleep(300);
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYUP);
        Sleep(500);

        // 检测是否用户要求停止
        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 点击ESC菜单中的 活动
        Sleep(1000);   //等待ESC菜单展开
        Utils::clickWindowClientArea(Utils::hwnd, 641, 145);
        Sleep(500);

        // 检测是否用户要求停止
        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 按下ESC
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYDOWN);
        Sleep(500);

        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYUP);
        Sleep(500);

        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 按下ESC
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYDOWN);
        Sleep(500);

        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYUP);
        Sleep(500);

        // 按住W 2秒
        for(int i = 0; i < 4 && isBusy(); i++){
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
            Sleep(500);
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        }

        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 分离线程
        AttachThreadInput(currentThreadId, threadId, FALSE);
        emit startTest1Done(true, QString("正常结束"));
        m_isRunning.store(0);
        return;
    }
    else {
        emit startTest1Done(false, QString("线程附加到鸣潮窗口句柄失败"));
        m_isRunning.store(0);
        qWarning() << "Failed to attach thread input.";
        return;
    }
}


void MainBackendWorker::onStartSpecialBoss(const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting){
    // 启动程序 UI线程 发信号后 已经更改为忙碌了
    // 不管任何方式退出 都发送信号要求UI更新状态
    m_isRunning.store(1);

    qDebug().noquote() << setting.toQString();
    qInfo() << QString("MainBackendWorker::onStartSpecialBoss start...");

    // 初始化句柄
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        emit startSpecialBossDone(false, QString("未能初始化鸣潮窗口句柄"), setting, rebootGameSetting);
        m_isRunning.store(0);
        return;
    }

    bool isWuwaRunning = Utils::isWuwaRunning();
    if(!isWuwaRunning){
        emit startSpecialBossDone(false, QString("未能初始化鸣潮窗口句柄"), setting, rebootGameSetting);
        m_isRunning.store(0);
        return;
    }

    qInfo() << QString("特殊BOSS设置 %1").arg(setting.toQString());
    qInfo() << QString("重启游戏设置 %1").arg(rebootGameSetting.toQString());

    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 每一步都需要执行该检查 封装为匿名函数
    auto isSpecialBossClosing = [&](bool isAbort, bool isNormalEnd, const QString& msg, const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting) {
        bool isWuwaRunning = Utils::isWuwaRunning();
        if(!isWuwaRunning){
            // 鸣潮已经被关闭 异常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startSpecialBossDone(false, QString("鸣潮已经被关闭"), setting, rebootGameSetting);
            m_isRunning.store(0);
            return true;
        }

        if(!isBusy()){
            // 用户中止 正常结束
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startSpecialBossDone(true, QString("用户停止"), setting, rebootGameSetting);
            m_isRunning.store(0);
            return true;
        }
        else{
            if(isAbort){
                // 根据输入参数决定是否正常结束
                AttachThreadInput(currentThreadId, threadId, FALSE);
                emit startSpecialBossDone(isNormalEnd, msg, setting, rebootGameSetting);
                m_isRunning.store(0);
                return true;
            }
            else{
                return false;
            }
        }
    };

    // 将线程附加到目标窗口
    // 测试 ESC 点击活动 ESC 往前走
    if (AttachThreadInput(currentThreadId, threadId, TRUE)) {
        // 激活窗口
        SendMessage(Utils::hwnd, WM_ACTIVATE, WA_ACTIVE, 0);   // 只要这行可以 只在一开始弹出 后续放到后台也可以传
        Sleep(500);

        for(int i = 0; i < 10; i++){
            Sleep(500);
            qDebug() << "等待500ms... ";
            if(isSpecialBossClosing(false, true, QString("用户中止脚本"), setting, rebootGameSetting)) return;
        }


        if(isSpecialBossClosing(true, true, QString("脚本运行结束"), setting, rebootGameSetting)) return;
        return;
    }
    else {
        if(isSpecialBossClosing(true, false, QString("用户中止脚本"), setting, rebootGameSetting)) return;
        return;
    }

}


bool MainBackendWorker::isPickUpEcho(const int& pickUpEchoRange){
    // 在按键精灵源码这是一个sub 也就是void函数 没有返回值 389行
    if(!MainBackendWorker::isPickUp.load()){
        for (int i = 0; i < pickUpEchoRange && isBusy(); i++) {
            for(int j = 0; j < 2; j++){
                cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                int findX, findY;
                bool isFindPic = Utils::findPic(capImg, \
                                                cv::imread(QString("%1/720p吸收.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED), \
                                                0.9, findX, findY);
                if(isFindPic){
                    Utils::keyPress(Utils::hwnd, 'F', 2);
                    Utils::keyPress(Utils::hwnd, 'D', 1);
                    Utils::keyPress(Utils::hwnd, 'F', 2);
                    Utils::keyPress(Utils::hwnd, 'A', 1);
                    Utils::keyPress(Utils::hwnd, 'F', 2);
                    Utils::keyPress(Utils::hwnd, 'S', 1);
                    Utils::keyPress(Utils::hwnd, 'F', 2);
                    Utils::keyPress(Utils::hwnd, 'W', 1);

                    Utils::keyPress(Utils::hwnd, 'F', 2);
                    Utils::keyPress(Utils::hwnd, 'S', 1);
                    Utils::keyPress(Utils::hwnd, 'A', 1);
                    Utils::keyPress(Utils::hwnd, 'F', 1);

                    Utils::keyPress(Utils::hwnd, 'W', 2);
                    Utils::keyPress(Utils::hwnd, 'F', 2);
                    Utils::keyPress(Utils::hwnd, 'D', 2);
                    Utils::keyPress(Utils::hwnd, 'F', 2);
                    return true;  // 结束过程
                }

                Sleep(50);
            }
        }
    }

    return true;   // 不关心返回值
}

bool MainBackendWorker::pickupEchoJudgeBossDead(){
    qInfo() << QString("MainBackendWorker::pickupEchoJudgeBossDead 捡声骸后判断boss是否死亡");

    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    int x, y;
    bool isFindPic = Utils::findPic(capImg, \
                                    cv::imread(QString("%1/720bosslv.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED), \
                                    0.9, x, y);

    if(isFindPic){
        pickUpEchoJudgeLeftTarget.store(2);
    }
    else{
        pickUpEchoJudgeLeftTarget.store(0);
    }

    pickUpEchoJudgeRestartFight.store(pickUpEchoJudgeLeftTarget.load());
    qDebug() << QString("捡声骸后判断是否重新战斗 %1").arg(pickUpEchoJudgeRestartFight.load());
}

bool MainBackendWorker::lockEnemy(){
    bool bossName = false;
    bool traceTarget = false;
    bool bossHPbar = false;
    bool forceJudgeBossExist = false;
    bool bossHPbar2 = false;
    bool bossHPbar3 = false;
    // int bossLevel = 0;
    MainBackendWorker::rebootCount.store(0);
    {
        QMutexLocker locker(&m_mutex);
        MainBackendWorker::rebootCalcStartTime = QTime::currentTime();
    }

    Q_UNUSED(bossName);
    Q_UNUSED(forceJudgeBossExist);

    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
    for(int i = 0; i < 80 && isBusy(); i++){
        //Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
        qDebug() << QString("开始确认敌人");
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        int x, y;
        bool isFindTraceColor = Utils::findColorEx(capImg, 0, 168, 47, 217, "F48A94", 0.8, x, y);
        if(isFindTraceColor){
            traceTarget = true;
        }
        else{
            traceTarget = false;
        }
        qDebug() << QString("是否找到BOSS目标？ %1").arg(isFindTraceColor == true ? "是" : "否");

        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        bool isFindBossHpBarColor = Utils::findColorEx(capImg, 800, 24, 848, 63, "F48A94", 0.8, x, y);
        if(isFindBossHpBarColor){
            bossHPbar = true;
        }
        else{
            bossHPbar = false;
        }
        qDebug() << QString("是否找到BOSS血条？ %1").arg(isFindBossHpBarColor == true ? "是" : "否");

        if(traceTarget && bossHPbar){
            // 确定boss存在
            qDebug() << QString("BOSS确定存在");
            Sleep(100);
            Utils::middleClickWindowClientArea(Utils::hwnd, Utils::CLIENT_WIDTH / 2, Utils::CLIENT_HEIGHT / 2);
            break;
        }
        Sleep(200);

        Utils::keyPress(Utils::hwnd, VK_SHIFT, 1);
        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        bool isFindBossHpBar2 = Utils::findColorEx(capImg, 430, 20, 480, 60, "44B3FF", 0.9, x, y);
        if(isFindBossHpBar2){
            bossHPbar2 = true;
            qDebug() << "找到boss血条";
        }
        else{
            bossHPbar2 = false;
            qDebug() << "未找到boss血条，检查boss血条是否绑定";
        }

        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        bool isFindBossHpBar3 = Utils::findColorEx(capImg, 800, 24, 848, 63, "0825FF", 0.9, x, y);
        if(isFindBossHpBar3){
            isFindBossHpBar3 = true;
            qDebug() << "找到boss血条";
        }
        else{
            isFindBossHpBar3 = false;
            qDebug() << "未找到boss血条，检查boss血条是否绑定";
        }

        qDebug() << "再次开始判断boss是否真的存在";
        if(traceTarget && bossHPbar2 && bossHPbar3){
            qDebug() << "确认敌人存在";
            Sleep(300);
            Utils::middleClickWindowClientArea(Utils::hwnd, Utils::CLIENT_WIDTH / 2, Utils::CLIENT_HEIGHT / 2);
            break;
        }

        int rebootCalcPastMs = 0;
        {
            QMutexLocker locker(&m_mutex);
            MainBackendWorker::rebootCalcEndTime = QTime::currentTime();
            rebootCalcPastMs = rebootCalcEndTime.msecsTo(rebootCalcStartTime);
        }
        if(rebootCalcPastMs > 60000){
            qWarning() << QString("触发重启游戏条件 已经过了 %1 ms").arg(rebootCalcPastMs);
            // ##### 调用重启游戏
        }

        Sleep(500);
        for (int i = 0; i < 5 && isBusy(); i++) {
            Sleep(300);
            Utils::middleClickWindowClientArea(Utils::hwnd, Utils::CLIENT_WIDTH / 2, Utils::CLIENT_HEIGHT / 2);
        }

        if(!isBusy()){
            break;
        }

        MainBackendWorker::rebootCount.ref();
        skipMonthCard();
        if(MainBackendWorker::rebootCount.load() > 30){
            quitRover();  //退出无妄者
            MainBackendWorker::rebootCount.store(5);
        }
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
    skipMonthCard();
    for(int i = 0; i < 15 && isBusy(); i++){
        Sleep(1000);
    }

    return true;
}

bool MainBackendWorker::noSwitchExp(const float& noSwitchCondition){
    //MainBackendWorker::startNoSwitchFight.store(false);
    // ##### 需要对屏幕截图进行ROI处理
    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
    int colorX, colorY;
    if(Utils::findColorEx(capImg, 439, 23, 479, 60, "44B3FF", 0.8, colorX, colorY)){
        // 1/8 血量
        if (qFuzzyCompare(noSwitchCondition, 0.125f)){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            cv::Mat hpBar = cv::imread(QString("%1/720pboss血条1分8.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
            int x, y;
            bool isFindPicOneEighth = Utils::findPic(capImg, hpBar, 0.8, x, y);  // 440, 30, 440+75, y+55
            if(isFindPicOneEighth){
                MainBackendWorker::startNoSwitchFight.store(false);
            }
            else{
                MainBackendWorker::startNoSwitchFight.store(true);
            }
        }

        if(!isBusy()){
            return true;
        }

        // 2/8 血量
        if (qFuzzyCompare(noSwitchCondition, 0.250f)){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            cv::Mat hpBar = cv::imread(QString("%1/720pboss血条2分8.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
            int x, y;
            bool isFindPicTwoEighth = Utils::findPic(capImg, hpBar, 0.8, x, y);  // 440+48, 30, 440+48+75, y+55
            if(isFindPicTwoEighth){
                MainBackendWorker::startNoSwitchFight.store(false);
            }
            else{
                MainBackendWorker::startNoSwitchFight.store(true);
            }
        }

        if(!isBusy()){
            return true;
        }

        // 3/8 血量
        if (qFuzzyCompare(noSwitchCondition, 0.375f)){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            cv::Mat hpBar = cv::imread(QString("%1/720pboss血条3分8.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
            int x, y;
            bool isFindPicThreeEighth = Utils::findPic(capImg, hpBar, 0.8, x, y);  // 440+48+48, 30, 440+48+48+75, y+55
            if(isFindPicThreeEighth){
                MainBackendWorker::startNoSwitchFight.store(false);
            }
            else{
                MainBackendWorker::startNoSwitchFight.store(true);
            }
        }

        if(!isBusy()){
            return true;
        }

        // 1/2 血量
        if (qFuzzyCompare(noSwitchCondition, 0.500f)){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            cv::Mat hpBar = cv::imread(QString("%1/720pboss血条4分8.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
            int x, y;
            bool isFindPicFourEighth = Utils::findPic(capImg, hpBar, 0.8, x, y);  // 440+48+48+48, 30, 440+48+48+48+75, y+55
            if(isFindPicFourEighth){
                MainBackendWorker::startNoSwitchFight.store(false);
            }
            else{
                MainBackendWorker::startNoSwitchFight.store(true);
            }
        }

        if(!isBusy()){
            return true;
        }

        // 5/8 血量
        if (qFuzzyCompare(noSwitchCondition, 0.625f)){
            cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
            cv::Mat hpBar = cv::imread(QString("%1/720pboss血条5分8.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
            int x, y;
            bool isFindPicFiveEighth = Utils::findPic(capImg, hpBar, 0.8, x, y);  // 440+48+48+48+48, 30, 440+48+48+48+48+75, y+55
            if(isFindPicFiveEighth){
                MainBackendWorker::startNoSwitchFight.store(false);
            }
            else{
                MainBackendWorker::startNoSwitchFight.store(true);
            }
        }

        if(!isBusy()){
            return true;
        }
    }



    return true;
}

bool MainBackendWorker::revive(){
    for(int i = 0; i < 10 && isBusy(); i++){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        cv::Mat reviveImg = cv::imread(QString("%1/复苏.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        int x, y;
        bool isFindRevivePic = Utils::findPic(capImg, reviveImg, 0.7, x, y);  // 257,113,343,180
        if(isFindRevivePic){
            for(int i = 0; i < 5 && isBusy(); i++){
                m_fastSwitchFightBackendWorker.stopWorker();
                m_noSwitchFightBackendWorker.stopWorker();
                Sleep(500);
            }
            if(!isBusy()){
                return true;
            }

            Utils::keyPress(Utils::hwnd, VK_ESCAPE, 1);
            QTime startTime = QTime::currentTime();
            while(isBusy()){
                capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                cv::Mat retryImg = cv::imread(QString("%1/重新挑战.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
                bool isFindRetry = Utils::findPic(capImg, retryImg, 0.8, x, y);  // 750,591,865,644
                if(isFindRetry){
                    for(int i = 0; i < 4 && isBusy(); i++){
                        Sleep(500);
                        Utils::clickWindowClientArea(Utils::hwnd, 807, 610);
                        Sleep(500);
                        Utils::clickWindowClientArea(Utils::hwnd, 857, 450);
                        Sleep(500);
                    }
                    if(!isBusy()){
                        return true;
                    }
                    MainBackendWorker::reviveVal.store(1);
                    break;  // exit while(do)

                }
                QTime endTime = QTime::currentTime();
                if(startTime.msecsTo(endTime) > 120000){
                    quitRover();
                    break;  // exit while(do)
                }
                Sleep(200);
            }
            if(!isBusy()){
                return true;
            }
        }
        Sleep(100);
    }
    return true;
}

bool MainBackendWorker::fight(const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting){
    qInfo() << QString("战斗开始");
    {
        QMutexLocker locker(&m_mutex);
        MainBackendWorker::battlePickUpTime = QTime::currentTime();
    }

    // 将文字吸收作为全局变量
    MainBackendWorker::absorbThres.store(setting.absorbThres);

    qInfo() << "战斗开始";
    // boss未死亡
bossUndead:
    // 检测到应该不切人
detectNoSwitch:

    int i = 0;
    MainBackendWorker::reviveVal.store(0);  // 复苏变量
    MainBackendWorker::startNoSwitchFight.store(0);  // 开启不切人战斗
    bool forceRestartFight = false;  // 强制重新开始战斗
    int detectNoSwitch = 0;     // 检测到应该不切人
    MainBackendWorker::isPickUp.store(0);      // 拾取成功
    MainBackendWorker::isContinueFight.store(0);  // 重复判定继续战斗
    QTime startTime = QTime::currentTime();
    {
        QMutexLocker locker(&m_mutex);
        MainBackendWorker::rebootCalcStartTime = QTime::currentTime();
    }

    if(setting.jumpRecovery){
        Utils::keyPress(Utils::hwnd, '2', 1);
        Utils::keyPress(Utils::hwnd, VK_SPACE, 1);
        Utils::clickWindowClientArea(Utils::hwnd, Utils::CLIENT_WIDTH / 2, Utils::CLIENT_HEIGHT / 2);
    }

    // 速切循环
    if(MainBackendWorker::startNoSwitchFight.load() == 0){  // 开启不切人战斗 = 0  也就是开启速切战斗
        int d = 1;
        int h = 1;
        int x, y;
        // ##### 速切战斗线程ID = BeginThread(速切战斗线程)
        emit startFastSwitchFightWorker();

        while(isBusy()){   // do - loop
            if(setting.ultimateCheckMode == SpecialBossSetting::UltimateCheckMode::Mode1){
                cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                cv::Mat ultimateIcon = cv::imread(QString("%1/720R（判断大招用）.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);

                bool isDetectUltimate1 = Utils::findPic(capImg, ultimateIcon, 0.6, x, y);   // 1147, 622, 1272, 723
                if(isDetectUltimate1){
                    qDebug() << "没有开启大招";
                    capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                    bool isFindColorEx = Utils::findColorEx(capImg, 450, 39, 454, 43, "44B3FF", 0.8, x, y);
                    if(isFindColorEx){
                        d = 1;
                    }
                    else{
                        d = 0;
                    }

                    if(setting.bossHealthAssist){
                        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                        if(setting.boss == SpecialBossSetting::SpecialBoss::Jue){
                            if(Utils::findColorEx(capImg, 176, 181, 199, 204, "FFFFFF", 1, x, y)){
                                h = 1;
                            }
                            else{
                                h = 0;
                            }
                        }
                        else if(setting.boss == SpecialBossSetting::SpecialBoss::Rover){
                            if(Utils::findColorEx(capImg, 207, 163, 269, 216, "FFFFFF", 1, x, y)){
                                h = 1;
                            }
                            else{
                                h = 0;
                            }
                        }
                        else if(setting.boss == SpecialBossSetting::SpecialBoss::Hecate){
                            if(Utils::findColorEx(capImg, 117, 177, 147, 205, "FFFFFF", 0.9, x, y)){
                                h = 1;
                            }
                            else{
                                h = 0;
                            }
                        }
                    }
                    else{
                        h = 0;
                    }
                }
                else{   // if(isDetectUltimate1){  是720R 判断大招用 的else分支
                    this->revive();
                    if(!isBusy()){
                        return true;
                    }
                    if(MainBackendWorker::reviveVal.load() == 1){
                        goto bossUndead;   // I know goto is deadly fucking bad, going to fix it...
                    }
                    for(int i = 0 ; i < 5 && isBusy(); i++){
                        Utils::clickWindowClientArea(Utils::hwnd, 627, 602);  //原来是697
                        Sleep(100);
                    }
                    if(!isBusy()){
                        return true;
                    }

                }
                if(setting.startWithoutSwitch == 1){
                    // ##### 调用不切人判断式
                    this->noSwitchExp(setting.startLeftHP);
                }
                Sleep(100);
            }
            else if(setting.ultimateCheckMode == SpecialBossSetting::UltimateCheckMode::Mode2){
                cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                cv::Mat ultimateIcon = cv::imread(QString("%1/720R（判断大招用）.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
                int x, y;
                bool isDetectUltimate2 = Utils::findPic(capImg, ultimateIcon, 0.4, x, y);  // 1147, 633, 1272, 723
                if(isDetectUltimate2){
                    // 646
                    bool isFindColorEx = Utils::findColorEx(capImg, 1119, 669, 1152, 697, "FEFAF7", 0.6, x, y);
                    if(isFindColorEx){
                        qDebug() << "没有开启大招";
                        if(Utils::findColorEx(capImg, 450, 39, 454, 43, "44B3FF", 0.8, x, y)){
                            d = 1;
                        }
                        else{
                            d = 0;
                        }

                        if(setting.bossHealthAssist){
                            capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                            if(setting.boss == SpecialBossSetting::SpecialBoss::Jue){
                                if(Utils::findColorEx(capImg, 176, 181, 199, 204, "FFFFFF", 1, x, y)){
                                    h = 1;
                                }
                                else{
                                    h = 0;
                                }
                            }
                            else if(setting.boss == SpecialBossSetting::SpecialBoss::Rover){
                                if(Utils::findColorEx(capImg, 207, 163, 269, 216, "FFFFFF", 1, x, y)){
                                    h = 1;
                                }
                                else{
                                    h = 0;
                                }
                            }
                            else if(setting.boss == SpecialBossSetting::SpecialBoss::Hecate){
                                if(Utils::findColorEx(capImg, 117, 177, 147, 205, "FFFFFF", 0.9, x, y)){
                                    h = 1;
                                }
                                else{
                                    h = 0;
                                }
                            }
                        }
                        else{
                            h = 0;
                        }
                    }
                    // bool isFindColorEx = Utils::findColorEx(capImg, 1119, 669, 1152, 697, "FEFAF7", 0.6, x, y); ENDIF
                    if(setting.startWithoutSwitch == 1){
                        // ##### 调用不切人判断式
                        this->noSwitchExp(setting.startLeftHP);
                    }
                    Sleep(100);
                }
                else{
                    //isDetectUltimate2 = Utils::findPic(capImg, ultimateIcon, 0.4, x, y);  // 1147, 633, 1272, 723 的ELSE
                    this->revive();
                    if(!isBusy()){
                        return true;
                    }
                    if(MainBackendWorker::reviveVal.load() == 1){
                        goto bossUndead;   // I know goto is deadly fucking bad, going to fix it...
                    }
                    for(int i = 0 ; i < 5 && isBusy(); i++){
                        Utils::clickWindowClientArea(Utils::hwnd, 627, 602);  //原来是697
                        Sleep(100);
                    }
                    if(!isBusy()){
                        return true;
                    }

                }
            }
            // 大招判断完成
            qDebug() << QString("h = %1, d = %2").arg(h ? "true" : "false").arg(d ? "true" : "false");
            if(h && d){
                i = 0;
                if(MainBackendWorker::isContinueFight.load() >= 1){
                    qDebug() << QString("重复判定继续战斗 %1").arg(MainBackendWorker::isContinueFight.load());
                    MainBackendWorker::isContinueFight.ref();
                    if(MainBackendWorker::isContinueFight.load() >= 4){
                        // ##### 速切战斗线程ID = BeginThread(速切战斗线程)
                        emit startFastSwitchFightWorker();

                        MainBackendWorker::isContinueFight.store(0);
                    }
                }
            }
            else{
                if(setting.boss == SpecialBossSetting::SpecialBoss::Hecate){
                    Sleep(1000);
                }
                i = i + 1;
                if(i == 1){
                    for(int i = 0; i < 5; i++){
                        m_fastSwitchFightBackendWorker.stopWorker();
                        Sleep(50);
                    }
                    MainBackendWorker::isContinueFight.ref();
                }
                if(i == 2){
                    if(setting.boss == SpecialBossSetting::SpecialBoss::Hecate){
                        for(int i = 0; i < 6 && isBusy(); i++){
                            Sleep(300);
                        }
                        if(!isBusy()){
                            return true;
                        }
                    }
                }
                if(i >= 3){
                    for(int i = 0; i < 5; i++){
                        m_fastSwitchFightBackendWorker.stopWorker();
                        Sleep(50);
                    }
                }
                qDebug() << QString("TracePrint i = %1").arg(i);
                if(i >= 4){
                    break; // break while(isBusy()){   // do - loop
                }
            }
            // 740 if 开启不切人战斗
            if(startNoSwitchFight.load() == 1){
                qInfo() << QString("boss血条达到不切人条件 开始（？不）切人用一号位输出");
                m_fastSwitchFightBackendWorker.stopWorker();
                break; // break while(isBusy()){   // do - loop
            }
            if(setting.jumpRecovery){
                QTime endTime = QTime::currentTime();
                qDebug() << QString("从开始战斗到现在过去了 %1 ms").arg(endTime.msecsTo(startTime));
                if(endTime.msecsTo(startTime) > 13500){
                    Sleep(500);
                    for(int i = 0; i < 3 && isBusy(); i++){
                        Utils::keyPress(Utils::hwnd, VK_SPACE, 1);
                        Sleep(100);
                        Utils::clickWindowClientArea(Utils::hwnd, Utils::CLIENT_WIDTH / 2, Utils::CLIENT_HEIGHT / 2);
                        Sleep(100);
                    }
                    startTime = QTime::currentTime();
                }
            }
            if(setting.pressFDuringBattle){
                QTime battlePickUpEndTime = QTime::currentTime();
                if(battlePickUpEndTime.msecsTo(battlePickUpTime) < 20000){
                    qDebug() << QString("战斗开始后 过去了 %1 ms").arg(battlePickUpEndTime.msecsTo(battlePickUpTime));
                    cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                    cv::Mat absorbImg = cv::imread(QString("%1/720p吸收.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
                    if(Utils::findPic(capImg, absorbImg, static_cast<double>(MainBackendWorker::absorbThres.load()), x, y)){
                        Utils::keyPress(Utils::hwnd, 'S', 1);
                        Utils::keyPress(Utils::hwnd, 'F', 1);
                        Utils::keyPress(Utils::hwnd, 'A', 1);
                        Utils::keyPress(Utils::hwnd, 'F', 1);
                        Utils::keyPress(Utils::hwnd, 'W', 2);
                        Utils::keyPress(Utils::hwnd, 'F', 1);
                        Utils::keyPress(Utils::hwnd, 'D', 2);
                        Utils::keyPress(Utils::hwnd, 'F', 1);
                    }
                }
            }
            Sleep(100);
            int rebootCalcPastMs = 0;
            {
                QMutexLocker locker(&m_mutex);
                MainBackendWorker::rebootCalcEndTime = QTime::currentTime();
                rebootCalcPastMs = MainBackendWorker::rebootCalcEndTime.msecsTo(MainBackendWorker::rebootCalcStartTime);
            }
            if(rebootCalcPastMs){
                m_fastSwitchFightBackendWorker.stopWorker();
                // ##### 重启游戏
                reboot(rebootGameSetting);
                {
                    QMutexLocker locker(&m_mutex);
                    MainBackendWorker::rebootCalcStartTime = QTime::currentTime();
                }
                emit startFastSwitchFightWorker();
            }
        }  // end of while(isBusy())  Do-loop

        if(!isBusy()){
            return true;
        }
    } // end if 开启不切人战斗 = 0  也就是开启速切战斗

    // 792行
    isContinueFight.store(0);
    if(MainBackendWorker::startNoSwitchFight.load() == 1){
        emit startNoSwitchFightWorker();
        int d = 1;
        int h = 1;
        int i = 0;
        int x, y;
        while(isBusy()){
            // 799 If Form1.大招检测方式.ListIndex=0
            if(setting.ultimateCheckMode == SpecialBossSetting::UltimateCheckMode::Mode1){
                cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                cv::Mat ultimateIcon = cv::imread(QString("%1/720R（判断大招用）.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
                // 1147, 622, 1272, 723
                if(Utils::findPic(capImg, ultimateIcon, 0.6, x, y)){
                    qDebug()<< "没有开启大招";
                    if(Utils::findColorEx(capImg, 450, 39, 454, 43, "44B3FF", 0.8, x, y)){
                        d = 1;
                    }
                    else{
                        d = 0;
                    }

                    if(setting.bossHealthAssist){
                        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                        if(setting.boss == SpecialBossSetting::SpecialBoss::Jue){
                            if(!Utils::findColorEx(capImg, 176, 181, 199, 204, "FFFFFF", 1, x, y)){
                                h = 1;
                            }
                            else{
                                h = 0;
                            }
                        }
                        else if(setting.boss == SpecialBossSetting::SpecialBoss::Rover){
                            if(!Utils::findColorEx(capImg, 207, 163, 269, 216, "FFFFFF", 1, x, y)){
                                h = 1;
                            }
                            else{
                                h = 0;
                            }
                        }
                        else if(setting.boss == SpecialBossSetting::SpecialBoss::Hecate){
                            if(!Utils::findColorEx(capImg, 117, 177, 147, 205, "FFFFFF", 0.9, x, y)){
                                h = 1;
                            }
                            else{
                                h = 0;
                            }
                        }
                    }
                    else{
                        h = 0;
                    }
                }
                else{
                    // 开启了大招
                    this->revive();
                    if(!isBusy()){
                        return true;
                    }
                    if(reviveVal.load() == 1){
                        goto bossUndead;
                    }
                    for(int i = 0; i < 5 && isBusy(); i++){
                        Utils::clickWindowClientArea(Utils::hwnd, 627, 602);
                        Sleep(300);
                    }
                    if(!isBusy()){
                        return true;
                    }
                }
                if(setting.startWithoutSwitch == 1){
                    // ##### 调用不切人判断式
                    this->noSwitchExp(setting.startLeftHP);
                }
                Sleep(100);
            }
            else if (setting.ultimateCheckMode == SpecialBossSetting::UltimateCheckMode::Mode2) {
                cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                cv::Mat ultimateIcon = cv::imread(QString("%1/720R（判断大招用）.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
                if(Utils::findPic(capImg, ultimateIcon, 0.4, x, y)){   // 1147, 622,1272,723
                    if(Utils::findColorEx(capImg, 1119, 669, 1152, 697, "FEFAF7", 0.6, x, y)){
                        qDebug() << "没有开启大招";
                        if(Utils::findColorEx(capImg, 450, 39, 454, 43, "44B3FF", 0.8, x, y)){
                            d = 1;
                        }
                        else {
                            d = 0;
                        }

                        capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                        if(setting.bossHealthAssist == 1){
                            if(setting.boss == SpecialBossSetting::SpecialBoss::Jue){
                                if(Utils::findColorEx(capImg, 176,181,199,204, "FFFFFF", 1.0, x, y)){
                                    h = 1;
                                }
                                else {
                                    h = 0;
                                }
                            }
                            else if(setting.boss == SpecialBossSetting::SpecialBoss::Rover){
                                if(Utils::findColorEx(capImg, 207,163,269,216, "FFFFFF", 1.0, x, y)){
                                    h = 1;
                                }
                                else {
                                    h = 0;
                                }
                            }
                            else if(setting.boss == SpecialBossSetting::SpecialBoss::Hecate){
                                if(Utils::findColorEx(capImg, 117,177,147,205, "FFFFFF", 0.9, x, y)){
                                    h = 1;
                                }
                                else {
                                    h = 0;
                                }
                            }
                        }
                        else{
                            h = 0;
                        }
                    }
                    if(setting.startWithoutSwitch == 1){
                        // ##### 调用不切人判断式
                        this->noSwitchExp(setting.startLeftHP);
                    }
                    Sleep(100);
                }
                else{
                    // 开启了大招
                    this->revive();
                    if(!isBusy()){
                        return true;
                    }
                    if(reviveVal.load() == 1){
                        goto bossUndead;
                    }
                    for(int i = 0; i < 2 && isBusy(); i++){
                        Utils::clickWindowClientArea(Utils::hwnd, 627, 602);
                        Sleep(300);
                    }
                    if(!isBusy()){
                        return true;
                    }
                }
            }
            // 大招判断完成  903
            qDebug() << QString("h = %1, d = %2").arg(h ? "true" : "false").arg(d ? "true" : "false");
            if((d+h) >= 1){
                i = 0;
                if(MainBackendWorker::isContinueFight.load() >= 1){
                    qDebug() << "重复判定继续战斗";
                    MainBackendWorker::isContinueFight.ref();
                    if(MainBackendWorker::isContinueFight.load() >= 4){
                        emit startNoSwitchFightWorker();
                        MainBackendWorker::isContinueFight.store(0);
                    }
                }
            }
            else{
                i = i + 1;
                if(i == 1){
                    for(int i = 0; i < 5; i++){
                        m_fastSwitchFightBackendWorker.stopWorker();
                        Sleep(50);
                    }
                    MainBackendWorker::isContinueFight.ref();
                }
                if(i == 2){
                    if(setting.boss == SpecialBossSetting::SpecialBoss::Hecate){
                        for(int i = 0; i < 6 && isBusy(); i++){
                            Sleep(300);
                        }
                        if(!isBusy()){
                            return true;
                        }
                    }
                }
                if(i >= 3){
                    for(int i = 0; i < 5; i++){
                        m_fastSwitchFightBackendWorker.stopWorker();
                        Sleep(50);
                    }
                }
                qDebug() << QString("TracePrint i = %1").arg(i);
                if(i >= 4){
                    break; // break while(isBusy()){   // do - loop
                }
            }
            if(startNoSwitchFight.load() == 0){
                detectNoSwitch = detectNoSwitch + 1;
                if(detectNoSwitch == 3){
                    qDebug() << QString("重新判断boss血条未达到不切人条件，强制开始重新切人输出");
                    detectNoSwitch = 0;
                    m_noSwitchFightBackendWorker.stopWorker();
                    goto detectNoSwitch;
                }
            }
            Sleep(500);
            // 重启计算结束时间 计算过去了多少ms 目前(20250105)没有多线程冲突修改时间的可能，但以防万一
            int rebootCalcPastMs = 0;
            {
                QMutexLocker locker(&m_mutex);
                MainBackendWorker::rebootCalcEndTime = QTime::currentTime();
                rebootCalcPastMs = MainBackendWorker::rebootCalcEndTime.msecsTo(MainBackendWorker::rebootCalcStartTime);
            }
            if(rebootCalcPastMs > 300000){
                qDebug() << QString("重启计算结束时间 - 重启计算开始时间 = %1 ms").arg(rebootCalcPastMs);
                for(int i = 0; i<5; i++){
                    m_noSwitchFightBackendWorker.stopWorker();
                    m_fastSwitchFightBackendWorker.stopWorker();
                    Sleep(50);
                }
                //##### 调用重启
                reboot(rebootGameSetting);
                {
                    QMutexLocker locker(&m_mutex);
                    MainBackendWorker::rebootCalcStartTime = QTime::currentTime();
                }

                emit startNoSwitchFightWorker();
            }

        } // end of while(isBusy())  Do-loop

        if(!isBusy()){
            return true;
        }
    } // end if 开启不切人战斗 = 1  也就是开启不切人战斗
    qInfo() << QString("战斗结束");
    return true;
}

bool MainBackendWorker::reboot(const RebootGameSetting& rebootGameSetting){
    qInfo() << QString("重启,参数信息 %1").arg(rebootGameSetting.toQString());


}


