#ifndef ULTIMATEJUDGER_H
#define ULTIMATEJUDGER_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"


class UltimateJudger : public QObject
{
    Q_OBJECT
public:
    explicit UltimateJudger(QObject *parent = nullptr);
    void stop();
    bool isBusy() const ;

signals:

public slots:
    void on_start_ultimateJudge();

private:
    QAtomicInt m_isBusy;
};

#endif // ULTIMATEJUDGER_H
