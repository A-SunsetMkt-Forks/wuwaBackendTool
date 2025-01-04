#ifndef FASTSWITCHFIGHTBACKENDWORKER_H
#define FASTSWITCHFIGHTBACKENDWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QtGlobal>

class FastSwitchFightBackendWorker : public QObject
{
    Q_OBJECT
public:
    explicit FastSwitchFightBackendWorker(QObject *parent = nullptr);
    bool isBusy();
    void stopWorker();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突
    QMutex m_mutex;

signals:

public slots:
    void startFight();
};

#endif // FASTSWITCHFIGHTBACKENDWORKER_H
