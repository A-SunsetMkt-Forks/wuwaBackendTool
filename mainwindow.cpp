#include "mainwindow.h"
#include "ui_mainwindow.h"

// 原始的 SHA-256 哈希值转为字节数组  临时允许本地时间作为toolCurrentTime
constexpr std::array<uint8_t, 32> expectedHash = {
    0x5e, 0x0e, 0x04, 0x3b, 0xb1, 0x1d, 0x9c, 0xa9,
    0x00, 0x2e, 0x61, 0x22, 0x16, 0x78, 0x56, 0xe7,
    0x80, 0x7c, 0x19, 0x99, 0xb3, 0x6f, 0x34, 0x08,
    0xfa, 0xef, 0x8d, 0xf8, 0x13, 0x83, 0x23, 0xcf
};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1) 启动时先做一次检查
    if (!initialCheck()) {
        //if(!isTempActivate()){
            // 如果未通过验证，直接退出程序
            std::exit(0);
            return;
        //}
    }


    ui->generalPanel->setActivateStartTime(licStartTime);
    ui->generalPanel->setExpirationTime(licStartTime.addMSecs(MAX_MSECS));
    ui->generalPanel->setCurrentTime(toolCurrentTime);

    //InternetTimeFetcher fetcher;
    //fetcher.fetchInternetTime(toolCurrentTime);
    softElapsedTime.start();

    // 2) 每分钟  更新    QDateTime toolCurrentTime;     QTimer softElapsedTime;
    connect(&checkLicTimer, &QTimer::timeout, this, &MainWindow::onCheckLic);
    checkLicTimer.start(60 * 1000); // 1分钟

    // 每秒更新UI
    connect(&updateCurrentDtTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimeOnUI);
    updateCurrentDtTimer.start(1000);


    // 注册自定义类型
    registerType();

    this->setWindowTitle("鸣潮自动刷boss脚本 v1.0.7 alpha 永久免费 无广告病毒");

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

    // 为普通boss填充默认参数
    genDefaultNormalBossSetting();

    // 启动自动切换背景图线程
    m_autoChangeWallpaperBackendWorker.moveToThread(&m_autoChangeWallpaperBackendThread);
    m_autoChangeWallpaperBackendThread.start();


    m_mainBackendWorkerNew.moveToThread(&m_mainBackendNewThread);
    m_mainBackendNewThread.start();

    // 锁定声骸
    connect(this, &MainWindow::startLockEcho, &this->m_mainBackendWorkerNew, &MainBackendWorkerNew::onStartLockEcho);
    connect(&this->m_mainBackendWorkerNew, &MainBackendWorkerNew::lockEchoDone, this, &MainWindow::onLockEchoDone);

    // 轮刷boss
    connect(this, &MainWindow::startNormalBoss, &this->m_mainBackendWorkerNew, &MainBackendWorkerNew::onStartNormalBoss);
    connect(&this->m_mainBackendWorkerNew, &MainBackendWorkerNew::normalBossDone, this, &MainWindow::onNormalBossDone);

    // 单数特殊boss
    connect(this, &MainWindow::startSpecialBoss, &this->m_mainBackendWorkerNew, &MainBackendWorkerNew::onStartSpecialBoss);
    connect(&this->m_mainBackendWorkerNew, &MainBackendWorkerNew::specialBossDone, this, &MainWindow::onSpecialBossDone);

    // 注册全局快捷键 允许快捷停止脚本运行
    registerGlobalHotKey();

    // 安装全局事件过滤器
    hotKeyFilter = new GlobalHotKeyFilter();
    qApp->installNativeEventFilter(hotKeyFilter);

    // 连接热键信号到槽
    connect(hotKeyFilter, &GlobalHotKeyFilter::hotKeyPressed, this, &MainWindow::onHotKeyActivated);


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


    if(!Utils::isRunningAsAdmin()){
        QMessageBox::warning(this, "请以管理员身份重启软件", "不以管理员身份启动，可能某些指令鸣潮不予理会");
    }
}

MainWindow::~MainWindow()
{

    if(hotKeyFilter){
        // 取消 停止脚本快捷键
        unregisterGlobalHotKey(); // 注销全局快捷键
        qApp->removeNativeEventFilter(hotKeyFilter);
        hotKeyFilter->deleteLater();
    }


    // 停止自动切换壁纸
    m_autoChangeWallpaperBackendWorker.stopWorker();
    m_autoChangeWallpaperBackendThread.quit();
    m_autoChangeWallpaperBackendThread.wait();


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
    qRegisterMetaType<NormalBossSetting>("NormalBossSetting");
}

void MainWindow::genDefaultLockEchoSetting(){
    auto setting = ui->echoLockEntriesPanel->getLockEchoSettingFromUI();
    ui->echoLockEntriesPanel->genDefaultSetting(setting);
    //qInfo().noquote() << setting.toQString();
    ui->echoLockEntriesPanel->setLockEchoSetting2UI(setting);
}

void MainWindow::genDefaultNormalBossSetting(){
    NormalBossSetting setting;
    //qInfo().noquote() << setting.toQString();
    ui->normalBossPanel->setNormalBossSetting2UI(setting);
}

void MainWindow::registerGlobalHotKey() {
    if (!RegisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyId, MOD_ALT, VK_F12)) {
        qDebug() << "Failed to register Alt + F12!";
    } else {
        qDebug() << "Alt + F12 hotkey registered.";
    }

    // 注册截图 快捷键
    //if (!RegisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyIdScrCpy, VK_F9, VK_NUMPAD5)) {  // F9 + 5
    //if (!RegisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyIdScrCpy, MOD_ALT, 0x50)) {
    if (!RegisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyIdScrCpy, MOD_CONTROL, 0x50)) {
        qDebug() << "Failed to register F9 + 5!";
    } else {
        qDebug() << "F9 + 5 hotkey registered.";
    }
}

void MainWindow::unregisterGlobalHotKey() {
    UnregisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyId);
    UnregisterHotKey(reinterpret_cast<HWND>(winId()), toggleHotKeyIdScrCpy);
}

void MainWindow::onHotKeyActivated(int id) {
    if (id == toggleHotKeyId) {
        qDebug() << "收到ALT+F12，中止脚本";
        on_stopBtn_clicked();
    } else if (id == toggleHotKeyIdScrCpy) {
        qDebug() << "收到 F9 + 5，触发截图";
        ui->debugPanel->on_scrCap_clicked();
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
    m_mainBackendWorkerNew.stopWorker();
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

    if(m_mainBackendWorkerNew.isBusy()){
        QMessageBox::warning(this, "有其他后台正忙", "请等待任务结束或手动停止后台任务，再启动锁定声骸");
        return;
    }

    LockEchoSetting lockEchoSetting = ui->echoLockEntriesPanel->getLockEchoSettingFromUI();
    //qInfo().noquote() << lockEchoSetting.toQString();

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

// 启动普通boss
void MainWindow::on_startNormalBoss_clicked(){
    qInfo() << QString("MainWindow::on_startNormalBoss_clicked");
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

    if(m_mainBackendWorkerNew.isBusy()){
        QMessageBox::warning(this, "有其他后台正忙", "请等待任务结束或手动停止后台任务，再启动普通boss轮刷");
        return;
    }

    NormalBossSetting normalBossSetting = ui->normalBossPanel->getNormalBossSettingFromUI();
    //qInfo().noquote() << normalBossSetting.toQString();

    ui->isBusyBox->setChecked(true);
    emit startNormalBoss(normalBossSetting);
}

// 普通boss结束
void MainWindow::onNormalBossDone(const bool& isNormalEnd, const QString& errMsg, const NormalBossSetting &normalBossSetting){
    ui->isBusyBox->setChecked(false);

    if(isNormalEnd){
        QMessageBox::information(this, "轮刷普通boss结束", errMsg);
    }
    else{
        QMessageBox::critical(this, "轮刷普通boss结束", errMsg);
    }

    qInfo() << QString("onNormalBossDone, result %1, msg %2").arg(isNormalEnd).arg(errMsg);
}

void MainWindow::on_startSingleBoss_clicked(){
    qInfo() << QString("准备启动特殊BOSS脚本 ");

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

    if(m_mainBackendWorkerNew.isBusy()){
        QMessageBox::warning(this, "有其他后台正忙", "请等待任务结束或手动停止后台任务，再启动普通boss轮刷");
        return;
    }

    SpecialBossSetting setting = ui->specialBossPanel->getSetting();
    ui->isBusyBox->setChecked(true);
    emit startSpecialBoss(setting);
}

void MainWindow::onSpecialBossDone(const bool& isNormalEnd, const QString& errMsg, const SpecialBossSetting &specialBossSetting){
    ui->isBusyBox->setChecked(false);

    if(isNormalEnd){
        QMessageBox::information(this, "单刷特殊boss结束", errMsg);
    }
    else{
        QMessageBox::critical(this, "单刷特殊boss结束", errMsg);
    }

    qInfo() << QString("onSpecialBossDone, result %1, msg %2").arg(isNormalEnd).arg(errMsg);
}

// 启动时只检查一次：若无文件 / 被篡改 / 已过期 => 让用户输入密码重置
bool MainWindow::initialCheck()
{
    bool ok = false;
    licStartTime = readLicenseFile(ok);

    if(!ok) {
        // 文件不存在或被破坏 => 立即提示
        QMessageBox::warning(this, "提示", "授权文件缺失或损坏，需要最高权限初始化。");
        if(!requestAdminPassword("请输入最高权限密码：", this)) {
            QMessageBox::critical(this, "错误", "密码错误");
            //qApp->quit();
            //std::exit(0);
            return false;
        }

        //验证通过 写入lic
        if(!writeLicenseFile(toolCurrentTime)) {
            QMessageBox::critical(this, "错误", "写入授权文件失败 软件即将关闭");
            //qApp->quit();
            //std::exit(0);
            return false;
        }
    }

    // 设置新时间
    InternetTimeFetcher fetcher;
    fetcher.fetchInternetTime(toolCurrentTime);
    if(!toolCurrentTime.isValid()){
        QMessageBox::critical(this, "错误", "无法联网获取时间");
        //qApp->quit();
        //std::exit(0);
        return false;
    }

    // 检查是否已经过期
    if(toolCurrentTime > licStartTime.addMSecs(MAX_MSECS)){
        qWarning() << QString("toolCurrentTime > licStartTime.addMSecs(MAX_MSECS)");

        this->m_mainBackendWorkerNew.stopWorker();
        this->m_autoChangeWallpaperBackendWorker.stopWorker();

        // 文件不存在或被破坏 => 立即提示
        QMessageBox::warning(this, "提示", "软件有效期已过 后台线程均停止 要求输入密码");
        if(!requestAdminPassword("请输入最高权限密码：", this)) {
            QMessageBox::critical(this, "错误", "密码错误 软件即将关闭");
            //qApp->quit();
            //std::exit(0);
            return false;
        }

        if(!writeLicenseFile(toolCurrentTime)) {
            QMessageBox::critical(this, "错误", "写入授权文件失败 软件即将关闭");
            //qApp->quit();
            //std::exit(0);
            return false;
        }

        // 成功更新时间
        licStartTime = toolCurrentTime;
        ui->generalPanel->setActivateStartTime(licStartTime);
        ui->generalPanel->setExpirationTime(toolCurrentTime.addMSecs(MAX_MSECS));
        ui->generalPanel->setCurrentTime(toolCurrentTime);
    }

    return true;
}

void MainWindow::onCheckLic()
{
    qInfo() << QString("MainWindow::onCheckLic()");
    if(toolCurrentTime > licStartTime.addMSecs(MAX_MSECS)){
        qWarning() << QString("toolCurrentTime > licStartTime.addMSecs(MAX_MSECS)");
        this->m_mainBackendWorkerNew.stopWorker();
        this->m_autoChangeWallpaperBackendWorker.stopWorker();

        // 文件不存在或被破坏 => 立即提示
        QMessageBox::warning(this, "提示", "软件有效期已过 后台线程均停止 要求输入密码");
        if(!requestAdminPassword("请输入最高权限密码：", this)) {
            QMessageBox::critical(this, "错误", "密码错误 软件即将关闭");
            //qApp->quit();
            std::exit(0);
        }

        if(!writeLicenseFile(toolCurrentTime)) {
            QMessageBox::critical(this, "错误", "写入授权文件失败 软件即将关闭");
            //qApp->quit();
            std::exit(0);
        }

        // 成功更新时间
        QMessageBox::information(this, "成功延期", "请手动继续后台线程");

        licStartTime = toolCurrentTime;
        ui->generalPanel->setActivateStartTime(licStartTime);
        ui->generalPanel->setExpirationTime(toolCurrentTime.addMSecs(MAX_MSECS));
        ui->generalPanel->setCurrentTime(toolCurrentTime);

    }
}

void MainWindow::onUpdateTimeOnUI(){
    //qInfo() << QString("onUpdateTimeOnUI() ");
    // 获取自上次重置以来的时间（以毫秒为单位）
    qint64 elapsedMs = softElapsedTime.restart();
    // 将毫秒数转换为秒数累加到 QDateTime
    toolCurrentTime = toolCurrentTime.addMSecs(elapsedMs);
    ui->generalPanel->setCurrentTime(toolCurrentTime);
    // qInfo() << QString("toolCurrentTime = %1 += %2 ms").arg(toolCurrentTime.toString("yyyy-MM-dd hh:mm:ss")).arg(elapsedMs);

}

bool MainWindow::isTempActivate(){
    bool ok;
    QString input = QInputDialog::getText(
                nullptr,
                "最后一次机会",
                "请输入低等级口令（已加密的内容）：",
                QLineEdit::Password,
                "",  // 默认值为空
                &ok
                );

    if (ok && !input.isEmpty()) {
        // 计算输入密码的哈希
        QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256);

        // 比较字节数组
        if (hash.size() == static_cast<int>(expectedHash.size())) {
            bool match = true;
            for (size_t i = 0; i < expectedHash.size(); ++i) {
                if (static_cast<uint8_t>(hash.at(i)) != expectedHash[i]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                // 设置 toolCurrentTime 为当前本地时间
                toolCurrentTime = QDateTime::currentDateTime();
                QMessageBox::information(nullptr, "成功", "激活成功！当前时间已设置。");
                return true; // 激活成功，退出检查逻辑
            }
        }
    }

    QMessageBox::critical(nullptr, "错误", "口令验证失败！");
    return false; // 激活成功，退出检查逻辑
}

