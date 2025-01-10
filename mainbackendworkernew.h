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

    bool islevel2 = false;     // 2星默认不判断
    bool islevel3 = false;     // 3星默认不判断
    bool islevel4 = false;     // 4星默认不判断
    bool islevel5 = false;     // 5星默认不判断

    QMap<int, QVector<QString>> cost2EntryMap;  // 通过cost 1 3 4可以索引得到想要的词条

    QString toQString() const {
        QString result;
        result += "isDiscardedJudge\t" + QString::number(isDiscardedJudge) + "\n";
        result += "isLockJudge\t" + QString::number(isLockJudge) + "\n";
        result += "isNormalJudge\t" + QString::number(isNormalJudge) + "\n";

        result += "islevel2\t" + QString::number(islevel2) + "\n";
        result += "islevel3\t" + QString::number(islevel3) + "\n";
        result += "islevel4\t" + QString::number(islevel4) + "\n";
        result += "islevel5\t" + QString::number(islevel5) + "\n";

        result += "cost2EntryMap\t\n";
        for (auto it = cost2EntryMap.constBegin(); it != cost2EntryMap.constEnd(); ++it) {
            result += QString("\tcost%1\t\n").arg(it.key());
            for (const QString& entry : it.value()) {
                result += QString("\t\t%1\n").arg(entry);
            }
        }

        return result;
    }
};
Q_DECLARE_METATYPE(SingleEchoSetting)

// 总设置
struct LockEchoSetting{
    QMap<QString, SingleEchoSetting> echoSetName2singleSetting; // 通过声骸套装名可以索引得到设置

    QString toQString() const {
        QString result;

        for (auto it = echoSetName2singleSetting.constBegin(); it != echoSetName2singleSetting.constEnd(); ++it) {
            result += QString("echoSetName\t%1\n").arg(it.key());
            result += it.value().toQString();
        }

        return result;
    }

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

    const cv::Rect isLockROI = {1121, 206, 27, 30};   //判断是否上锁的位置
    const cv::Rect isDiscardROI = {1191, 207, 27, 29};  //  判断是否丢弃了的位置
    const cv::Rect costROI = {1191, 138, 26, 21};   // COST 后面数字的位置
    const cv::Rect mainEntryROI = {870, 278, 230, 30}; // 主词条位置


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
