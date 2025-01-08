#include "generalpanel.h"
#include "ui_generalpanel.h"

GeneralPanel::GeneralPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralPanel)
{
    ui->setupUi(this);

}

GeneralPanel::~GeneralPanel()
{
    delete ui;
}

RebootGameSetting GeneralPanel::getRebootGameSetting() const {
    RebootGameSetting setting;

    // Retrieve the values from the UI
    if (ui->rebootCycles) {
        setting.rebootCycles = static_cast<unsigned int>(ui->rebootCycles->value());
    }

    if (ui->rebootWaitS) {
        setting.rebootWaitS = static_cast<unsigned int>(ui->rebootWaitS->value());
    }

    if (ui->startBtnX && ui->startBtnY) {
        setting.startGameBtnPos = QPoint(ui->startBtnX->value(), ui->startBtnY->value());
    }

    return setting;
}

