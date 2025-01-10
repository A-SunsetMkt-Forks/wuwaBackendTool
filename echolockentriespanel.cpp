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
