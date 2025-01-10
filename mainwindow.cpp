#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 注册自定义类型
    registerType();

    // 绑定日志
    // debug 重定向
    DebugMessageHandler::instance().setPlainTextEdit(ui->logPlainText);
    qInstallMessageHandler(DebugMessageHandler::messageHandler);
    DebugMessageHandler::instance().setLogLevel(QtMsgType::QtInfoMsg);
    connect(ui->logPlainText, &QPlainTextEdit::textChanged, [this]() {
        QScrollBar *scrollBar = ui->logPlainText->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });

    // 为logLevelBox设置本文代理格式
    CustomDelegate *delegate = new CustomDelegate(ui->logLevelBox);
    ui->logLevelBox->setItemDelegate(delegate);

    if (Utils::isRunningAsAdmin()) {
        qDebug() << "Program is running as Administrator.";
    } else {
        qDebug() << "Program is NOT running as Administrator." ;
    }

    // 为锁定声骸填充默认参数
    genDefaultLockEchoSetting();

    // 启动自动切换背景图线程
    m_autoChangeWallpaperBackendWorker.moveToThread(&m_autoChangeWallpaperBackendThread);
    m_autoChangeWallpaperBackendThread.start();

    // 启动后台线程
    m_mainBackendWorker.moveToThread(&m_mainBackendThread);
    m_mainBackendThread.start();

    m_mainBackendWorkerNew.moveToThread(&m_mainBackendNewThread);
    m_mainBackendNewThread.start();

    // 锁定声骸
    connect(this, &MainWindow::startLockEcho, &this->m_mainBackendWorkerNew, &MainBackendWorkerNew::onStartLockEcho);
    connect(&this->m_mainBackendWorkerNew, &MainBackendWorkerNew::lockEchoDone, this, &MainWindow::onLockEchoDone);

    // test1的连接
    connect(this, &MainWindow::startTest1, &this->m_mainBackendWorker, &MainBackendWorker::onStartTest1);
    connect(&this->m_mainBackendWorker, &MainBackendWorker::startTest1Done, this, &MainWindow::onStartTest1Done);

    // 特殊boss 角 无妄者 连接
    connect(ui->specialBossPanel, &SpeicalBossWidget::startSpecialBoss, this, &MainWindow::checkSpecialBoss);
    connect(this, &MainWindow::startSpecialBoss, &this->m_mainBackendWorker, &MainBackendWorker::onStartSpecialBoss);
    connect(&this->m_mainBackendWorker, &MainBackendWorker::startSpecialBossDone, this, &MainWindow::onStartSpecialBossDone);

    // 注册全局快捷键 允许快捷停止脚本运行
    registerGlobalHotKey();

    // 安装全局事件过滤器
    hotKeyFilter = new GlobalHotKeyFilter();
    qApp->installNativeEventFilter(hotKeyFilter);

    // 连接热键信号到槽
    connect(hotKeyFilter, &GlobalHotKeyFilter::hotKeyPressed, this, &MainWindow::onHotKeyActivated);

    // 连接 测试面板
    connect(ui->debugPanel, &DebugForm::startTestFastSwitch, &this->m_mainBackendWorker.m_fastSwitchFightBackendWorker, &FastSwitchFightBackendWorker::testStartFight);
    connect(ui->debugPanel, &DebugForm::startTestRebootGame, &this->m_mainBackendWorker, &MainBackendWorker::onStartTestRebootGame);


    //连接 更新壁纸
    connect(&this->m_autoChangeWallpaperBackendWorker, &AutoChangeWallpaperBackendworker::sendImageAsWallpaper, this, &MainWindow::onSendImageAsWallpaper);
    connect(this, &MainWindow::startChangeWallpaper, &this->m_autoChangeWallpaperBackendWorker, &AutoChangeWallpaperBackendworker::startWorker);
    emit startChangeWallpaper();

    const bool isTransparentBkg = false;
    if(isTransparentBkg){
        // ui某些控件改透明
        ui->tabWidget->setAttribute(Qt::WA_StyledBackground, true);
        ui->tabWidget->setStyleSheet("background: transparent;");

        ui->centralWidget->setStyleSheet(
                    "QWidget { background: transparent; }"
                    "QTabWidget::pane { background: transparent; }"
                    "QTabBar::tab { background: transparent; }"
                    "QStackedWidget { background: transparent; }"
                    // 根据需要添加更多的组件类型
                    );

        ui->tabWidget->setStyleSheet(R"(
                                     /* QTabWidget::pane 决定整个 tab 页区域的背景 */
                                     QTabWidget::pane {
                                     background: transparent;
                                     border: none; /* 如果不想要边框可以去掉 */
                                     }

                                     /* QTabBar::tab 决定选项卡按钮的背景 */
                                     QTabBar::tab {
                                     background: transparent;
                                     }

                                     /* QStackedWidget 是显示 tab 页内容的容器 */
                                     QStackedWidget {
                                     background: transparent;
                                     }
                                     )");

        // 3. 如果每个 tab 页本身（比如 tab1、tab2）还需要单独设置，可以再加：
        ui->generalPanel->setStyleSheet("background: transparent;");
        ui->generalPanel->setAttribute(Qt::WA_StyledBackground, true);
        ui->debugPanel->setStyleSheet("background: transparent;");
        ui->debugPanel->setAttribute(Qt::WA_StyledBackground, true);
    }
}

MainWindow::~MainWindow()
{
    // 取消 停止脚本快捷键
    unregisterGlobalHotKey(); // 注销全局快捷键
    qApp->removeNativeEventFilter(hotKeyFilter);
    delete hotKeyFilter;

    // 停止自动切换壁纸
    m_autoChangeWallpaperBackendWorker.stopWorker();
    m_autoChangeWallpaperBackendThread.quit();
    m_autoChangeWallpaperBackendThread.wait();

    // 停止后台线程
    m_mainBackendWorker.stopWorker();
    m_mainBackendThread.quit();
    m_mainBackendThread.wait();

    m_mainBackendWorkerNew.stopWorker();
    m_mainBackendNewThread.quit();
    m_mainBackendNewThread.wait();

    delete ui;
}

// 注册变量类型
void MainWindow::registerType(){
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextCharFormat>("QTextCharFormat");
    qRegisterMetaType<QMessageBox::Icon>("QMessageBox::Icon");
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<SpecialBossSetting>("SpecialBossSetting");
    qRegisterMetaType<RebootGameSetting>("RebootGameSetting");
    qRegisterMetaType<LockEchoSetting>("LockEchoSetting");
    qRegisterMetaType<SingleEchoSetting>("SingleEchoSetting");

}

void MainWindow::genDefaultLockEchoSetting(){
    auto setting = ui->echoLockEntriesPanel->getLockEchoSettingFromUI();
    for(auto echoSet: setting.echoSetName2singleSetting.keys()){
        SingleEchoSetting &echoSetSetting = setting.echoSetName2singleSetting[echoSet];
        if(echoSet == "freezingFrost"){
             // 今州冰套
             echoSetSetting.islevel5 = true;
             echoSetSetting.isLockJudge = true;
             echoSetSetting.isDiscardedJudge = true;
             echoSetSetting.isNormalJudge = true;
             // 今州冰套 C1
             QVector<QString> c1entries = { "attackRatio"};
             echoSetSetting.cost2EntryMap[1] = c1entries;

             // 今州冰套C3
             QVector<QString> c3entries = { "glacioDMG", "energyRegen"};
             echoSetSetting.cost2EntryMap[3] = c3entries;

             // 今州冰套C4
             QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
             echoSetSetting.cost2EntryMap[4] = c4entries;
        }
    }
    qInfo().noquote() << setting.toQString();
    ui->echoLockEntriesPanel->setLockEchoSetting2UI(setting);
}

void MainWindow::registerGlobalHotKey() {
    if (!RegisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyId, MOD_ALT, VK_F12)) {
        qDebug() << "Failed to register Alt + F12!";
    } else {
        qDebug() << "Alt + F12 hotkey registered.";
    }
}

void MainWindow::unregisterGlobalHotKey() {
    UnregisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyId);
}

void MainWindow::onHotKeyActivated(int id) {
    if (id == toggleHotKeyId) {
        qDebug() << "收到ALT+F12，中止脚本";
        on_stopBtn_clicked();
    } else {
        qWarning() << "Unhandled hotkey ID:" << id;
    }
}

void MainWindow::on_getGameWin_clicked(){
    bool isWuwaInit = Utils::initWuwaHwnd();
    if(!isWuwaInit){
        qWarning() << QString("未能绑定鸣潮窗口句柄");
        return;
    }

    QImage image = Utils::captureWindowToQImage(Utils::hwnd);
    if(image.isNull()){
        qWarning() << QString("未能获取鸣潮窗口图像");
        return;
    }

    QString capImagePath = QString("%1/capWuwa.png").arg(Utils::IMAGE_DIR());
    image.save(capImagePath);
}

void MainWindow::on_test1_clicked(){
    // 首先检查后台是否繁忙
    if(m_mainBackendWorker.isBusy()){
        QMessageBox::warning(this, "脚本正在后台运行", "请先手动停止或等待脚本运行结束");
        return;
    }

    emit startTest1();
    ui->isBusyBox->setChecked(true);

}

void MainWindow::onStartTest1Done(const bool &isNormalEnd, const QString& msg){
    ui->isBusyBox->setChecked(false);

    if(isNormalEnd){
        QMessageBox::information(this, "后台任务正常结束", "后台任务正常结束");
    }
    else{
        QMessageBox::critical(this, "后台任务异常结束", msg);
    }

    qInfo() << QString("onStartTest1Done, result %1, msg %2").arg(isNormalEnd).arg(msg);
}

void MainWindow::on_stopBtn_clicked(){
    m_mainBackendWorker.stopWorker();
    m_mainBackendWorkerNew.stopWorker();
}






void MainWindow::checkSpecialBoss(const SpecialBossSetting& setting){
    // 检查窗口是否已经启动
    bool isWuwaInit = Utils::initWuwaHwnd();
    if(!isWuwaInit){
        QMessageBox::warning(this, "尚未启动鸣潮", "请先启动鸣潮再尝试启动脚本");
        return;
    }

    bool isWuwaRunning = Utils::isWuwaRunning();
    if(!isWuwaRunning){
        QMessageBox::warning(this, "尚未启动鸣潮", "请先启动鸣潮再尝试启动脚本");
        return;
    }

    // 获取一张图 然后检查其分辨率
    QImage image = Utils::captureWindowToQImage(Utils::hwnd);
    if(image.isNull()){
        QMessageBox::warning(this, "不可以将鸣潮最小化", "请将鸣潮置于前台，启动脚本后可将其他窗口置于鸣潮上面。但不可将鸣潮最小化");
        return;
    }

    // 检查分辨率
    if(image.width() != Utils::CLIENT_WIDTH || image.height() != Utils::CLIENT_HEIGHT){
        QMessageBox::warning(this, "脚本仅支持1280 720窗口模式",
                             QString("当前分辨率 %1 * %2\n"
                                     "可考虑将windows DPI 缩放改为100%\n"
                                     "并多次切换游戏分辨率，最后设置为1280 * 720\n"
                                     "然后重启游戏，启动脚本").arg(image.width()).arg(image.height()));
        return;
    }

    // 检查此时是否正忙
    if(m_mainBackendWorkerNew.isBusy() || m_mainBackendWorker.isBusy()){
        QMessageBox::warning(this, "后台正忙", "请停止其他脚本，再启用本脚本");
        return;
    }

    // 从通用面板获取一些额外参数
    RebootGameSetting rebootGameSetting = ui->generalPanel->getRebootGameSetting();
    emit startSpecialBoss(setting, rebootGameSetting);
    ui->isBusyBox->setChecked(true);
}

void MainWindow::onStartSpecialBossDone(const bool& isNormalEnd, const QString& msg, \
                                        const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting){
    Q_UNUSED(setting);
    Q_UNUSED(rebootGameSetting);
    ui->isBusyBox->setChecked(false);

    if(isNormalEnd){
        //QMessageBox::information(this, "特殊boss正常结束", msg);
    }
    else{
        //QMessageBox::critical(this, "特殊boss异常结束", msg);
    }

    qInfo() << QString("onStartSpecialBossDone, result %1, msg %2").arg(isNormalEnd).arg(msg);
}

// 更新壁纸
void MainWindow::onSendImageAsWallpaper(const QImage& img){
    return;
    if (img.isNull()) {
        qWarning() << "Image is null. Cannot set as wallpaper.";
        return;
    }

    // Step 1: 检查图像格式
    if (img.format() != QImage::Format_RGB888) {
        qWarning() << "Image format is not RGB888. Converting to RGB888.";
    }

    // 如果不是 RGB888，转换为 RGB888
    QImage processedImg = img.convertToFormat(QImage::Format_RGB888);

    qint64 totalR = 0, totalG = 0, totalB = 0;
    int pixelCount = processedImg.width() * processedImg.height();

    // Step 2: 遍历像素，逐字节读取 RGB 值
    for (int y = 0; y < processedImg.height(); ++y) {
        const uchar *line = processedImg.scanLine(y);
        for (int x = 0; x < processedImg.width(); ++x) {
            int index = x * 3; // RGB888 每个像素占用 3 字节
            totalR += line[index];     // Red
            totalG += line[index + 1]; // Green
            totalB += line[index + 2]; // Blue
        }
    }

    // Step 3: 计算平均 RGB 值
    int avgR = totalR / pixelCount;
    int avgG = totalG / pixelCount;
    int avgB = totalB / pixelCount;

    // Step 4: 计算亮度
    int brightness = (avgR * 299 + avgG * 587 + avgB * 114) / 1000; // 加权亮度公式
    QString textColor = (brightness < 128) ? "white" : "black"; // 根据亮度选择相反色

    // Step 3: 动态设置全局样式表
    QString styleSheet = QString("QWidget { color: %1; font-weight: bold; }").arg(textColor);
    qApp->setStyleSheet(styleSheet);

    // Step 4: 更新壁纸
    // 获取 centralWidget 的大小
    QSize targetSize = ui->centralWidget->size();

    // 转换 QImage 为 QPixmap，并进行缩放（使用 Bicubic 插值）
    QPixmap pixmap = QPixmap::fromImage(img.scaled(targetSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    // 设置背景到 centralWidget
    QPalette palette = ui->centralWidget->palette();
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    ui->centralWidget->setPalette(palette);
    ui->centralWidget->setAutoFillBackground(true);

    qDebug() << "Wallpaper successfully updated.";
}


void MainWindow::on_startLockEcho_clicked(){
    qInfo() << QString("MainWindow::on_startLockEcho_clicked");

    if(m_mainBackendWorkerNew.isBusy() || m_mainBackendWorker.isBusy()){
        QMessageBox::warning(this, "有其他后台正忙", "请等待任务结束或手动停止后台任务，再启动锁定声骸");
        return;
    }

    LockEchoSetting lockEchoSetting = ui->echoLockEntriesPanel->getLockEchoSettingFromUI();
    qInfo().noquote() << lockEchoSetting.toQString();

    ui->isBusyBox->setChecked(true);
    emit startLockEcho(lockEchoSetting);


}

void MainWindow::onLockEchoDone(const bool& isNormalEnd, const QString& errMsg, const LockEchoSetting &lockEchoSetting){
    ui->isBusyBox->setChecked(false);

    if(isNormalEnd){
        QMessageBox::information(this, "锁定声骸结束", errMsg);
    }
    else{
        QMessageBox::critical(this, "锁定声骸结束", errMsg);
    }

    qInfo() << QString("onLockEchoDone, result %1, msg %2").arg(isNormalEnd).arg(errMsg);
}
