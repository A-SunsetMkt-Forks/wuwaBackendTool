#ifndef SPEICALBOSSWIDGET_H
#define SPEICALBOSSWIDGET_H

#include <QWidget>
#include "settingparams.h"

namespace Ui {
class SpeicalBossWidget;
}

class SpeicalBossWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpeicalBossWidget(QWidget *parent = nullptr);
    ~SpeicalBossWidget();

    SpecialBossSetting getSetting();

private:
    Ui::SpeicalBossWidget *ui;

signals:
    void startSpecialBoss(const SpecialBossSetting& setting);

public slots:
    void on_activateBtn_clicked();

};

#endif // SPEICALBOSSWIDGET_H
