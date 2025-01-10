#include "abstractentrypanel.h"
#include "ui_abstractentrypanel.h"

AbstractEntryPanel::AbstractEntryPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbstractEntryPanel)
{
    ui->setupUi(this);
}

AbstractEntryPanel::~AbstractEntryPanel()
{
    delete ui;
}

SingleEchoSetting AbstractEntryPanel::getSingleEchoSettingFromUI(){
    SingleEchoSetting setting;
    setting.isDiscardedJudge = ui->isDiscardJudge->isChecked();
    setting.isLockJudge = ui->isLockJudge->isChecked();
    setting.isNormalJudge = ui->isNormalJudge->isChecked();

    setting.islevel2 = ui->level2->isChecked();
    setting.islevel3 = ui->level3->isChecked();
    setting.islevel4 = ui->level4->isChecked();
    setting.islevel5 = ui->level5->isChecked();

    // cost1
    setting.cost2EntryMap[1] = ui->cost1->getEntryMapfromUI();
    // cost3
    setting.cost2EntryMap[3] = ui->cost3->getEntryMapfromUI();
    // cost4
    setting.cost2EntryMap[4] = ui->cost4->getEntryMapfromUI();

    return setting;
}

void AbstractEntryPanel::setSingleEchoSetting2UI(const SingleEchoSetting& setting) {
    // 设置基本判断条件
    ui->isDiscardJudge->setChecked(setting.isDiscardedJudge);
    ui->isLockJudge->setChecked(setting.isLockJudge);
    ui->isNormalJudge->setChecked(setting.isNormalJudge);

    // 设置星级判断
    ui->level2->setChecked(setting.islevel2);
    ui->level3->setChecked(setting.islevel3);
    ui->level4->setChecked(setting.islevel4);
    ui->level5->setChecked(setting.islevel5);

    // 设置词条 (cost1, cost3, cost4)
    if (setting.cost2EntryMap.contains(1)) {
        ui->cost1->setEntryMap2UI(setting.cost2EntryMap[1]);
    }
    if (setting.cost2EntryMap.contains(3)) {
        ui->cost3->setEntryMap2UI(setting.cost2EntryMap[3]);
    }
    if (setting.cost2EntryMap.contains(4)) {
        ui->cost4->setEntryMap2UI(setting.cost2EntryMap[4]);
    }
}
