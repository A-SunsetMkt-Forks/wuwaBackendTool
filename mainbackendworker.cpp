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
    /*
    // 对应副本BOSS单刷1122V10 源文件 265行 Function 月卡跳过
    if(dateTime.time().hour() == 4 && dateTime.time().minute() == 0){
        // 循环5次
        for(int i = 0; i < 5; i++){
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
    }
    else if(dateTime.time().hour() == 4 && dateTime.time().minute() == 1){

    }
    */
    // 不再使用上述方法 改用图像检测


}

void MainBackendWorker::pickUpEchoIsBossDied(){

}

void MainBackendWorker::quitRover(){

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
    // 在按键精灵源码这是一个sub 也就是void函数 没有返回值 363行
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

        {
            QMutexLocker locker(&m_mutex);
            MainBackendWorker::rebootCalcEndTime = QTime::currentTime();
        }
        if(rebootCalcStartTime.msecsTo(rebootCalcEndTime) > 60000){
            qWarning() << QString("触发重启游戏条件 已经过了 %1 ms").arg(rebootCalcStartTime.msecsTo(rebootCalcEndTime));
            // 调用重启游戏
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
    MainBackendWorker::startNoSwitchFight.store(false);
    // ##### 需要对屏幕截图进行ROI处理
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

    return true;
}

bool MainBackendWorker::revive(){
    for(int i = 0; i < 10 && isBusy(); i++){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        cv::Mat reviveImg = cv::imread(QString("%1/复苏.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        int x, y;
        bool isFindRevivePic = Utils::findPic(capImg, reviveImg, 0.7, x, y);  // 257,113,343,180
        if(isFindRevivePic){
            for(int i = 0; i<5 && isBusy(); i++){
                // ##### 信号暂时放在这 没有实现
                emit stopSwitchFightSignal();
                emit stopNoSwitchFightSignal();
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
    {
        QMutexLocker locker(&m_mutex);
        MainBackendWorker::battlePickUpTime = QTime::currentTime();
    }

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
    if(MainBackendWorker::startNoSwitchFight.load() == 0){  // 开启不切人战斗 = 0
        int d = 1;
        int h = 1;
        // ##### 速切战斗线程ID = BeginThread(速切战斗线程)
        while(isBusy()){
            if(setting.ultimateCheckMode == SpecialBossSetting::UltimateCheckMode::Mode1){
                cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
                cv::Mat ultimateIcon = cv::imread(QString("%1/720R（判断大招用）.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
                int x, y;
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
                    if(MainBackendWorker::reviveVal.load() == 1){
                        goto bossUndead;   // I know goto is deadly fucking bad, going to fix it...
                    }
                    for(int i = 0 ; i < 5 && isBusy(); i++){
                        Utils::clickWindowClientArea(Utils::hwnd, 627, 697);
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
                    if(MainBackendWorker::reviveVal.load() == 1){
                        goto bossUndead;   // I know goto is deadly fucking bad, going to fix it...
                    }
                    for(int i = 0 ; i < 5 && isBusy(); i++){
                        Utils::clickWindowClientArea(Utils::hwnd, 627, 697);
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
                        // ##### StopThread 速切战斗线程ID
                        Sleep(50);
                    }
                    MainBackendWorker::isContinueFight.ref();
                }
                if(i == 2){

                }
            }
        }

        if(!isBusy()){
            return true;
        }
    }




    return true;
}

