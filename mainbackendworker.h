#ifndef MAINBACKENDWORKER_H
#define MAINBACKENDWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>

class MainBackendWorker : public QObject
{
    Q_OBJECT

public:
    explicit MainBackendWorker(QObject* parent = nullptr);  // 构造函数需要接受 QObject* 类型的参数
    bool isBusy();
    void stopWorker();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突

signals:
    void startTest1Done(const bool &isNormalEnd, const QString& errMsg);

public slots:

    void onStartTest1();

};

#endif // MAINBACKENDWORKER_H
