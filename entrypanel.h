#ifndef ENTRYPANEL_H
#define ENTRYPANEL_H

#include <QWidget>
#include "mainbackendworkernew.h"

namespace Ui {
class EntryPanel;
}

class EntryPanel : public QWidget
{
    Q_OBJECT

public:
    explicit EntryPanel(QWidget *parent = nullptr);
    ~EntryPanel();
    QVector<QString> getEntryMapfromUI();
    void setEntryMap2UI(const QVector<QString>& entries);

private:
    Ui::EntryPanel *ui;
};

#endif // ENTRYPANEL_H
