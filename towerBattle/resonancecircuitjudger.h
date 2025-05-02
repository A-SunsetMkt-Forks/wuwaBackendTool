#ifndef RESONANCECIRCUITJUDGER_H
#define RESONANCECIRCUITJUDGER_H

#include <QObject>
#include <QAtomicInt>
#include <QThread>

#include "utils.h"
#include "towerBattle/towerbattledatamanager.h"

class ResonanceCircuitJudger : public QObject
{
    Q_OBJECT
public:
    explicit ResonanceCircuitJudger(QObject *parent = nullptr);
    void stop();
    bool isBusy() const ;

signals:

public slots:
    void on_start_resonance_recognition();

private:
    QAtomicInt m_isBusy;
};

#endif // RESONANCECIRCUITJUDGER_H
