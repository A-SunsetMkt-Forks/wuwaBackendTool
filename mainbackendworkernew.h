#ifndef MAINBACKENDWORKERNEW_H
#define MAINBACKENDWORKERNEW_H

#include <QObject>
#include <QAtomicInt>
#include <QDebug>
#include <QElapsedTimer>
#include "utils.h"

// 某个声骸套装的设置
struct SingleEchoSetting{
    bool isDiscardedJudge = true;  // true表示丢弃的 参与判断
    bool isLockJudge = true;      // true表示锁定的 参与判断
    bool isNormalJudge = true;    // true表示普通的 参与判断
};
Q_DECLARE_METATYPE(SingleEchoSetting)

// 总设置
struct LockEchoSetting{
    QMap<QString, SingleEchoSetting> echoSetName2singleSetting; // 通过声骸套装名可以索引得到设置

};
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

    // 启动时 初始化套装名字和图片列表
    void initEchoSetName2IconMap();

    // 尝试进入背包 如果找不到则3次ESC + 找背包   返回true表示被用户打断或找到了背包并点了进去。返回false表示没有找到背包
    bool enterBagInterface();

    // 尝试进入声骸面板
    bool enterEchoInterface();

    // 处理一个页面的声骸
    bool lockOnePageEcho();

    // 慢速拖动 越慢越容易成功
    bool dragWindowClient3(HWND hwnd, int startx, int starty, int endx, int endy, int steps, int stepPauseMs);

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
    const int echoRowMargin = 137;   // 向下移动 需要向下挪动  20250110 1505 是137  137是静态更准的，但是移动拖拽鼠标似乎需要+2
    const int maxColNum = 6;    // 一个页面默认能显示6列4行
    const int maxRowNum = 4;

    // 比如左上角的套装图标在全图就是 echoSetRoi.x + topLeftEchoROI.x echoSetRoi.y + topLeftEchoROI.y echoSetRoi.width echoSetRoi.height
    // 20250110 1509 {2, 99, 26, 26}
    const cv::Rect echoSetRoi = {0, 97, 30, 30};  // ### 可考虑适度再放大 增加容错

    // 声骸套装名字
    QVector<QString> echoSetNameVector;
    QMap<QString, cv::Mat> echoSet2echoSetIconMap;      // 输入英文套装名 得到对应套装icon
    QMap<QString, QString> echoSetNameTranslationMap;   // 输入英文套装名 得到对应中文名
};

#endif // MAINBACKENDWORKERNEW_H
