#include "fastswitchfightbackendworker.h"

FastSwitchFightBackendWorker::FastSwitchFightBackendWorker(QObject *parent) : QObject(parent)
{

}

bool FastSwitchFightBackendWorker::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void FastSwitchFightBackendWorker::stopWorker(){
    m_isRunning.store(0);
}

