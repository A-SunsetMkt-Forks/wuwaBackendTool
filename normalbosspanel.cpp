#include "normalbosspanel.h"
#include "ui_normalbosspanel.h"

NormalBossPanel::NormalBossPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalBossPanel)
{
    ui->setupUi(this);
}

NormalBossPanel::~NormalBossPanel()
{
    delete ui;
}

NormalBossSetting NormalBossPanel::getNormalBossSettingFromUI() {
    NormalBossSetting setting;
    setting.setBossEnabled(NormalBossEnum::DragonOfDirge, ui->dragonOfDirge->isChecked());
    setting.setBossEnabled(NormalBossEnum::SentryConstruct, ui->sentryConstruct->isChecked());
    setting.setBossEnabled(NormalBossEnum::Crownless, ui->crownless->isChecked());
    return setting;
}


void NormalBossPanel::setNormalBossSetting2UI(const NormalBossSetting& setting) {
    ui->dragonOfDirge->setChecked(setting.isBossEnabled(NormalBossEnum::DragonOfDirge));
    ui->sentryConstruct->setChecked(setting.isBossEnabled(NormalBossEnum::SentryConstruct));
    ui->crownless->setChecked(setting.isBossEnabled(NormalBossEnum::Crownless));
}
