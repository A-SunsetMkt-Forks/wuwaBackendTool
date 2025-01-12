#include "echolockentriespanel.h"
#include "ui_echolockentriespanel.h"

EchoLockEntriesPanel::EchoLockEntriesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EchoLockEntriesPanel)
{
    ui->setupUi(this);


}

EchoLockEntriesPanel::~EchoLockEntriesPanel()
{
    delete ui;
}

LockEchoSetting EchoLockEntriesPanel::getLockEchoSettingFromUI(){
    LockEchoSetting lockEchoSetting;
    QVector<QString> echoSetNameList;
    // 遍历所有页面
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        AbstractEntryPanel* panel = nullptr;
        QWidget* page = ui->tabWidget->widget(i);
        if (page) {
            // 尝试将 page 转换为 AbstractEntryPanel
            panel = qobject_cast<AbstractEntryPanel*>(page);
            if (panel) {
                echoSetNameList.push_back(panel->objectName()); // 或调用其他 AbstractEntryPanel 特定的方法
            } else {
                qWarning() << "Page is not an AbstractEntryPanel.";
                continue;
            }
        }
        QString echoSetName = page->objectName();
        lockEchoSetting.echoSetName2singleSetting[echoSetName] = panel->getSingleEchoSettingFromUI();
    }


    return lockEchoSetting;
}

void EchoLockEntriesPanel::setLockEchoSetting2UI(const LockEchoSetting& setting){
    // 遍历所有页面
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        AbstractEntryPanel* panel = nullptr;
        QWidget* page = ui->tabWidget->widget(i);
        if (page) {
            // 尝试将 page 转换为 AbstractEntryPanel
            panel = qobject_cast<AbstractEntryPanel*>(page);
            if (panel) {
                QString echoSetName = page->objectName();
                if (setting.echoSetName2singleSetting.contains(echoSetName)) {
                    // 将对应的 SingleEchoSetting 数据设置到页面
                    panel->setSingleEchoSetting2UI(setting.echoSetName2singleSetting[echoSetName]);
                }
            } else {
                qWarning() << "Page is not an AbstractEntryPanel.";
            }
        }
    }
}

void EchoLockEntriesPanel::genDefaultSetting(LockEchoSetting &setting){
    for(auto &echoSet: setting.echoSetName2singleSetting.keys()){
        SingleEchoSetting &echoSetSetting = setting.echoSetName2singleSetting[echoSet];
        if(echoSet == "freezingFrost"){
            // 今州冰套
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;
            // 今州冰套 C1
            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;

            // 今州冰套C3
            QVector<QString> c3entries = { "glacioDMG", "energyRegen"};
            echoSetSetting.cost2EntryMap[3] = c3entries;

            // 今州冰套C4
            QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "moltenRift"){
            // 今州火套
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = { "fusionDMG", "energyRegen"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "voidThunder"){
            // 今州雷套
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = { "electroDMG", "energyRegen"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "sierraGale"){
            // 今州风套
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = { "aeroDMG", "energyRegen"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "celestialLight"){
            // 今州光套
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = { "spectroDMG", "energyRegen"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "sunSinkingEclipse"){
            // 今州暗套
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = { "havocDMG", "energyRegen"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "rejuvenatingGlow"){
            // 今州治疗套
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio", "HPratio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = { "heal", "energyRegen", "HPratio"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio", "heal"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "moonlitClouds"){
            // 今州共鸣
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = {"energyRegen", "electroDMG", "glacioDMG"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }
        else if(echoSet == "lingeringTunes"){
            // 今州攻击
            echoSetSetting.islevel5 = true;
            echoSetSetting.isLockJudge = true;
            echoSetSetting.isDiscardedJudge = true;
            echoSetSetting.isNormalJudge = true;

            QVector<QString> c1entries = { "attackRatio"};
            echoSetSetting.cost2EntryMap[1] = c1entries;


            QVector<QString> c3entries = {"energyRegen", "attackRaio"};
            echoSetSetting.cost2EntryMap[3] = c3entries;


            QVector<QString> c4entries = { "criticalDMG", "criticalRatio", "attackRaio"};
            echoSetSetting.cost2EntryMap[4] = c4entries;
        }

    }
}
