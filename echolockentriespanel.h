#ifndef ECHOLOCKENTRIESPANEL_H
#define ECHOLOCKENTRIESPANEL_H

#include <QWidget>

namespace Ui {
class EchoLockEntriesPanel;
}

class EchoLockEntriesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit EchoLockEntriesPanel(QWidget *parent = nullptr);
    ~EchoLockEntriesPanel();

private:
    Ui::EchoLockEntriesPanel *ui;
};

#endif // ECHOLOCKENTRIESPANEL_H
