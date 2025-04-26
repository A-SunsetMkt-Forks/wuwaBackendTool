#include "TowerBattleDataManager.h"


TowerBattleDataManager& TowerBattleDataManager::Instance() {
    // C++11 起局部静态变量初始化线程安全
    static TowerBattleDataManager instance;
    return instance;
}

// 私有构造函数（可在此添加初始化逻辑）
TowerBattleDataManager::TowerBattleDataManager() {
    // 初始化代码
}

// 私有析构函数（可在此添加清理逻辑）
TowerBattleDataManager::~TowerBattleDataManager() {
    // 清理代码
}

//======= 图像数据 =======//
const cv::Mat& TowerBattleDataManager::getLastCapImg() const {
    QReadLocker locker(&readwriteLocker);
    return m_lastCapImg;
}

void TowerBattleDataManager::setLastCapImg(const cv::Mat& newImg) {
    QWriteLocker locker(&readwriteLocker);
    newImg.copyTo(m_lastCapImg); // 深拷贝保证线程安全
}

//======= 采集参数 =======//
int TowerBattleDataManager::getCapFrameRate() const {
    QReadLocker locker(&readwriteLocker);
    return m_cap_frame_rate;
}

void TowerBattleDataManager::setCapFrameRate(int rate) {
    QWriteLocker locker(&readwriteLocker);
    m_cap_frame_rate = rate;
}

int TowerBattleDataManager::getTickRate() const {
    QReadLocker locker(&readwriteLocker);
    return m_tick_rate;
}

void TowerBattleDataManager::setTickRate(int rate) {
    QWriteLocker locker(&readwriteLocker);
    m_tick_rate = rate;
}

//======= 角色状态 =======//
int TowerBattleDataManager::getCurrentIndex() const {
    QReadLocker locker(&readwriteLocker);
    return current_idx;
}

void TowerBattleDataManager::setCurrentIndex(int idx) {
    QWriteLocker locker(&readwriteLocker);
    current_idx = idx;
}

bool TowerBattleDataManager::isResonanceSkillReady() const {
    QReadLocker locker(&readwriteLocker);
    return resonance_skill_ready;
}

void TowerBattleDataManager::setResonanceSkillReady(bool ready) {
    QWriteLocker locker(&readwriteLocker);
    resonance_skill_ready = ready;
}

bool TowerBattleDataManager::isResonanceLiberationReady() const {
    QReadLocker locker(&readwriteLocker);
    return resonance_liberation_ready;
}

void TowerBattleDataManager::setResonanceLiberationReady(bool ready) {
    QWriteLocker locker(&readwriteLocker);
    resonance_liberation_ready = ready;
}

double TowerBattleDataManager::getResonanceCircuit() const {
    QReadLocker locker(&readwriteLocker);
    return resonance_circuit;
}

void TowerBattleDataManager::setResonanceCircuit(double value) {
    QWriteLocker locker(&readwriteLocker);
    resonance_circuit = value;
}

bool TowerBattleDataManager::isEchoSkillReady() const {
    QReadLocker locker(&readwriteLocker);
    return echo_skill_ready;
}

void TowerBattleDataManager::setEchoSkillReady(bool ready) {
    QWriteLocker locker(&readwriteLocker);
    echo_skill_ready = ready;
}

bool TowerBattleDataManager::isExplorerToolReady() const {
    QReadLocker locker(&readwriteLocker);
    return explorer_tool_ready;
}

void TowerBattleDataManager::setExplorerToolReady(bool ready) {
    QWriteLocker locker(&readwriteLocker);
    explorer_tool_ready = ready;
}

void TowerBattleDataManager::setCurrentTeamVec(const QVector<TowerBattleDataManager::Charactor> &currentTeamVec){
    QWriteLocker locker(&readwriteLocker);
    m_currentTeamVec = currentTeamVec;
}

QVector<TowerBattleDataManager::Charactor> TowerBattleDataManager::getCurrentTeamVec() const{
    QReadLocker locker(&readwriteLocker);
    return m_currentTeamVec;
}


QString TowerBattleDataManager::teamEnum2QString(const TowerBattleDataManager::Team& team){
    switch (team) {
    case TowerBattleDataManager::Team::Camellya_Sanhua_Shorekeeper:
        return QString("1椿 2散 3守");

    default:
        return QString("未知配队");

    }
}

QString TowerBattleDataManager::charactorEnum2QString(const TowerBattleDataManager::Charactor& charactor){
    switch (charactor) {
    case TowerBattleDataManager::Charactor::Camellya:
        return QString("Camellya");
    case TowerBattleDataManager::Charactor::Sanhua:
        return QString("Sanhua");
    case TowerBattleDataManager::Charactor::Shorekeeper:
        return QString("Shorekeeper");

    default:
        return QString("未知角色");

    }
}
