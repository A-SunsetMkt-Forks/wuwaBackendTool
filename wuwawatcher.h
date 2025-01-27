#ifndef WUWAWATCHER_H
#define WUWAWATCHER_H

#include <QObject>
#include <QDebug>
#include <QAtomicInt>
#include "utils.h"


class WuwaWatcher : public QObject
{
    Q_OBJECT
public:
    explicit WuwaWatcher(QObject *parent = nullptr);
    bool isBusy();

signals:
    void rebootGame();

public slots:
    // 停止工作
    void stopWorker();
    // 开始监测
    void startWatcher();

private:
    QAtomicInt m_isRunning;


};

#endif // WUWAWATCHER_H
