#ifndef MAINBACKENDWORKERNEW_H
#define MAINBACKENDWORKERNEW_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include "utils.h"

struct LockEchoSetting{

};
// 声明为 Qt 元类型
Q_DECLARE_METATYPE(LockEchoSetting)

class MainBackendWorkerNew : public QObject
{
    Q_OBJECT
public:
    explicit MainBackendWorkerNew(QObject *parent = nullptr);
    bool isBusy();
    void stopWorker();

private:
    QAtomicInt m_isRunning;   //原子int 防止多线程冲突

    // 尝试进入背包 如果找不到则3次ESC + 找背包   返回true表示被用户打断或找到了背包并点了进去。返回false表示没有找到背包
    bool enterBagInterface();

    // 尝试进入声骸面板
    bool enterEchoInterface();

    // 处理一个页面的声骸
    bool lockOnePageEcho();

signals:
    void lockEchoDone(const bool& isNormalEnd, const QString& errMsg, const LockEchoSetting &lockEchoSetting);

public slots:
    // 响应UI要求 开始自动锁定声骸
    void onStartLockEcho(const LockEchoSetting &lockEchoSetting);

private:
    // 常量坐标或ROI区
    const cv::Rect searchBagROI = {178, 2, 85, 69};   // 在整个游戏画面找背包的区域
    const cv::Rect searchEchoIconROI = {1, 62, 102, 640};  //  在背包画面左侧区域找声骸图标
    const cv::Rect topLeftEchoROI = {136, 79, 102, 125};   // 进入声骸页面后 最左上角默认声骸的位置
    const int echoColMargin = 111;   // 向右移动 需要向右挪动
    const int echoRowMargin = 137;   // 向下移动 需要向下挪动
    const int maxColNum = 6;    // 一个页面默认能显示6列4行
    const int maxRowNum = 4;

    const cv::Rect echoSetRoi = {2, 101, 24, 24};   // 比如左上角的套装图标在全图就是 echoSetRoi.x + topLeftEchoROI.x echoSetRoi.y + topLeftEchoROI.y echoSetRoi.width echoSetRoi.height

    // 声骸套装名字
    const QString freezingFrost = "freezingFrost";  // 凝夜白霜  今州冰套
    const QString moltenRift = "moltenRift";        // 熔山裂谷  今州火套
    const QString voidThunder = "voidThunder";      // 彻空冥雷  今州雷套
    const QString sierraGale = "sierraGale";        // 啸谷长风  今州风套
    const QString celestialLight = "celestialLight";   // 浮星祛暗  今州光套
    const QString sunSinkingEclipse = "sunSinkingEclipse"; // 沉日劫明  今州暗套
    const QString rejuvenatingGlow = "rejuvenatingGlow";  // 隐世回光 今州奶套
    const QString moonlitClouds = "moonlitClouds";        // 轻云出月 今州共鸣
    const QString lingeringTunes = "lingeringTunes";      // 不绝余音 今州攻击

    const QString frostyResolve = "frostyResolve";        // 凌冽决断之心 黎纳汐塔冰套
    const QString eternalRadiance = "eternalRadiance";    // 此间永驻之光 黎纳汐塔光套
    const QString midnightVeil = "midnightVeil";    // 幽夜隐匿之帷 黎纳汐塔暗套
    const QString empyreanAnthem = "empyreanAnthem";    // 高天共奏之曲 黎纳汐塔共鸣协同攻击套
    const QString tidebreakingCourage = "tidebreakingCourage";  // 无惧浪涛之勇 黎纳汐塔共鸣效率增伤套

};

#endif // MAINBACKENDWORKERNEW_H
