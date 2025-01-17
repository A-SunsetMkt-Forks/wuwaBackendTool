#ifndef DEBUGBACKENDWORKER_H
#define DEBUGBACKENDWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include "utils.h"

class DebugBackendWorker : public QObject
{
    Q_OBJECT
public:
    explicit DebugBackendWorker(QObject *parent = nullptr);
    bool isBusy();
    void stopWorker();

signals:
    void activateCapDone(const bool& isOK, const QString& msg);

public slots:
    void onStartActivateCap(const QString& imgFilePath, const unsigned int& waitMs);


private:
    QAtomicInt m_isBusy;
    bool isAttached = false;
};

#endif // DEBUGBACKENDWORKER_H
