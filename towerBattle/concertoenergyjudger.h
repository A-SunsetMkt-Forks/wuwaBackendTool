#ifndef CONCERTOENERGYJUDGER_H
#define CONCERTOENERGYJUDGER_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"

class ConcertoEnergyJudger : public QObject
{
    Q_OBJECT
public:
    explicit ConcertoEnergyJudger(QObject *parent = nullptr);

    void stop();
    bool isBusy() const ;

signals:

public slots:
    void on_start_concertoEnergyJudge();

private:
    QAtomicInt m_isBusy;
};

#endif // CONCERTOENERGYJUDGER_H
