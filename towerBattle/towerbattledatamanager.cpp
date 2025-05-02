#include "TowerBattleDataManager.h"


TowerBattleDataManager& TowerBattleDataManager::Instance() {
    // C++11 起局部静态变量初始化线程安全
    static TowerBattleDataManager instance;
    return instance;
}

// 私有构造函数（可在此添加初始化逻辑）
TowerBattleDataManager::TowerBattleDataManager() {
    // 初始化代码
    // 读取美术资源
    QString artResourcesDir = QCoreApplication::applicationDirPath() + "/towerBattle/";

    QString numPad_1_path = artResourcesDir + "numPad_1.bmp";
    numPad_1 = cv::imread(numPad_1_path.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);

    QString numPad_2_path = artResourcesDir + "numPad_2.bmp";
    numPad_2 = cv::imread(numPad_2_path.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);

    QString numPad_3_path = artResourcesDir + "numPad_3.bmp";
    numPad_3 = cv::imread(numPad_3_path.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);

    // 遍历加载美术资源
    QVector<Charactor> supportCharactorList = {Charactor::UNDEFINED, Charactor::Sanhua, Charactor::Camellya};
    for (auto charactor : supportCharactorList) {
           QMap<QString, cv::Mat> thisCharactorMap;
           QString charactorArtResPath = artResourcesDir + charactorEnum2QString(charactor) + "/";

           QDir dir(charactorArtResPath);
           if (!dir.exists()) continue; // 跳过不存在的目录

           // 遍历路径下的所有.bmp文件
           QDirIterator it(charactorArtResPath, {"*.bmp"}, QDir::Files);
           while (it.hasNext()) {
               QString filePath = it.next();
               QFileInfo fileInfo(filePath);

               // 读取图像并检查有效性
               cv::Mat image = cv::imread(filePath.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
               if (!image.empty()) { // 仅存储非空 cv::Mat
                   QString key = fileInfo.baseName(); // 文件名（不含扩展名）作为 Key
                   thisCharactorMap[key] = image;
               }
           }

           charactor2artRes[charactor] = thisCharactorMap; // 允许存储空 QMap（但无空 cv::Mat）
       }

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

double TowerBattleDataManager::getResonanceSkillReady() const {
    QReadLocker locker(&readwriteLocker);
    return resonance_skill_ready;
}

void TowerBattleDataManager::setResonanceSkillReady(double ready) {
    QWriteLocker locker(&readwriteLocker);
    resonance_skill_ready = ready;
}

double TowerBattleDataManager::getResonanceLiberationReady() const {
    QReadLocker locker(&readwriteLocker);
    return resonance_liberation_ready;
}

void TowerBattleDataManager::setResonanceLiberationReady(double ready) {
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


double TowerBattleDataManager::getConcertoEnergy() const{
    QReadLocker locker(&readwriteLocker);
    return concerto_energy;
}

void TowerBattleDataManager::setConcertoEnergy(double val){
    QWriteLocker locker(&readwriteLocker);
    concerto_energy = val;
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

QVector<cv::Mat> TowerBattleDataManager::getNumPads() const {
    QReadLocker locker(&readwriteLocker);
    QVector<cv::Mat> numPadVec = {cv::Mat(), numPad_1, numPad_2, numPad_3};
    return numPadVec;
}

cv::Rect TowerBattleDataManager::getNumPadRoi() const {
    QReadLocker locker(&readwriteLocker);
    return m_numPadArea;
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

QMap<QString, cv::Mat> TowerBattleDataManager::getArtResByName(const Charactor& charactor){
    if(!charactor2artRes.contains(charactor)){
        return QMap<QString, cv::Mat> ();
    }
    else{
        QReadLocker locker(&readwriteLocker);
        return charactor2artRes[charactor];
    }
}
