#include "speicalbosswidget.h"
#include "ui_speicalbosswidget.h"

SpeicalBossWidget::SpeicalBossWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpeicalBossWidget)
{
    ui->setupUi(this);
}

SpeicalBossWidget::~SpeicalBossWidget()
{
    delete ui;
}

SpecialBossSetting SpeicalBossWidget::getSetting()
{
    SpecialBossSetting setting;

    // 设置 Boss 类型
    if (ui->JueRadioButton->isChecked()) {
        setting.boss = SpecialBossSetting::SpecialBoss::Jue;
    } else if (ui->RoverRadioButton->isChecked()) {
        setting.boss = SpecialBossSetting::SpecialBoss::Rover;
    } else if (ui->HecateRadioButton->isChecked()){
        setting.boss = SpecialBossSetting::SpecialBoss::Hecate;

    }

    // 设置拾取方式
    QString picpUpEchoMethod = ui->pickUpEchoMethod->currentText();
    if (picpUpEchoMethod.contains("方")) {
        setting.pickupShape = SpecialBossSetting::PickupShape::Square;
    } else if (picpUpEchoMethod.contains("普通")) {
        setting.pickupShape = SpecialBossSetting::PickupShape::Normal;
    } else if (picpUpEchoMethod.contains("直线")) {
        setting.pickupShape = SpecialBossSetting::PickupShape::Line;
    } else if (picpUpEchoMethod.contains("蚊香")) {
        setting.pickupShape = SpecialBossSetting::PickupShape::Spiral;
    } else {
        // 如果未匹配到，设置为默认值
        qWarning() << QString("UI选择 %1 作为声骸拾取方式，未知方式，设置为方形").arg(picpUpEchoMethod);
        setting.pickupShape = SpecialBossSetting::PickupShape::Square;
    }

    // 设置拾取范围
    setting.echoPickupRange = ui->pickUpEchoRange->value();
    setting.spiralPickupRange = ui->spiralPickupRange->value();

    // 是否战斗时按 F
    setting.pressFDuringBattle = ui->isPressFwhenFighting->isChecked();

    // 设置动作额外延迟
    setting.delayAction = ui->actionDalay->value();

    // 跳 A 回血
    setting.jumpRecovery = ui->jumpAttackToHeal->isChecked();

    // BOSS 血条辅助判定
    setting.bossHealthAssist = ui->bossHPAssis->isChecked();

    // 开启不切人战斗
    setting.startWithoutSwitch = ui->isTriggerNoSwitch->isChecked();

    // 启动时机
    setting.startLeftHP = ui->triggerNoSwitchCondition->currentText().toFloat();

    // 吸收文字 模板匹配阈值
    setting.absorbThres = ui->absorbThres->value();

    // 大招检测方式
    switch (ui->ultimateSkillDetectMethod->currentIndex()) {
    case 0:
        setting.ultimateCheckMode = SpecialBossSetting::UltimateCheckMode::Mode1;
        break;
    case 1:
        setting.ultimateCheckMode = SpecialBossSetting::UltimateCheckMode::Mode2;
        break;
    }

    // 自选副本等级（从 ComboBox 获取）
    setting.customDungeonLevel = ui->dungeonLevel->currentText().isEmpty() ? 0 : ui->dungeonLevel->currentText().toInt();

    return setting;
}

