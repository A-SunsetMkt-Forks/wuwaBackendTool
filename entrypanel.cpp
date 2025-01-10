#include "entrypanel.h"
#include "ui_entrypanel.h"

EntryPanel::EntryPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryPanel)
{
    ui->setupUi(this);

}

EntryPanel::~EntryPanel()
{
    delete ui;
}

QVector<QString> EntryPanel::getEntryMapfromUI(){
    QVector<QString> selectEntryList;

    // 判断每个 QCheckBox 是否被选中
    if (ui->HP->isChecked()) {
        selectEntryList.push_back("HP");
    }

    if (ui->HPratio->isChecked()) {
        selectEntryList.push_back("HPRatio");
    }

    if (ui->aeroDMG->isChecked()) {
        selectEntryList.push_back("aeroDMG");
    }

    if (ui->attack->isChecked()) {
        selectEntryList.push_back("attack");
    }

    if (ui->attackRatio->isChecked()) {
        selectEntryList.push_back("attackRatio");
    }

    if (ui->criticalDMG->isChecked()) {
        selectEntryList.push_back("criticalDMG");
    }

    if (ui->criticalRatio->isChecked()) {
        selectEntryList.push_back("criticalRatio");
    }

    if (ui->defence->isChecked()) {
        selectEntryList.push_back("defence");
    }

    if (ui->defenceRatio->isChecked()) {
        selectEntryList.push_back("defenceRatio");
    }

    if (ui->electroDMG->isChecked()) {
        selectEntryList.push_back("electroDMG");
    }

    if (ui->energyRegen->isChecked()) {
        selectEntryList.push_back("energyRegen");
    }

    if (ui->fusionDMG->isChecked()) {
        selectEntryList.push_back("fusionDMG");
    }

    if (ui->glacioDMG->isChecked()) {
        selectEntryList.push_back("glacioDMG");
    }

    if (ui->havocDMG->isChecked()) {
        selectEntryList.push_back("havocDMG");
    }

    if (ui->heal->isChecked()) {
        selectEntryList.push_back("heal");
    }

    if (ui->spectroDMG->isChecked()) {
        selectEntryList.push_back("spectroDMG");
    }

    return selectEntryList;
}


void EntryPanel::setEntryMap2UI(const QVector<QString>& entries) {
    // 重置所有 QCheckBox
    ui->HP->setChecked(false);
    ui->HPratio->setChecked(false);
    ui->aeroDMG->setChecked(false);
    ui->attack->setChecked(false);
    ui->attackRatio->setChecked(false);
    ui->criticalDMG->setChecked(false);
    ui->criticalRatio->setChecked(false);
    ui->defence->setChecked(false);
    ui->defenceRatio->setChecked(false);
    ui->electroDMG->setChecked(false);
    ui->energyRegen->setChecked(false);
    ui->fusionDMG->setChecked(false);
    ui->glacioDMG->setChecked(false);
    ui->havocDMG->setChecked(false);
    ui->heal->setChecked(false);
    ui->spectroDMG->setChecked(false);

    // 根据 entries 启用对应的 QCheckBox
    for (const QString& entry : entries) {
        if (entry == "HP") {
            ui->HP->setChecked(true);
        } else if (entry == "HPRatio") {
            ui->HPratio->setChecked(true);
        } else if (entry == "aeroDMG") {
            ui->aeroDMG->setChecked(true);
        } else if (entry == "attack") {
            ui->attack->setChecked(true);
        } else if (entry == "attackRatio") {
            ui->attackRatio->setChecked(true);
        } else if (entry == "criticalDMG") {
            ui->criticalDMG->setChecked(true);
        } else if (entry == "criticalRatio") {
            ui->criticalRatio->setChecked(true);
        } else if (entry == "defence") {
            ui->defence->setChecked(true);
        } else if (entry == "defenceRatio") {
            ui->defenceRatio->setChecked(true);
        } else if (entry == "electroDMG") {
            ui->electroDMG->setChecked(true);
        } else if (entry == "energyRegen") {
            ui->energyRegen->setChecked(true);
        } else if (entry == "fusionDMG") {
            ui->fusionDMG->setChecked(true);
        } else if (entry == "glacioDMG") {
            ui->glacioDMG->setChecked(true);
        } else if (entry == "havocDMG") {
            ui->havocDMG->setChecked(true);
        } else if (entry == "heal") {
            ui->heal->setChecked(true);
        } else if (entry == "spectroDMG") {
            ui->spectroDMG->setChecked(true);
        }
    }
}

