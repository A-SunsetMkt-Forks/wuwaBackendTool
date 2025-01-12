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

NormalBossSetting NormalBossPanel::getNormalBossSettingFromUI(){
    NormalBossSetting setting;
    setting.crownless = ui->crownless->isChecked();  // 无冠者
    setting.dragonOfDirge = ui->dragonOfDirge->isChecked();  // 叹息古龙
    setting.sentryConstruct = ui->sentryConstruct->isChecked();// 异构武装

    return setting;
}


void NormalBossPanel::setNormalBossSetting2UI(const NormalBossSetting& setting) {
    // 根据 setting 的属性设置 UI 的状态
    ui->crownless->setChecked(setting.crownless); // 无冠者
    ui->dragonOfDirge->setChecked(setting.dragonOfDirge); // 叹息古龙
    ui->sentryConstruct->setChecked(setting.sentryConstruct); // 异构武装
}
