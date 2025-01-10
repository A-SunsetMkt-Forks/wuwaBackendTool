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
        QWidget* page = ui->tabWidget->widget(i);
        if (page) {
            echoSetNameList.push_back(page->objectName()); // 获取页面的 objectName
        }
        QString echoSetName = page->objectName();
        SingleEchoSetting singleEchoSetting;
        //singleEchoSetting.islevel2 =
    }


    return lockEchoSetting;
}
