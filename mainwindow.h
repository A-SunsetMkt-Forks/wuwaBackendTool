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
#include <QStackedWidget>

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
#include "speicalbosswidget.h"   // 特殊boss面板
#include "mainbackendworker.h"  // 后台线程
#include "globalhotkeyfilter.h"   // 允许全局快捷键 停止脚本程序
#include "settingparams.h"    // 每种应用场景都有自己的一套参数

#include "autochangewallpaperbackendworker.h"   // 自动切换背景图

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

    // 后台工作线程和后台工作器
    QThread m_mainBackendThread;
    MainBackendWorker m_mainBackendWorker;

    // 自动切换背景
    QThread m_autoChangeWallpaperBackendThread;
    AutoChangeWallpaperBackendworker m_autoChangeWallpaperBackendWorker;

    // 全局注册快捷键 ALT F12 停止脚本
    GlobalHotKeyFilter* hotKeyFilter;
    const int toggleHotKeyId = 1; // Alt + F12 的热键 ID
    // 注册和取消注册快捷键
    void unregisterGlobalHotKey();
    void registerGlobalHotKey();

signals:
    void startTest1();

    // 转发信号 请求后台线程执行
    void startSpecialBoss(const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

    // 开启自动更新壁纸
    void startChangeWallpaper();

private slots:
    void on_getGameWin_clicked();


    // 测试 ESC 点击活动 ESC 往前走
    void on_test1_clicked();

    // 响应后台结束的信号
    void onStartTest1Done(const bool &isNormalEnd, const QString& msg);

    // 打断一切后台操作
    void on_stopBtn_clicked();

    // 响应外界的快捷键 停止脚本运行
    void onHotKeyActivated(int id);

    // 接收特殊boss脚本启动请求 检查窗口句柄和分辨率 没问题就转发信号
    void checkSpecialBoss(const SpecialBossSetting& setting);

    // 后台线程结束
    void onStartSpecialBossDone(const bool& isNormalEnd, const QString& msg, const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

    // 更新壁纸
    void onSendImageAsWallpaper(const QImage& img);
    void setWidgetsTransparent(QWidget* widget);

};


#endif // MAINWINDOW_H
