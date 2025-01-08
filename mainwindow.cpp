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

    // 启动自动切换背景图线程
    m_autoChangeWallpaperBackendWorker.moveToThread(&m_autoChangeWallpaperBackendThread);
    m_autoChangeWallpaperBackendThread.start();

    // 启动后台线程
    m_mainBackendWorker.moveToThread(&m_mainBackendThread);
    m_mainBackendThread.start();

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

    ui->tabWidget->setStyleSheet(
        "QTabWidget::pane { background: transparent; }"
        "QTabBar::tab { background: transparent; }"
        "QStackedWidget { background: transparent; }"
    );

    // 特别处理 QTabWidget
    QTabWidget* tabWidget = ui->centralWidget->findChild<QTabWidget*>();
    if (tabWidget) {
        tabWidget->setAttribute(Qt::WA_StyledBackground, true);
        tabWidget->setStyleSheet("background: transparent;");

        // 设置 QTabBar 透明
        QTabBar* tabBar = tabWidget->findChild<QTabBar*>();
        if (tabBar) {
            qDebug() << QString("set tabBar %1 as background: transparent;").arg(tabBar->objectName());
            tabBar->setAttribute(Qt::WA_StyledBackground, true);
            tabBar->setStyleSheet("background: transparent;");

        }

        // 设置 QStackedWidget 透明
        QStackedWidget* stackedWidget = tabWidget->findChild<QStackedWidget*>();
        if (stackedWidget) {
            qDebug() << QString("set stackedWidget %1 as background: transparent;").arg(stackedWidget->objectName());
            stackedWidget->setAttribute(Qt::WA_StyledBackground, true);
            stackedWidget->setStyleSheet("background: transparent;");
            setWidgetsTransparent(stackedWidget);
        }

        // 设置每个标签页的内容透明
        for (int i = 0; i < tabWidget->count(); ++i) {
            QWidget* page = tabWidget->widget(i);
            if (page) {
                qDebug() << QString("set page %1 as background: transparent;").arg(page->objectName());
                setWidgetsTransparent(page);
            }
        }
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
    if (img.isNull()) {
        qWarning() << "Image is null. Cannot set as wallpaper.";
        return;
    }



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

void MainWindow::setWidgetsTransparent(QWidget* widget) {
    if (!widget) return;

    widget->setAttribute(Qt::WA_StyledBackground, true);
    widget->setStyleSheet("background: transparent;");

    const QList<QWidget*> children = widget->findChildren<QWidget*>();
    for (QWidget* child : children) {
        setWidgetsTransparent(child);
    }
}

