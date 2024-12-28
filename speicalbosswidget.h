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

    SpecialBoosSetting getSetting();

private:
    Ui::SpeicalBossWidget *ui;

public slots:
    void on_activateBtn_clicked();

};

#endif // SPEICALBOSSWIDGET_H
