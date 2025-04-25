#ifndef TOWERBATTLEDATAMANAGER_H
#define TOWERBATTLEDATAMANAGER_H

#include <QString>
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include <opencv2/opencv.hpp>


class TowerBattleDataManager
{
public:
    // 获取单例实例（返回引用）
    static TowerBattleDataManager& Instance();

    QVector<double> readData() const ;
    void writeData(const QVector<double> &data);

    // 删除拷贝构造函数和赋值运算符
    TowerBattleDataManager(const TowerBattleDataManager&) = delete;
    TowerBattleDataManager& operator=(const TowerBattleDataManager&) = delete;


private:
    // 构造函数和析构函数私有化
    TowerBattleDataManager();
    ~TowerBattleDataManager();

    mutable QReadWriteLock readwriteLocker; // 读写锁（mutable 允许 const 函数中使用）
    QVector<double> testData;

    int
};

#endif // TOWERBATTLEDATAMANAGER_H
