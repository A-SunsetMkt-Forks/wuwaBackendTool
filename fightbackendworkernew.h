#ifndef FIGHTBACKENDWORKERNEW_H
#define FIGHTBACKENDWORKERNEW_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>

#include "utils.h"

class FightBackendWorkerNew : public QObject
{
    Q_OBJECT
public:
    explicit FightBackendWorkerNew(QObject *parent = nullptr);
    bool isBusy();
    void stopWorker();
    void pauseWorker();
    void resumeWorker();

signals:

public slots:
    void onStartFight();

private:
    QAtomicInt m_isRunning;
    QAtomicInt m_isPause;
};

#endif // FIGHTBACKENDWORKERNEW_H
