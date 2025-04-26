#ifndef TEAMIDXRECOGNITOR_H
#define TEAMIDXRECOGNITOR_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"

class TeamIdxRecognitor : public QObject
{
    Q_OBJECT
public:
    explicit TeamIdxRecognitor(QObject *parent = nullptr);
    void stop();
    bool isBusy();

signals:

public slots:
    void on_start_teamIdxRecognition();

private:
    QAtomicInt m_isBusy;
};

#endif // TEAMIDXRECOGNITOR_H
