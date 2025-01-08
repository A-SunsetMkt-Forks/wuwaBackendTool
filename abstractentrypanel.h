#ifndef ABSTRACTENTRYPANEL_H
#define ABSTRACTENTRYPANEL_H

#include <QWidget>

namespace Ui {
class AbstractEntryPanel;
}

class AbstractEntryPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractEntryPanel(QWidget *parent = nullptr);
    ~AbstractEntryPanel();

private:
    Ui::AbstractEntryPanel *ui;
};

#endif // ABSTRACTENTRYPANEL_H
