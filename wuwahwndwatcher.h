#ifndef WUWAHWNDWATCHER_H
#define WUWAHWNDWATCHER_H

#include <QObject>
#include <QAtomicInt>
#include "utils.h"

// 定时监督wuwa是否挂了，如果挂了则要求重启
// 兼职月卡判断

class WuwaHWNDWatcher : public QObject
{
    Q_OBJECT
public:
    explicit WuwaHWNDWatcher(QObject *parent = nullptr);
    bool isBusy();
    void stopWorker();

signals:
    void stopMainBackendWorker();  // 不能继续跑了 回传停止信号

public slots:
    void startWatcher();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突
    const int m_loopMsecs = 5000; // 5秒循环判断一次
};

#endif // WUWAHWNDWATCHER_H
