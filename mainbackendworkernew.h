#ifndef MAINBACKENDWORKERNEW_H
#define MAINBACKENDWORKERNEW_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include "utils.h"
#include "settingparams.h"
#include "fightbackendworkernew.h"



class MainBackendWorkerNew : public QObject
{
    Q_OBJECT
public:
    explicit MainBackendWorkerNew(QObject *parent = nullptr);
    ~MainBackendWorkerNew();
    bool isBusy();
    void stopWorker();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突

    // 启动时 初始化套装名字和图片列表
    void initEchoSetName2IconMap();

    // 启动时 初始化词条名字和图片列表
    void initEntryName2IconMap();

    // 尝试进入背包 如果找不到则3次ESC + 找背包   返回true表示被用户打断或找到了背包并点了进去。返回false表示没有找到背包
    bool enterBagInterface();

    // 尝试进入声骸面板 必须已经进入了背包面板 才能进入声骸面板
    bool enterEchoInterface();

    // 处理一个页面的声骸
    bool lockOnePageEcho(const LockEchoSetting& lockEchoSetting);

    // 慢速拖动 越慢越容易成功
    bool dragWindowClient3(HWND hwnd, int startx, int starty, int endx, int endy, int steps, int stepPauseMs);

    // 工具函数，不断轮询 设置最大等待时间和轮询间隔，必须以指定匹配度找到模板。如果没找到则存储错误信息供反馈
    bool loopFindPic(const cv::Mat& templateImg, const double& requireSimilarity, \
                     const int &maxWaitMs, const int &refreshMs, const QString& ifFailedMsg, double& similarity, \
                     int& x, int& y, int& timeCostMs);

    // 跳过月卡 可能需要频繁调用 在各个节点和找不到血条 + 背包时调用
    void skipMonthCard();

    // 复苏
    bool revive();


    // 轮刷boss的总准备工作 完成后应该是在残像探寻界面
    bool normalBossPreperation(const NormalBossSetting &normalBossSetting, QString& errMsg);

    // 尝试恢复到默认界面 便于下一步找索拉指南或其他目标物
    bool backToMain();

    // 进入索拉指南 必须首先已经在主界面
    bool enterSolaGuide();

    // 进入残像探寻 必须已经在索拉指南
    bool enterEchoList();

    // 刷某boss 起点是残像探寻 终点是拾取后  // 未来补齐： 月卡 重启游戏 复活   // 最多允许进行300s
    bool oneBossLoop(const NormalBossSetting &normalBossSetting, const NormalBossEnum& bossName, QString& errMsg);

    // 通用的准备工作 从残像探寻界面 到传送到boss面前
    bool echoList2bossPositionPreparation(const NormalBossSetting &normalBossSetting, \
                                                                const QString& bossEnName, \
                                                                const QString& bossChName, QString& errMsg);
    // 刷dragonOfDirge 叹息之龙 准备工作 从传送到boss面前 到锁定boss
    bool dragonOfDirgePreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 刷crownLess 无冠者 准备工作 从传送到boss面前 到锁定boss
    bool crownLessPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 刷异构武装 异构武装 准备工作 从传送到boss面前 到锁定boss
    bool sentryConstructPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 刷无归的谬误 准备工作 从传送到boss面前  到锁定boss
    bool fallacyOfNoReturnPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 无常凶鹭
    bool impermanenceHeronPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 燎照之骑
    bool infernoRiderPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 辉萤军势
    bool lampylumenMyriadPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 哀声鸷
    bool mourningAixPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 云闪之麟
    bool tempestMephisPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);
    // 朔雷之麟
    bool thunderingMephisPreparation(const NormalBossSetting &normalBossSetting, QString& errMsg);

signals:
    // 锁定声骸完成
    void lockEchoDone(const bool& isNormalEnd, const QString& errMsg, const LockEchoSetting &lockEchoSetting);

    // 轮刷boss完成
    void normalBossDone(const bool& isNormalEnd, const QString& errMsg, const NormalBossSetting &normalBossSetting);

    // 要求战斗线程开始工作
    void startFight();

public slots:
    // 响应UI要求 开始自动锁定声骸
    void onStartLockEcho(const LockEchoSetting &lockEchoSetting);

    // 响应UI要求 开始自动轮刷普通boss
    void onStartNormalBoss(const NormalBossSetting &normalBossSetting);



private:
    // 常量坐标或ROI区
    const cv::Rect searchBagROI = {178, 2, 85, 69};   // 在整个游戏画面找背包的区域
    const cv::Rect searchEchoIconROI = {1, 62, 102, 640};  //  在背包画面左侧区域找声骸图标
    const cv::Rect topLeftEchoROI = {136, 79, 102, 125};   // 进入声骸页面后 最左上角默认声骸的位置
    const int echoColMargin = 111;   // 向右移动 需要向右挪动
    const int echoRowMargin = 137;   // 向下移动 需要向下挪动  20250110 1505 是137  137是静态更准的，但是移动拖拽鼠标似乎需要+2
    const int maxColNum = 6;    // 一个页面默认能显示6列4行
    const int maxRowNum = 4;

   // const cv::Rect isLockROI = {1121, 206, 27, 30};   //判断是否上锁的位置
    //const cv::Rect isDiscardROI = {1191, 207, 27, 29};  //  判断是否丢弃了的位置
    const cv::Rect isLockROI = {858, 77, 377, 278};   //判断是否上锁的位置
    const cv::Rect isDiscardROI = {858, 77, 377, 278};  //  判断是否丢弃了的位置
    const cv::Rect costROI = {1191, 138, 26, 23};   // COST 后面数字的位置
    const cv::Rect mainEntryROI = {870, 278, 230, 32}; // 主词条位置


    // 比如左上角的套装图标在全图就是 echoSetRoi.x + topLeftEchoROI.x echoSetRoi.y + topLeftEchoROI.y echoSetRoi.width echoSetRoi.height
    // 20250110 1509 {2, 99, 26, 26}
    const cv::Rect echoSetRoi = {0, 97, 30, 30};  // ### 可考虑适度再放大 增加容错

    // 声骸套装名字
    QVector<QString> echoSetNameVector;   // 支持的套装列表
    QMap<QString, cv::Mat> echoSet2echoSetIconMap;      // 输入英文套装名 得到对应套装icon
    QMap<QString, QString> echoSetNameTranslationMap;   // 输入英文套装名 得到对应中文名

    QVector<QString> entryNameVector;  // 支持的词条列表
    QMap<QString, cv::Mat> entryName2entryIconMap;      // 输入英文套装名 得到对应词条icon
    QMap<QString, QString> entryNameTranslationMap;   // 输入英文套装名 得到对应中文名


    // 轮刷boss相关
    unsigned int pickUpNormalBossEcho = 0;
    const int defaultRefreshMs = 250;  // 默认轮询截图间隔
    const int defaultMaxWaitMs = 2500;   // 轮询最大等待时间
    const cv::Point scrollEchoListPos = {534, 117};  // echo列表向下滚动滚轮的鼠标位置
    const int maxFightMs = 600000;

    // 拖拽 可以稍快 无需太精确 寻找残像 翻页 530 549 -> 530 189
    const cv::Point scrollEchoListsStartPos = {530, 549};
    const cv::Point scrollEchoListsEndPos = {530, 189};
    const cv::Point scrollEchoListsWaitPos = {51, 386};  // 防止挡住或改变图像

    // 残像记录按钮位置 找到一次以后即固定 点击这里
    cv::Point switchEchoListPos = cv::Point(-1, -1);  //负数表示不可用

    // 战斗线程
    QThread m_fightThread;
    FightBackendWorkerNew m_fightBackendWorkerNew;

    // 脚本线程启动 强制执行一次月卡判断
    bool isFirstSkipMonthCard = false;


};

#endif // MAINBACKENDWORKERNEW_H
