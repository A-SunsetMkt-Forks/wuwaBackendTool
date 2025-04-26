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
    QThread m_imageCapturerThread;
    ImageCapturer m_imageCapturer;


    // 初始化配队介绍信息
    void initTeamInfoMap();
    // 初始化配队转角色信息
    void initTeamCharactorMap();

    // 枚举值转队名
    QString teamEnum2QString(const TowerBattleDataManager::Team& team);


public slots:
    // 用户切换配队
    void on_supportTeam_currentTextChanged(const QString &text);

    // 开始战斗
    void on_startButton_clicked();

    // 停止线程
    void onStop();
};

#endif // TOWEROFADVERSITYWIDGET_H
