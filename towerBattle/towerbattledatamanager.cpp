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


void TowerBattleDataManager::writeData(const QVector<double> &data) {
    QWriteLocker locker(&readwriteLocker);  // 自动加写锁
    testData = data;  // 写操作
}

// 读取数据（读锁保护）
QVector<double> TowerBattleDataManager::readData() const {
    QReadLocker locker(&readwriteLocker);  // 自动加读锁
    return testData;  // 读操作

}
