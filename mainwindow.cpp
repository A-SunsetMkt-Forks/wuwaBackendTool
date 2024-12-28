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

    // 启动后台线程
    m_mainBackendWorker.moveToThread(&m_mainBackendThread);
    m_mainBackendThread.start();

    // test1的连接
    connect(this, &MainWindow::startTest1, &this->m_mainBackendWorker, &MainBackendWorker::onStartTest1);
    connect(&this->m_mainBackendWorker, &MainBackendWorker::startTest1Done, this, &MainWindow::onStartTest1Done);

    // 注册全局快捷键 允许快捷停止脚本运行
    registerGlobalHotKey();

    // 安装全局事件过滤器
    hotKeyFilter = new GlobalHotKeyFilter();
    qApp->installNativeEventFilter(hotKeyFilter);

    // 连接热键信号到槽
    connect(hotKeyFilter, &GlobalHotKeyFilter::hotKeyPressed, this, &MainWindow::onHotKeyActivated);

}

MainWindow::~MainWindow()
{
    // 取消 停止脚本快捷键
    unregisterGlobalHotKey(); // 注销全局快捷键
    qApp->removeNativeEventFilter(hotKeyFilter);
    delete hotKeyFilter;

    // 停止后台线程
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

    image.save(QString("capWuwa.png"));
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

void MainWindow::on_sendBtn_clicked(){
    qDebug() << "sendBtnClicked";
    // 中文窗口标题
    QString windowTitle = "鸣潮  ";

    // 转换为宽字符（Unicode）
    //HWND hwnd = FindWindow(nullptr, (LPCWSTR)windowTitle.utf16());
    HWND hwnd = FindWindow(nullptr, windowTitle.toStdWString().c_str());
    if (!hwnd) {
        qWarning() << "Failed to find window with title: " << windowTitle;
        return;
    }
    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 将线程附加到目标窗口
    if (AttachThreadInput(currentThreadId, threadId, TRUE)) {
        // 激活窗口
        SendMessage(hwnd, WM_ACTIVATE, WA_ACTIVE, 0);   // 只要这行可以 只在一开始弹出 后续放到后台也可以传

        Utils::sendKeyToWindow(hwnd, VK_ESCAPE, WM_KEYDOWN);
        Sleep(300);

        Utils::sendKeyToWindow(hwnd, VK_ESCAPE, WM_KEYUP);
        Sleep(300);

        //PostMessage(hwnd, WM_KEYDOWN, VK_MENU, 0);

        Utils::clickWindowClientArea(hwnd, 641, 145);
        Sleep(500);
        //PostMessage(hwnd, WM_KEYUP, VK_MENU, 0);

        //sendKeySequenceToWindow(hwnd);
        //sendKeySequenceToWindow(hwnd);
        //sendKeySequenceToWindow(hwnd);
        //sendKeySequenceToWindow(hwnd);

        // 分离线程
        AttachThreadInput(currentThreadId, threadId, FALSE);
    } else {
        qDebug() << "Failed to attach thread input.";
    }


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

