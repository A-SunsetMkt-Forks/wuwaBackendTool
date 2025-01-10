#ifndef ABSTRACTENTRYPANEL_H
#define ABSTRACTENTRYPANEL_H

#include <QWidget>
#include "mainbackendworkernew.h"

namespace Ui {
class AbstractEntryPanel;
}

class AbstractEntryPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractEntryPanel(QWidget *parent = nullptr);
    ~AbstractEntryPanel();
    SingleEchoSetting getSingleEchoSettingFromUI();
    void setSingleEchoSetting2UI(const SingleEchoSetting& setting);

private:
    Ui::AbstractEntryPanel *ui;
};

#endif // ABSTRACTENTRYPANEL_H
