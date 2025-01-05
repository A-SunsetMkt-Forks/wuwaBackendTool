#include "fastswitchfightbackendworker.h"

FastSwitchFightBackendWorker::FastSwitchFightBackendWorker(QObject *parent) : QObject(parent)
{
    m_isRunning.store(0);

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
    qInfo() << QString("FastSwitchFightBackendWorker::startFight 速切人战斗结束");
}

void FastSwitchFightBackendWorker::startFight(){
    m_isRunning.store(1);
    qInfo() << QString("FastSwitchFightBackendWorker::startFight 速切人战斗开始");
    bool isQuickMacroType = false;
    while(isBusy()){
        if(isQuickMacroType){
            Utils::keyPress(Utils::hwnd, 'R', 1);
            Sleep(100);
            Utils::keyPress(Utils::hwnd, 'E', 1);
            Sleep(100);
            Utils::keyPress(Utils::hwnd, 'Q', 1);
            Sleep(100);

            Utils::keyPress(Utils::hwnd, '1', 1);  // 切人
            Sleep(300);
            Utils::keyPress(Utils::hwnd, '2', 1);  // 切人
            Sleep(300);
            Utils::keyPress(Utils::hwnd, '3', 1);  // 切人
            Sleep(300);

            Utils::clickWindowClient(Utils::hwnd);
            Sleep(100);
            Utils::clickWindowClient(Utils::hwnd);
            Sleep(100);

        }
        else{
            const int teamSize = 3;
            for(int i = 0; i < teamSize && isBusy(); i++){
                Utils::keyPress(Utils::hwnd, i + 1 + '0', 1);  // 切人
                Sleep(300);
                Utils::keyPress(Utils::hwnd, 'R', 1);
                Sleep(100);
                Utils::keyPress(Utils::hwnd, 'E', 1);
                Sleep(100);
                Utils::keyPress(Utils::hwnd, 'Q', 1);
                Sleep(100);
                Utils::clickWindowClient(Utils::hwnd);
                Sleep(100);
                Utils::clickWindowClient(Utils::hwnd);
                Sleep(100);
            }

        }

        if(!isBusy()){
            break;
        }
    }

}

void FastSwitchFightBackendWorker::testStartFight(){
    m_isRunning.store(1);
    qInfo() << QString("FastSwitchFightBackendWorker::testStartFight 测试速切人战斗开始");
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        qWarning() << QString("FastSwitchFightBackendWorker::testStartFight()init wuwa hwnd failed");
        m_isRunning.store(0);
        return;
    }
    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();
    if(!AttachThreadInput(currentThreadId, threadId, TRUE)){
        qWarning() << QString("FastSwitchFightBackendWorker::testStartFight() AttachThreadInput failed, error code %1").arg(GetLastError());
        m_isRunning.store(0);
        AttachThreadInput(currentThreadId, threadId, FALSE);
        return;
    }

    // 激活窗口
    SendMessage(Utils::hwnd, WM_ACTIVATE, WA_ACTIVE, 0);   // 只要这行可以 只在一开始弹出 后续放到后台也可以传
    Utils::middleClickWindowClientArea(Utils::hwnd, Utils::CLIENT_WIDTH / 2, Utils::CLIENT_HEIGHT / 2);
    bool isQuickMacroType = false;
    while(isBusy()){
        if(isQuickMacroType){
            Utils::keyPress(Utils::hwnd, 'R', 1);
            Sleep(100);
            Utils::keyPress(Utils::hwnd, 'E', 1);
            Sleep(100);
            Utils::keyPress(Utils::hwnd, 'Q', 1);
            Sleep(100);

            Utils::keyPress(Utils::hwnd, '1', 1);  // 切人
            Sleep(300);
            Utils::keyPress(Utils::hwnd, '2', 1);  // 切人
            Sleep(300);
            Utils::keyPress(Utils::hwnd, '3', 1);  // 切人
            Sleep(300);

            Utils::clickWindowClient(Utils::hwnd);
            Sleep(100);
            Utils::clickWindowClient(Utils::hwnd);
            Sleep(100);

        }
        else{
            const int teamSize = 3;
            for(int i = 0; i < teamSize && isBusy(); i++){
                Utils::keyPress(Utils::hwnd, i + 1 + '0', 1);  // 切人
                Sleep(300);
                Utils::keyPress(Utils::hwnd, 'R', 1);
                Sleep(100);
                Utils::keyPress(Utils::hwnd, 'E', 1);
                Sleep(100);
                Utils::keyPress(Utils::hwnd, 'Q', 1);
                Sleep(100);
                Utils::clickWindowClient(Utils::hwnd);
                Sleep(100);
                Utils::clickWindowClient(Utils::hwnd);
                Sleep(100);
            }

        }

        if(!isBusy()){
            break;
        }
    }

    AttachThreadInput(currentThreadId, threadId, FALSE);
    return;
}
