#include "battlecontroller.h"

BattleController::BattleController(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}


void BattleController::stop(){
    m_isBusy.store(0);
}


bool BattleController::isBusy() const {
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void BattleController::on_start_battleController(){
    m_isBusy.store(1);
    // 初始化 验证截图
    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    QThread::msleep(250);
    cv::Mat capImg = dataManager.getLastCapImg();
    if(capImg.cols != 1280 || capImg.rows != 720){
        m_errMsg = QString("必须保证游戏画面为严格1280 720.目前分辨率 %1 %2，请去windows桌面右键-显示设置-调节DPI为100% 然后重启本软件和游戏").arg(capImg.cols).arg(capImg.rows);
        qCritical() << m_errMsg;
        emit battleDone(false, m_errMsg);
        m_isBusy.store(0);
        return;
    }

    // 获取当前配队
    QVector<TowerBattleDataManager::Charactor> team = dataManager.getCurrentTeamVec();
    double tickRate = dataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;

    // 激活窗口
    DWORD threadId = GetWindowThreadProcessId(Utils::hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();
    if(!AttachThreadInput(currentThreadId, threadId, TRUE) ){
        m_errMsg = QString("激活窗口失败 检查是否以管理员身份运行本工具");
        qCritical() << m_errMsg;
        emit battleDone(false, m_errMsg);
        AttachThreadInput(currentThreadId, threadId, FALSE);
        m_isBusy.store(0);
        return;
    }

    while(isBusy()){
        if(team[1] == TowerBattleDataManager::Charactor::Camellya && \
                team[2] == TowerBattleDataManager::Charactor::Sanhua &&\
                team[3] == TowerBattleDataManager::Charactor::Shorekeeper){
            bool ret = Camellya_Sanhua_Shorekeeper();
            emit battleDone(ret, m_errMsg);
            AttachThreadInput(currentThreadId, threadId, FALSE);
            m_isBusy.store(0);
            return;
        }
        else{
            m_errMsg = QString("不支持当前配队");
            qCritical() << m_errMsg;
            emit battleDone(false, m_errMsg);
            AttachThreadInput(currentThreadId, threadId, FALSE);
            m_isBusy.store(0);
            return;
        }



    }


    m_errMsg = QString("被用户中断");
    emit battleDone(true, m_errMsg);
    AttachThreadInput(currentThreadId, threadId, FALSE);
    m_isBusy.store(0);
    return;
}


bool BattleController::Camellya_Sanhua_Shorekeeper(){
    int errCount = 0;
    // ===================== 热身轴
    // 切椿EQ
    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    // 要求切换至1
    while(isBusy() && dataManager.getCurrentIndex() != 1 ){
        Utils::keyPress(Utils::hwnd, '1', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }
    qInfo() << QString("完成切换为1椿");
    // 椿释放 E
    qInfo() << QString("椿准备释放E技能");
    Utils::keyPress(Utils::hwnd, 'E', 1);
    while(isBusy() && dataManager.getResonanceSkillReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'E', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }
    qInfo() << QString("椿E释放完毕");

    // 切换散华
    qInfo() << QString("准备切换为2散");
    while(isBusy() && dataManager.getCurrentIndex() != 2  ){
        Utils::keyPress(Utils::hwnd, '2', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }
    qInfo() << QString("完成切换为2散");

    // 散华释放E
    qInfo() << QString("准备释放E技能");
    while(isBusy() && dataManager.getResonanceSkillReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'E', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }
    qInfo() << QString("完成释放E技能");

    // 散华释放R
    qInfo() << QString("准备释放R技能");
    while(isBusy() && dataManager.getResonanceLiberationReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'R', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }
    qInfo() << QString("完成释放R技能");

    // 散华Z
    Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONDOWN, 1, 1);
    int sanhuaVote = 0;
    while(isBusy() ){
        if(dataManager.getResonanceCircuit() < 1.0 ){
            sanhuaVote = 0;
            QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
        }
        else{
            sanhuaVote++;
            if(sanhuaVote > 2){
                // 连续3票 认为OK
                break;
            }
            QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
        }


    }
    if(!isBusy()){
        Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONUP, 1, 1);
        m_errMsg = QString("被用户中断");
        return true;
    }

    //QThread::msleep(1400 + QRandomGenerator::global()->generateDouble() * 50);
    // 松开Z
    Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONUP, 1, 1);

    // 切守
    while(isBusy() && dataManager.getCurrentIndex() !=3 ){
        Utils::keyPress(Utils::hwnd, '3', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // 守打满两次回路
    for(int i = 0; i< 2; i++){
        // 守一直A 直到回路满了
        while(isBusy() && dataManager.getResonanceCircuit() < 1.0 ){
            Utils::clickWindowClientArea(Utils::hwnd, 1, 1);
            QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
        }
        if(!isBusy()){
            m_errMsg = QString("被用户中断");
            return true;
        }

        // 守 Z 清空回路
        Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONDOWN, 1, 1);
        while(isBusy() && dataManager.getResonanceCircuit() >= 1.0 ){
            QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
        }
        if(!isBusy()){
            Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONUP, 1, 1);
            m_errMsg = QString("被用户中断");
            return true;
        }
        Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONUP, 1, 1);
    }

    // 守 EQR
    // 声骸
    Utils::keyPress(Utils::hwnd, 'Q', 1);
    QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);

    // E
    Utils::keyPress(Utils::hwnd, 'E', 1);
    while(isBusy() && dataManager.getResonanceSkillReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'E', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // R
    Utils::keyPress(Utils::hwnd, 'R', 1);
    while(isBusy() && dataManager.getResonanceLiberationReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'R', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // 检测是否满协奏 如果没满 继续A两下
    while(isBusy() && dataManager.getConcertoEnergy() < 1.0 ){
        Utils::clickWindowClientArea(Utils::hwnd, 1, 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // 多A一次确保满协奏
    Utils::clickWindowClientArea(Utils::hwnd, 1, 1);
    QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);

    // 切椿
    while(isBusy() && dataManager.getCurrentIndex() !=1 ){
        Utils::keyPress(Utils::hwnd, '1', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }




    return true;
}
