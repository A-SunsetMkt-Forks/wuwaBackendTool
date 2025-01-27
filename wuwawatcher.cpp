#include "wuwawatcher.h"

WuwaWatcher::WuwaWatcher(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);
}

bool WuwaWatcher::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void WuwaWatcher::stopWorker(){
    m_isRunning.store(0);
    qInfo() << QString("WuwaWatcher 线程结束");
}

void WuwaWatcher::startWatcher(){
    m_isRunning.store(1);
    // 每100ms监督是否还活着
    bool isWuwaAlive = Utils::isWuwaRunning();
    while(isBusy()){
        Sleep(100);

        isWuwaAlive = Utils::isWuwaRunning();
        if(!isWuwaAlive){
            break;
        }
    }

    // 检查停下来的原因
    if(isWuwaAlive){
        // 用户停止  什么都不做
        stopWorker();
        return;
    }
    else{
        // 需要重启
        qWarning() << QString("检测到鸣潮崩溃 重启游戏");
        emit rebootGame();
        stopWorker();
        return;
    }

}
