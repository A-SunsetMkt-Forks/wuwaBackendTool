#ifndef TEAMIDXRECOGNITIONMONITOR_H
#define TEAMIDXRECOGNITIONMONITOR_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/towerbattledatamanager.h"
#include "towerBattle/teamidxrecognitor.h"
#include "utils.h"

class TeamIdxRecognitionMonitor : public QObject
{
    Q_OBJECT
public:
    explicit TeamIdxRecognitionMonitor(QObject *parent = nullptr);
    void stop();
    bool isBusy();

signals:
    void updateCurrentTeamIdx(const int& idx);

public slots:
    void on_start_teamIdxRecognitorMonitor(const TeamIdxRecognitor* recognitor);

private:
    QAtomicInt m_isBusy;
};

#endif // TEAMIDXRECOGNITIONMONITOR_H
