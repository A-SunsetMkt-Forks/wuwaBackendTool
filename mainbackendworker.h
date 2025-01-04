#ifndef MAINBACKENDWORKER_H
#define MAINBACKENDWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QtGlobal>
#include <QThread>

// 获取窗体 句柄
#include <windows.h>
#include <wingdi.h>

// 通用工具
#include "utils.h"

// 后台运行的参数
#include "settingparams.h"

// 速切战斗子线程
#include "fastswitchfightbackendworker.h"
#include "noswitchfightbackendworker.h"

class MainBackendWorker : public QObject
{
    Q_OBJECT

public:
    explicit MainBackendWorker(QObject* parent = nullptr);  // 构造函数需要接受 QObject* 类型的参数
    ~MainBackendWorker();
    bool isBusy();
    void stopWorker();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突
    QMutex m_mutex;

    void skipMonthCard();    // 领取月卡
    void pickUpEchoIsBossDied();  // 捡声骸后判断boss是否死亡
    void quitRover();   // 退出无妄者

signals:
    // 完成测试后台流程
    void startTest1Done(const bool &isNormalEnd, const QString& msg);
    // 完成副本BOSS单刷
    void startSpecialBossDone(const bool& isNormalEnd, const QString& msg, const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

    //  两种战斗子线程
    void startNoSwitchFightWorker();  // 开启不切人战斗线程
    void startFastSwitchFightWorker();  // 开启速切战斗线程

public slots:
    // 简单测试 调试用
    void onStartTest1();

    // 特殊boss 角和无妄者的刷取
    void onStartSpecialBoss(const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

private:
    // 按键精灵的执行过程
    // 副本BOSS单刷 363行 捡声骸判断
    bool isPickUpEcho(const int& pickUpEchoRange);

    // 副本BOSS单刷  398行 锁定敌人
    bool lockEnemy();

    // 副本BOSS单刷 479行 不切人判断式
    bool noSwitchExp(const float& noSwitchCondition);

    // 副本BOSS单刷 524行 复苏
    bool revive();

    // 副本BOSS单刷 565 战斗代码
    bool fight(const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

    // 副本BOSS单刷 重启
    bool reboot(const RebootGameSetting& rebootGameSetting);


    //以下为若干GLOBAL全局变量
    static QTime rebootCalcStartTime;   // 重启计算开始时间
    static QTime rebootCalcEndTime;    // 重启计算结束时间
    static QTime dungeonStartTime;     // 副本战斗开始时间
    static QTime dungeonEndTime;       // 副本战斗结束时间
    static QAtomicInt isContinueFight;   // 重复判定继续战斗
    static QTime battlePickUpTime;  // 战斗时按拾取开始时间
    static QAtomicInt reviveVal; // 复苏变量

    static QAtomicInt startNoSwitchFight;  // 开启不切人战斗
    static QAtomicInt isPickUp;    // 拾取成功
    static QAtomicInt rebootCount;  // 重启计数


    //速切战斗子线程
    QThread m_fastSwitchFightBackendThread;
    FastSwitchFightBackendWorker m_fastSwitchFightBackendWorker;

    // 不切人战斗子线程
    QThread m_noSwitchFightBackendThread;
    NoSwitchFightBackendWorker m_noSwitchFightBackendWorker;

};

#endif // MAINBACKENDWORKER_H
