#ifndef RESONANCESKILLJUDGEMONITOR_H
#define RESONANCESKILLJUDGEMONITOR_H

#include <QObject>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/resonanceskilljudger.h"
#include "utils.h"
#include "towerBattle/towerbattledatamanager.h"

class ResonanceSkillJudgeMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ResonanceSkillJudgeMonitor(QObject *parent = nullptr);
    void stop();
    bool isBusy() const ;

signals:
    void updateResonanceSkillStatus(const double& val);

public slots:
    void on_start_monitor(const ResonanceSkillJudger* judger);

private:
    QAtomicInt m_isBusy;


};

#endif // RESONANCESKILLJUDGEMONITOR_H
