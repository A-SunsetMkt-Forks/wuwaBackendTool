#ifndef GENERALPANEL_H
#define GENERALPANEL_H

#include <QWidget>
#include <QFileDialog>
#include <QElapsedTimer>

#include "settingparams.h"
#include "utils.h"

namespace Ui {
class GeneralPanel;
}

class GeneralPanel : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralPanel(QWidget *parent = nullptr);
    ~GeneralPanel();

    RebootGameSetting getRebootGameSetting() const;

private:
    Ui::GeneralPanel *ui;

private slots:
    void on_testFindPic_clicked();
};

#endif // GENERALPANEL_H
