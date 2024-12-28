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

    // 初始化句柄
    bool isInit = Utils::initWuwaHwnd();
    if(!isInit){
        emit startTest1Done(false, QString("未能初始化鸣潮窗口句柄"));
        m_isRunning.store(0);
        return;
    }

    // 检查分辨率 可选

    // 尝试后台激活窗口（并不强制将窗口置前）
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // 将线程附加到目标窗口
    // 测试 ESC 点击活动 ESC 往前走
    if (AttachThreadInput(currentThreadId, threadId, TRUE)) {
        // 激活窗口
        SendMessage(Utils::hwnd, WM_ACTIVATE, WA_ACTIVE, 0);   // 只要这行可以 只在一开始弹出 后续放到后台也可以传
        Sleep(1000);
        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 按下ESC
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYDOWN);
        Sleep(300);
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYUP);
        Sleep(500);

        // 检测是否用户要求停止
        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 点击ESC菜单中的 活动
        Sleep(1000);   //等待ESC菜单展开
        Utils::clickWindowClientArea(Utils::hwnd, 641, 145);
        Sleep(500);

        // 检测是否用户要求停止
        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 按下ESC
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYDOWN);
        Sleep(500);

        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYUP);
        Sleep(500);

        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 按下ESC
        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYDOWN);
        Sleep(500);

        Utils::sendKeyToWindow(Utils::hwnd, VK_ESCAPE, WM_KEYUP);
        Sleep(500);

        // 按住W 2秒
        for(int i = 0; i < 4 && isBusy(); i++){
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYDOWN);
            Sleep(500);
            Utils::sendKeyToWindow(Utils::hwnd, 'W', WM_KEYUP);
        }

        if(!isBusy()){
            AttachThreadInput(currentThreadId, threadId, FALSE);
            emit startTest1Done(true, QString("用户停止"));
            m_isRunning.store(0);
            return;
        }

        // 分离线程
        AttachThreadInput(currentThreadId, threadId, FALSE);
        emit startTest1Done(true, QString("正常结束"));
        m_isRunning.store(0);
        return;
    }
    else {
        emit startTest1Done(false, QString("线程附加到鸣潮窗口句柄失败"));
        m_isRunning.store(0);
        qWarning() << "Failed to attach thread input.";
        return;
    }


}
