#ifndef NORMALBOSSPANEL_H
#define NORMALBOSSPANEL_H

#include <QWidget>
#include "settingparams.h"

namespace Ui {
class NormalBossPanel;
}

class NormalBossPanel : public QWidget
{
    Q_OBJECT

public:
    explicit NormalBossPanel(QWidget *parent = nullptr);
    ~NormalBossPanel();

    NormalBossSetting getNormalBossSettingFromUI();
    void setNormalBossSetting2UI(const NormalBossSetting& setting);

private:
    Ui::NormalBossPanel *ui;
};

#endif // NORMALBOSSPANEL_H
