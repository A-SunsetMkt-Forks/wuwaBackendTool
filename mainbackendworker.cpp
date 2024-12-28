#include "mainbackendworker.h"

// 实现部分
MainBackendWorker::MainBackendWorker(QObject* parent)
    : QObject(parent)  // 初始化基类 QObject
{
    m_isRunning.store(0);

}

bool MainBackendWorker::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}


void MainBackendWorker::stopWorker(){
    m_isRunning.store(0);
}

void MainBackendWorker::onStartTest1(){
    // 启动程序 UI线程 发信号后 已经更改为忙碌了
    // 不管任何方式退出 都发送信号要求UI更新状态
    m_isRunning.store(1);

    qDebug() << QString("MainBackendWorker::onStartTest1 start...");

}
