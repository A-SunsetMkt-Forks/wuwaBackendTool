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

// 新开的后台工作线程
#include "mainbackendworkernew.h"

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
#include "globalhotkeyfilter.h"   // 允许全局快捷键 停止脚本程序
#include "settingparams.h"    // 每种应用场景都有自己的一套参数

#include "autochangewallpaperbackendworker.h"   // 自动切换背景图

#include "internettimefetcher.h"  // 获取互联网时间
#include "commonTools/encrypttools.h"   // 最高权限密码

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

    // 启动时检测lic文件
    bool initialCheck();

    // 注册非原生数据类型 便于传递信号和槽函数
    void registerType();

    // 生成默认的声骸锁定参数 并应用到UI
    void genDefaultLockEchoSetting();

    // 生成默认的声骸锁定参数 并应用到UI
    void genDefaultNormalBossSetting();


    // 自动切换背景
    QThread m_autoChangeWallpaperBackendThread;
    AutoChangeWallpaperBackendworker m_autoChangeWallpaperBackendWorker;

    // 新的后台工作线程
    QThread m_mainBackendNewThread;
    MainBackendWorkerNew m_mainBackendWorkerNew;

    // 全局注册快捷键 ALT F12 停止脚本
    GlobalHotKeyFilter* hotKeyFilter;
    const int toggleHotKeyId = 1; // Alt + F12 的热键 ID  用于停止脚本运行
    const int toggleHotKeyIdScrCpy = 2;  // 用于 ALT+P 的快捷键ID  截图到指定地方
    // 注册和取消注册快捷键
    void unregisterGlobalHotKey();
    void registerGlobalHotKey();

signals:

    // 转发信号 要求后台执行 特殊boss
    void startSpecialBoss(const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

    // 开启自动更新壁纸
    void startChangeWallpaper();

    // 要求后台线程锁定声骸
    void startLockEcho(const LockEchoSetting& lockEchoSetting);

    // 要求后台线程轮刷普通boss
    void startNormalBoss(const NormalBossSetting& setting);

private slots:
    // 每1分钟检查 是否过期
    void onCheckLic();

    // 单纯每秒更新时间
    void onUpdateTimeOnUI();

    void on_getGameWin_clicked();


    // 响应 ESC 点击活动 ESC 往前走 结束的信号
    void onStartTest1Done(const bool &isNormalEnd, const QString& msg);

    // 打断一切后台操作
    void on_stopBtn_clicked();

    // 响应外界的快捷键 停止脚本运行
    void onHotKeyActivated(int id);

    // 更新壁纸
    void onSendImageAsWallpaper(const QImage& img);

    // 接收特殊boss脚本启动请求 检查窗口句柄和分辨率 没问题就转发信号
    void checkSpecialBoss(const SpecialBossSetting& setting);

    // 后台特殊boss线程结束
    void onStartSpecialBossDone(const bool& isNormalEnd, const QString& msg, const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

    // 启动声骸锁定
    void on_startLockEcho_clicked();

    // 声骸锁定结束
    void onLockEchoDone(const bool& isNormalEnd, const QString& errMsg, const LockEchoSetting &lockEchoSetting);

    // 启动普通boss
    void on_startNormalBoss_clicked();
    // 普通boss结束
    void onNormalBossDone(const bool& isNormalEnd, const QString& errMsg, const NormalBossSetting &normalBossSetting);

private:
    QTimer checkLicTimer;   // 每60秒检查一次licence  可能不需要了 开启时检查即可
    QTimer updateCurrentDtTimer;  // 每秒更新UI的定时器

    QDateTime toolCurrentTime; // 软件启动通过验证后 获取时间，然后每60秒触发一次验证lic 通过后 将这段时间加上
    QElapsedTimer softElapsedTime;  // 每隔1分钟检测软件运行时间 toolCurrentTime += softElapsedTime.elasped();    softElapsedTime.reset();

    QDateTime licStartTime;  //从验证文件读取的起点时间


    // 临时功能 如果连不上网 允许本地时间
    bool isTempActivate();

};


#endif // MAINWINDOW_H
