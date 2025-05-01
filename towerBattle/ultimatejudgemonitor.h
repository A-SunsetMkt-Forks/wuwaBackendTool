#ifndef ULTIMATEJUDGEMONITOR_H
#define ULTIMATEJUDGEMONITOR_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"


class UltimateJudgeMonitor : public QObject
{
    Q_OBJECT
public:
    explicit UltimateJudgeMonitor(QObject *parent = nullptr);
    void stop();
    bool isBusy() const ;

signals:
    void updateResonanceLiberationReady(const double& isReady);

public slots:
    void on_start_monitor();

private:
    QAtomicInt m_isBusy;


};

#endif // ULTIMATEJUDGEMONITOR_H
