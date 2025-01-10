#ifndef MAINBACKENDWORKERNEW_H
#define MAINBACKENDWORKERNEW_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include "utils.h"

struct LockEchoSetting{

};
// 声明为 Qt 元类型
Q_DECLARE_METATYPE(LockEchoSetting)

class MainBackendWorkerNew : public QObject
{
    Q_OBJECT
public:
    explicit MainBackendWorkerNew(QObject *parent = nullptr);
    bool isBusy();
    void stopWorker();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突

    // 尝试进入背包 如果找不到则3次ESC + 找背包   返回true表示被用户打断或找到了背包并点了进去。返回false表示没有找到背包
    bool enterBagInterface();

    // 尝试进入声骸面板
    bool enterEchoInterface();

signals:
    void lockEchoDone(const bool& isNormalEnd, const QString& errMsg, const LockEchoSetting &lockEchoSetting);

public slots:

    // 响应UI要求 开始自动锁定声骸
    void onStartLockEcho(const LockEchoSetting &lockEchoSetting);




private:
    // 常量坐标或ROI区
    const cv::Rect searchBagROI = {178, 2, 85, 69};
};

#endif // MAINBACKENDWORKERNEW_H
