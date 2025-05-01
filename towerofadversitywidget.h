#ifndef TOWEROFADVERSITYWIDGET_H
#define TOWEROFADVERSITYWIDGET_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QThread>

// 数据中转站
#include "towerBattle/towerbattledatamanager.h"


// 每秒30帧采图工具
#include "towerBattle/imagecapturer.h"
#include "towerBattle/imagecapturermonitor.h"

// 判断当前角色 1 2 3的工具
#include "towerBattle/teamidxrecognitor.h"
#include "towerBattle/teamidxrecognitionmonitor.h"

// 判断角色回路状态
#include "towerBattle/resonancecircuitjudger.h";
#include "towerBattle/resonancecircuitjudgemonitor.h"

// 判断角色大招状态
#include "towerBattle/ultimatejudger.h";
#include "towerBattle/ultimatejudgemonitor.h"

// 判断角色共鸣技能状态
#include "towerBattle/resonanceskilljudger.h";
#include "towerBattle/resonanceskilljudgemonitor.h"

// 判断角色协奏能量
#include "towerBattle/concertoenergyjudger.h";
#include "towerBattle/concertoenergyjudgemonitor.h"


namespace Ui {
class TowerOfAdversityWidget;
}


class TowerOfAdversityWidget : public QWidget
{
    Q_OBJECT
public:

public:
    explicit TowerOfAdversityWidget(QWidget *parent = nullptr);
    ~TowerOfAdversityWidget();

private:
    Ui::TowerOfAdversityWidget *ui;

    QMap<TowerBattleDataManager::Team, QString> m_teamInfoMap;
    QMap<TowerBattleDataManager::Team, QVector<TowerBattleDataManager::Charactor>> m_teamCharactorMap;

    // 后台线程管理
    // 采图管理器
    QThread m_imageCapturerThread;
    ImageCapturer m_imageCapturer;
    QThread m_imageCapturerMonitorThread;
    ImageCapturerMonitor m_imageCapturerMonitor;

    // 判断角色编号1 2 3
    QThread m_teamIdxRecognitorThread;
    TeamIdxRecognitor m_teamIdxRecognitor;
    QThread m_teamIdxRecognitonMonitorThread;
    TeamIdxRecognitionMonitor m_teamIdxRecognitionMonitor;

    // 判断角色回路
    QThread m_resonanceCircuitJudgerThread;
    ResonanceCircuitJudger m_resonanceCircuitJudger;
    QThread m_resonanceCircuitJudgeMonitorThread;
    ResonanceCircuitJudgeMonitor m_resonanceCircuitJudgeMonitor;

    // 判断角色 共鸣解放 也就是大招
    QThread m_ultimateJudgerThread;
    UltimateJudger m_ultimateJudger;
    QThread m_ultimateJudgeMonitorThread;
    UltimateJudgeMonitor m_ultimateJudgeMonitor;


    // 判断角色 共鸣技能
    QThread m_resonanceSkillJudgerThread;
    ResonanceSkillJudger m_resonanceSkillJudger;
    QThread m_resonanceSkillJudgeMonitorThread;
    ResonanceSkillJudgeMonitor m_resonanceSkillJudgeMonitor;

    // 判断角色 协奏能量
    QThread m_concertoEnergyJudgerThread;
    ConcertoEnergyJudger m_concertoEnergyJudger;
    QThread m_concertoEnergyJudgeMonitorThread;
    ConcertoEnergyJudgeMonitor m_concertoEnergyJudgeMonitor;




    // 初始化配队介绍信息
    void initTeamInfoMap();
    // 初始化配队转角色信息
    void initTeamCharactorMap();



public slots:
    // 用户切换配队
    void on_supportTeam_currentTextChanged(const QString &text);

    // 开始战斗
    void on_startButton_clicked();

    // 停止线程
    void onStop();

    // 切换游戏图像监视器的状态 或刷新图像
    void on_updateGameMonitorStatus(const bool& isBusy, const cv::Mat& mat);

    // 更新当前选中1 2 3号
    void on_updateCurrentTeamIdx(const int& idx);

    // 更新回路状态
    void on_updateResonanceCircuit(const double& value);

    // 更新大招状态
    void on_updateResonanceLiberationReady(const double& isReady);

    // 更新共鸣技能状态
    void on_updateResonanceSkillStatus(const double& val);

    // 更新协奏能量
    void on_updateConcertoEnergy(const double &val);

signals:
    // 发送信号 要求采图线程 和监控采图线程工作
    void start_capturer();
    void start_capturerMonitor(const ImageCapturer* capturer);

    // 要求检测 配队 1 2 3 编号
    void start_teamIdxRecognitor();
    void start_teamIdxRecognitonMonitor(const TeamIdxRecognitor* capturer);

    // 要求检测共鸣回路
    void start_resonance_recognition();
    void start_resonance_recognition_minitor(const ResonanceCircuitJudger* capturer);

    // 要求检测大招 共鸣解放
    void start_ultimate_judge();
    void start_ultimate_judge_monitor(const UltimateJudger* capturer);

    // 要求检测 共鸣技能
    void start_resonance_skill_recognition();
    void start_resonance_skill_recognition_minitor(const ResonanceSkillJudger* capturer);

    // 要求检测 协奏能量
    void start_concerto_energy_recognition();
    void start_concerto_energy_recognition_monitor(const ConcertoEnergyJudger* capturer);

};

#endif // TOWEROFADVERSITYWIDGET_H
