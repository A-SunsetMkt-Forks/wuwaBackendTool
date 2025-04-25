#ifndef TOWEROFADVERSITYWIDGET_H
#define TOWEROFADVERSITYWIDGET_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QDebug>

namespace Ui {
class TowerOfAdversityWidget;
}




class TowerOfAdversityWidget : public QWidget
{
    Q_OBJECT
public:
    enum Team{
        Camellya_Sanhua_Shorekeeper
    };

public:
    explicit TowerOfAdversityWidget(QWidget *parent = nullptr);
    ~TowerOfAdversityWidget();

private:
    Ui::TowerOfAdversityWidget *ui;

    QMap<Team, QString> m_teamInfoMap;

    // 初始化配队介绍信息
    void initTeamInfoMap();
    // 枚举值转队名
    QString teamEnum2QString(const Team& team);
};

#endif // TOWEROFADVERSITYWIDGET_H
