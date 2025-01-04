#include "noswitchfightbackendworker.h"

NoSwitchFightBackendWorker::NoSwitchFightBackendWorker(QObject *parent) : QObject(parent)
{

}

bool NoSwitchFightBackendWorker::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void NoSwitchFightBackendWorker::stopWorker(){
    m_isRunning.store(0);
}
