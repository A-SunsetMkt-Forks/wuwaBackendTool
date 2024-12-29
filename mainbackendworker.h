#ifndef MAINBACKENDWORKER_H
#define MAINBACKENDWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include <QDateTime>

// 获取窗体 句柄
#include <windows.h>
#include <wingdi.h>

// 通用工具
#include "utils.h"

#include "settingparams.h"

class MainBackendWorker : public QObject
{
    Q_OBJECT

public:
    explicit MainBackendWorker(QObject* parent = nullptr);  // 构造函数需要接受 QObject* 类型的参数
    bool isBusy();
    void stopWorker();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突

    void skipMonthCard();    // 领取月卡
    void pickUpEchoIsBossDied();  // 捡声骸后判断boss是否死亡

signals:
    void startTest1Done(const bool &isNormalEnd, const QString& msg);

    void startSpecialBossDone(const bool& isNormalEnd, const QString& msg, const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

public slots:
    void onStartTest1();

    // 特殊boss
    void onStartSpecialBoss(const SpecialBossSetting& setting, const RebootGameSetting& rebootGameSetting);

};

#endif // MAINBACKENDWORKER_H
