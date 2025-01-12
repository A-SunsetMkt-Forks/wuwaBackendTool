#include "fightbackendworkernew.h"

FightBackendWorkerNew::FightBackendWorkerNew(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);
    m_isPause.store(0);
}

bool FightBackendWorkerNew::isBusy(){
    if(m_isRunning.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void FightBackendWorkerNew::stopWorker(){
    m_isRunning.store(0);
    m_isPause.store(1);
    qInfo() << QString("FightBackendWorkerNew 战斗子线程 线程结束");
}

void FightBackendWorkerNew::pauseWorker(){
    m_isPause.store(1);
    qInfo() << QString("FightBackendWorkerNew 战斗子线程 暂停工作");
}

void FightBackendWorkerNew::resumeWorker(){
    if(isBusy()){
        m_isPause.store(0);
    }
    qInfo() << QString("FightBackendWorkerNew 战斗子线程 恢复工作");
}


void FightBackendWorkerNew::onStartFight(){
    m_isRunning.store(1);
    m_isPause.store(0);

    qInfo() << QString("FightBackendWorkerNew 战斗子线程 线程开始");
    // 匿名函数（lambda），返回 bool 表示“是否依然需要继续”
    auto checkPause = [this]() -> bool {
        while (m_isPause.load() == 1)
        {
            if (!isBusy()) {
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  //每100ms接收恢复的信号
        }
        // 再多检查一次
        return isBusy();
    };

    const int waitMs = 250;
    while(isBusy()){
        const int teamSize = 3;
        for(int i = 0; i < teamSize && isBusy(); i++){\
            if(!checkPause()) break;
            Utils::keyPress(Utils::hwnd, i + 1 + '0', 1);  // 切人
            Sleep(250);

            if(!checkPause()) break;
            Utils::keyPress(Utils::hwnd, 'R', 1);
            Sleep(250);

            if(!checkPause()) break;
            Utils::keyPress(Utils::hwnd, 'E', 1);
            Sleep(250);

            if(!checkPause()) break;
            Utils::keyPress(Utils::hwnd, 'Q', 1);
            Sleep(250);

            if(!checkPause()) break;
            Utils::clickWindowClient(Utils::hwnd);
            Sleep(250);

            if(!checkPause()) break;
            Utils::clickWindowClient(Utils::hwnd);
            Sleep(250);
        }
    }

    return;
}
