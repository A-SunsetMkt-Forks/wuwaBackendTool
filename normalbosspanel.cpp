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
    setting.setBossEnabled(NormalBossEnum::Lorelei, ui->lorelei->isChecked());
    setting.setBossEnabled(NormalBossEnum::FallacyOfNoReturn, ui->fallacyOfNoReturn->isChecked());
    setting.setBossEnabled(NormalBossEnum::FeilianBeringal, ui->feilianBeringal->isChecked());
    setting.setBossEnabled(NormalBossEnum::ImpermanenceHeron, ui->impermanenceHeron->isChecked());
    setting.setBossEnabled(NormalBossEnum::InfernoRider, ui->infernoRider->isChecked());
    setting.setBossEnabled(NormalBossEnum::LampylumenMyriad, ui->lampylumenMyriad->isChecked());
    setting.setBossEnabled(NormalBossEnum::MechAbomination, ui->mechAbomination->isChecked());
    setting.setBossEnabled(NormalBossEnum::MourningAix, ui->mourningAix->isChecked());
    setting.setBossEnabled(NormalBossEnum::TempestMephis, ui->tempestMephis->isChecked());
    setting.setBossEnabled(NormalBossEnum::ThunderingMephis, ui->thunderingMephis->isChecked());
    setting.setBossEnabled(NormalBossEnum::BellBorneGeochelone, ui->bellBorneGeochelone->isChecked());
    setting.setBossEnabled(NormalBossEnum::GloriousLionStatue, ui->gloriousLionStatue->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareCrownless, ui->nightmareCrownless->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareFeilianBeringal, ui->nightmareFeilianBeringal->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareImpermanenceHeron, ui->nightmareImpermanenceHeron->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareInfernoRider, ui->nightmareInfernoRider->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareMourningAix, ui->nightmareMourningAix->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareTempestMephis, ui->nightmareTempestMephis->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareThunderingMephis, ui->nightmareThunderingMephis->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareLampylumenMyriad, ui->nightmareLampylumenMyriad->isChecked());
    setting.setBossEnabled(NormalBossEnum::NightmareKelpi, ui->nightmareKelpi->isChecked());
    return setting;
}


void NormalBossPanel::setNormalBossSetting2UI(const NormalBossSetting& setting) {
    ui->dragonOfDirge->setChecked(setting.isBossEnabled(NormalBossEnum::DragonOfDirge));
    ui->sentryConstruct->setChecked(setting.isBossEnabled(NormalBossEnum::SentryConstruct));
    ui->crownless->setChecked(setting.isBossEnabled(NormalBossEnum::Crownless));
    ui->lorelei->setChecked(setting.isBossEnabled(NormalBossEnum::Lorelei));
    ui->fallacyOfNoReturn->setChecked(setting.isBossEnabled(NormalBossEnum::FallacyOfNoReturn));
    ui->feilianBeringal->setChecked(setting.isBossEnabled(NormalBossEnum::FeilianBeringal));
    ui->impermanenceHeron->setChecked(setting.isBossEnabled(NormalBossEnum::ImpermanenceHeron));
    ui->infernoRider->setChecked(setting.isBossEnabled(NormalBossEnum::InfernoRider));
    ui->lampylumenMyriad->setChecked(setting.isBossEnabled(NormalBossEnum::LampylumenMyriad));
    ui->mechAbomination->setChecked(setting.isBossEnabled(NormalBossEnum::MechAbomination));
    ui->mourningAix->setChecked(setting.isBossEnabled(NormalBossEnum::MourningAix));
    ui->tempestMephis->setChecked(setting.isBossEnabled(NormalBossEnum::TempestMephis));
    ui->thunderingMephis->setChecked(setting.isBossEnabled(NormalBossEnum::ThunderingMephis));
    ui->bellBorneGeochelone->setChecked(setting.isBossEnabled(NormalBossEnum::BellBorneGeochelone));
    ui->nightmareCrownless->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareCrownless));
    ui->nightmareFeilianBeringal->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareFeilianBeringal));
    ui->nightmareImpermanenceHeron->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareImpermanenceHeron));
    ui->nightmareInfernoRider->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareInfernoRider));
    ui->nightmareMourningAix->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareMourningAix));
    ui->nightmareTempestMephis->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareTempestMephis));
    ui->nightmareThunderingMephis->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareThunderingMephis));
    ui->nightmareLampylumenMyriad->setChecked(setting.isBossEnabled(NormalBossEnum::NightmareLampylumenMyriad));
}
