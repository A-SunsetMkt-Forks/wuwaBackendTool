#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt库
#include <QMainWindow>
#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QScrollBar>
#include <QThread>

// 标准C++库
#include <iostream>

// 获取窗体 句柄
#include <windows.h>
#include <wingdi.h>

// 日志
#include "commonTools/customdelegate.h"
#include "commonTools/debugmessagehandler.h"

#include "utils.h"    //通用工具
#include "generalpanel.h"  // 子面板
#include "mainbackendworker.h"  // 后台线程


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // 注册非原生数据类型 便于传递信号和槽函数
    void registerType();

    QThread m_mainBackendThread;
    MainBackendWorker m_mainBackendWorker;

signals:
    void startTest1();


private slots:
    void on_getGameWin_clicked();

    // 后台发出按键请求
    void on_sendBtn_clicked();

    // 测试 ESC 点击活动 ESC 往前走
    void on_test1_clicked();

    // 响应后台结束的信号
    void onStartTest1Done(const bool &isNormalEnd, const QString& errMsg);

    // 打断一切后台操作
    void on_stopBtn_clicked();
};

#endif // MAINWINDOW_H
