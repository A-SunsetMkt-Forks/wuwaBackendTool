#ifndef RESONANCECIRCUITJUDGEMONITOR_H
#define RESONANCECIRCUITJUDGEMONITOR_H

#include <QObject>
#include <QAtomicInt>
#include <QThread>

#include "utils.h"
#include "towerBattle/towerbattledatamanager.h"
#include "towerBattle/resonancecircuitjudger.h"

class ResonanceCircuitJudgeMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ResonanceCircuitJudgeMonitor(QObject *parent = nullptr);
    void stop();
    bool isBusy();

signals:
    void updateResonanceCircuitStatus(const double& value);

public slots:
    void on_start_monitor(const ResonanceCircuitJudger* judger);

private:
    QAtomicInt m_isBusy;
};

#endif // RESONANCECIRCUITJUDGEMONITOR_H
