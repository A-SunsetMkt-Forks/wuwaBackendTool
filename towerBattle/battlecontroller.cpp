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

    while(isBusy()){
        if(team[1] == TowerBattleDataManager::Charactor::Camellya && \
                team[2] == TowerBattleDataManager::Charactor::Sanhua &&\
                team[2] == TowerBattleDataManager::Charactor::Shorekeeper){
            bool ret = Camellya_Sanhua_Shorekeeper();
            emit battleDone(ret, m_errMsg);
            m_isBusy.store(0);
            return;
        }
        else{

            m_errMsg = QString("不支持当前配队");
            qCritical() << m_errMsg;
            emit battleDone(false, m_errMsg);
            m_isBusy.store(0);
            return;
        }



    }


    m_errMsg = QString("被用户中断");
    emit battleDone(true, m_errMsg);
    m_isBusy.store(0);
    return;
}


bool BattleController::Camellya_Sanhua_Shorekeeper(){
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
    // 椿释放 E
    while(isBusy() && dataManager.getResonanceSkillReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'E', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // 切换散华
    while(isBusy() && dataManager.getCurrentIndex() != 2  ){
        Utils::keyPress(Utils::hwnd, '2', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // 散华释放E
    while(isBusy() && dataManager.getResonanceSkillReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'E', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // 散华释放R
    while(isBusy() && dataManager.getResonanceLiberationReady() > 0.8 ){
        Utils::keyPress(Utils::hwnd, 'R', 1);
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }

    // 散华Z
    Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONDOWN, 1, 1);
    while(isBusy() && dataManager.getResonanceCircuit() < 1.0 ){
        QThread::msleep(basicWaitMs + QRandomGenerator::global()->generateDouble() * 50);
    }
    if(!isBusy()){
        m_errMsg = QString("被用户中断");
        return true;
    }
    // 松开Z
    Utils::sendMouseToWindow(Utils::hwnd, WM_LBUTTONUP, 1, 1);


    return true;
}
