#include "teamidxrecognitor.h"

TeamIdxRecognitor::TeamIdxRecognitor(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}

void TeamIdxRecognitor::stop(){
    m_isBusy.store(0);
}


bool TeamIdxRecognitor::isBusy(){
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void TeamIdxRecognitor::on_start_teamIdxRecognition(){

}

