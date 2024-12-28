#ifndef GENERALPANEL_H
#define GENERALPANEL_H

#include <QWidget>

namespace Ui {
class GeneralPanel;
}

class GeneralPanel : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralPanel(QWidget *parent = nullptr);
    ~GeneralPanel();

private:
    Ui::GeneralPanel *ui;
};

#endif // GENERALPANEL_H
