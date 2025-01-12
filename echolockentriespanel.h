#ifndef ECHOLOCKENTRIESPANEL_H
#define ECHOLOCKENTRIESPANEL_H

#include <QWidget>
#include "mainbackendworkernew.h"
#include "abstractentrypanel.h"
#include "settingparams.h"
/*
声骸锁定面板
上面是套装分页
下面是abstractentrypanel 包含锁定、弃置、cost1 3 4
abstractentrypanel下面是entrypanel表示主词条比如攻击力、双爆 属性伤害等
 */

namespace Ui {
class EchoLockEntriesPanel;
}

class EchoLockEntriesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit EchoLockEntriesPanel(QWidget *parent = nullptr);
    ~EchoLockEntriesPanel();
    LockEchoSetting getLockEchoSettingFromUI();

    void setLockEchoSetting2UI(const LockEchoSetting& setting);

    void genDefaultSetting(LockEchoSetting &setting);

private:
    Ui::EchoLockEntriesPanel *ui;
};

#endif // ECHOLOCKENTRIESPANEL_H
