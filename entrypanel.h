#ifndef ENTRYPANEL_H
#define ENTRYPANEL_H

#include <QWidget>

namespace Ui {
class EntryPanel;
}

class EntryPanel : public QWidget
{
    Q_OBJECT

public:
    explicit EntryPanel(QWidget *parent = nullptr);
    ~EntryPanel();

private:
    Ui::EntryPanel *ui;
};

#endif // ENTRYPANEL_H
