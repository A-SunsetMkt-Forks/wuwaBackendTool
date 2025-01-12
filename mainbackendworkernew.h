#ifndef MAINBACKENDWORKERNEW_H
#define MAINBACKENDWORKERNEW_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include <QElapsedTimer>
#include "utils.h"
#include "settingparams.h"



class MainBackendWorkerNew : public QObject
{
    Q_OBJECT
public:
    explicit MainBackendWorkerNew(QObject *parent = nullptr);
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

    // 尝试进入声骸面板
    bool enterEchoInterface();

    // 处理一个页面的声骸
    bool lockOnePageEcho(const LockEchoSetting& lockEchoSetting);

    // 慢速拖动 越慢越容易成功
    bool dragWindowClient3(HWND hwnd, int startx, int starty, int endx, int endy, int steps, int stepPauseMs);

    // 工具函数，不断轮询 设置最大等待时间和轮询间隔，必须以指定匹配度找到模板。如果没找到则存储错误信息供反馈
    bool loopFindPic(const cv::Mat& capImg, const cv::Mat& templateImg, const double& requireSimilarity, \
                     const int &maxWaitMs, const int &refreshMs, const QString& ifFailedMsg, double& similarity, \
                     int& x, int& y, int& timeCostMs);

signals:
    // 锁定声骸完成
    void lockEchoDone(const bool& isNormalEnd, const QString& errMsg, const LockEchoSetting &lockEchoSetting);

    // 轮刷boss完成
    void normalBossDone(const bool& isNormalEnd, const QString& errMsg, const NormalBossSetting &normalBossSetting);

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
};

#endif // MAINBACKENDWORKERNEW_H
