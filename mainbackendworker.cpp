#include "mainbackendworker.h"

QTime MainBackendWorker::rebootCalcStartTime;
QTime MainBackendWorker::rebootCalcEndTime;
QTime MainBackendWorker::dungeonStartTime;
QTime MainBackendWorker::dungeonEndTime;
QAtomicInt MainBackendWorker::isContinueFight;
QTime MainBackendWorker::battlePickUpTime;  // 战斗时按拾取开始时间
QAtomicInt MainBackendWorker::reviveVal; // 复苏变量

// 实现部分
MainBackendWorker::MainBackendWorker(QObject* parent)
    : QObject(parent)  // 初始化基类 QObject
{
    m_isRunning.store(0);

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
    bool isPickUp = false;
    if(!isPickUp){
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
    int rebootCount = 0;
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
            Utils::middleClickWindowClientArea(Utils::hwnd, 640, 360);
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
            Utils::middleClickWindowClientArea(Utils::hwnd, 640, 360);
            break;
        }

        // ##### 时间差默认是s还是ms？
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
            Utils::middleClickWindowClientArea(Utils::hwnd, 640, 360);
        }

        if(!isBusy()){
            break;
        }

        rebootCount++;
        skipMonthCard();
        if(rebootCount > 30){
            quitRover();  //退出无妄者
            rebootCount = 5;
        }
    }
    Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
    skipMonthCard();
    for(int i = 0; i < 15 && isBusy(); i++){
        Sleep(1000);
    }

    return true;
}

bool MainBackendWorker::revive(){
    for(int i = 0; i < 10 && isBusy(); i++){
        cv::Mat capImg = Utils::qImage2CvMat(Utils::captureWindowToQImage(Utils::hwnd));
        cv::Mat reviveImg = cv::imread(QString("%1/复苏.bmp").arg(Utils::IMAGE_DIR()).toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
        int x, y;
        bool isFindRevivePic = Utils::findPic(capImg, reviveImg, 0.7, x, y);
        if(isFindRevivePic){
            for(int i = 0; i<5 && isBusy(); i++){
                emit stopSwitchFightSignal();
                emit stopNoSwitchFightSignal();
                Sleep(500);
            }
            if(!isBusy()){
                return true;
            }

            Utils::keyPress(Utils::hwnd, VK_ESCAPE, 1);

        }
    }

    /*
 For 10
        FindPic 257,113,343,180,"C:\noki\复苏.bmp",0.7,intX,intY
        If intX > 0 And intY > 0 Then
            TracePrint "C:\noki\复苏.bmp"
            //              Call QUI输出调试信息("C:\鸣潮1.3副本boss单刷日志.log")
            For 5
                StopThread 不切人战斗线程ID
                StopThread 速切战斗线程ID
                Delay 500
            Next
            KeyPress "Esc", 1
            开始时间 = Plugin.Sys.GetTime()
            Do
                FindPic 750,591,865,644,"C:\noki\重新挑战.bmp",0.8,intX,intY
                If intX > 0 And intY > 0 Then
                    For 4
                        Delay 500
                        MoveTo 807,610
                        Delay 500
                        LeftClick 1
                        Delay 500
                        MoveTo 857, 450
                        Delay 500
                        LeftClick 1
                    Next
                    复苏变量=1
                    Exit Do
                End If
                结束时间 = Plugin.Sys.GetTime()
                If 结束时间 - 开始时间 > 120000 Then
                    Call 退出无妄者
                    Exit Do
                End If
                Delay 200
            Loop
        End If
        Delay 100
    Next
    */

    return true;
}

