#ifndef TOWERBATTLEDATAMANAGER_H
#define TOWERBATTLEDATAMANAGER_H

#include <QString>
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include <QApplication>
#include <opencv2/opencv.hpp>




class TowerBattleDataManager
{
public:
    enum Team{
        Camellya_Sanhua_Shorekeeper
    };

    enum Charactor{
        UNDEFINED,
        Camellya,
        Sanhua,
        Shorekeeper
    };


    // 获取单例实例（返回引用）
    static TowerBattleDataManager& Instance();


    // 删除拷贝构造函数和赋值运算符
    TowerBattleDataManager(const TowerBattleDataManager&) = delete;
    TowerBattleDataManager& operator=(const TowerBattleDataManager&) = delete;

    //======= 线程安全的读写接口 =======//
    // OpenCV图像数据
    const cv::Mat& getLastCapImg() const;
    void setLastCapImg(const cv::Mat& newImg);

    // 图像采集参数
    int getCapFrameRate() const;
    void setCapFrameRate(int rate);
    int getTickRate() const;
    void setTickRate(int rate);

    // 角色状态
    int getCurrentIndex() const;
    void setCurrentIndex(int idx);
    bool isResonanceSkillReady() const;
    void setResonanceSkillReady(bool ready);
    bool isResonanceLiberationReady() const;
    void setResonanceLiberationReady(bool ready);
    double getResonanceCircuit() const;
    void setResonanceCircuit(double value);
    bool isEchoSkillReady() const;
    void setEchoSkillReady(bool ready);
    bool isExplorerToolReady() const;
    void setExplorerToolReady(bool ready);

    // 写入配队
    void setCurrentTeamVec(const QVector<TowerBattleDataManager::Charactor> &currentTeamVec);
    QVector<TowerBattleDataManager::Charactor> getCurrentTeamVec() const;

    // 获取不可更改的美术资源
    // [0]是空的 [1 2 3]才是numPad截图
    QVector<cv::Mat> getNumPads() const;
    cv::Rect getNumPadRoi() const ;


    // 枚举值转字符串
    QString teamEnum2QString(const TowerBattleDataManager::Team& team);
    QString charactorEnum2QString(const TowerBattleDataManager::Charactor& charactor);

private:
    // 构造函数和析构函数私有化
    TowerBattleDataManager();
    ~TowerBattleDataManager();

    mutable QReadWriteLock readwriteLocker; // 读写锁（mutable 允许 const 函数中使用）

    // 最后一次获取的图像
    cv::Mat m_lastCapImg;
    int m_cap_frame_rate = 60;  // 捕获图像帧率 默认30
    int m_tick_rate = 20;  // 每秒做多少次判断

    int current_idx = 1;  // 默认1号位
    bool resonance_skill_ready = true;
    bool resonance_liberation_ready = true;
    double resonance_circuit = 0.0;  // 0.0表示空回路 1.0表示满回路
    bool echo_skill_ready = true;
    bool explorer_tool_ready = true;

    QVector<TowerBattleDataManager::Charactor> m_currentTeamVec;  // 四个元素 因为0 什么都不是，1 、2、3才是有效编号

    // 初始化时读取的美术资源
    cv::Mat numPad_1;
    cv::Mat numPad_2;
    cv::Mat numPad_3;
    // 1280 720时对应的numPad区域
    cv::Rect m_numPadArea = cv::Rect(1128, 87, 102, 313);


};

#endif // TOWERBATTLEDATAMANAGER_H
