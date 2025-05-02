#ifndef CONCERTOENERGYJUDGEMONITOR_H
#define CONCERTOENERGYJUDGEMONITOR_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/concertoenergyjudger.h"
#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"

class ConcertoEnergyJudgeMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ConcertoEnergyJudgeMonitor(QObject *parent = nullptr);
    void stop();
    bool isBusy() const ;

signals:
    void updateConcertoEnergy(const double& val);
public slots:
    void on_start_monitor(const ConcertoEnergyJudger* judger);

private:
    QAtomicInt m_isBusy;
};

#endif // CONCERTOENERGYJUDGEMONITOR_H
