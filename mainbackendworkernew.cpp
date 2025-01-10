#include "mainbackendworkernew.h"

MainBackendWorkerNew::MainBackendWorkerNew(QObject *parent) : QObject(parent)
{

}

bool MainBackendWorkerNew::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void MainBackendWorkerNew::stopWorker(){
    m_isRunning.store(0);
    qInfo() << QString("MainBackendWorkerNew 线程结束");
}
