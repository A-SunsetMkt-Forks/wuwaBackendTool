#ifndef BATTLECONTROLLER_H
#define BATTLECONTROLLER_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>
#include <QRandomGenerator>

#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"


class BattleController : public QObject
{
    Q_OBJECT
public:
    explicit BattleController(QObject *parent = nullptr);

    void stop();
    bool isBusy() const ;

signals:
    void battleDone(bool isOK, QString errMsg);

public slots:
    void on_start_battleController();

private:
    QAtomicInt m_isBusy;
    QString m_errMsg;

    const double basicWaitMs = 100;
    const double rangeWaitMs = 50;  // 等待时间 100~150ms随机


    //椿散守
    bool Camellya_Sanhua_Shorekeeper();

};

#endif // BATTLECONTROLLER_H
