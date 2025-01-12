#ifndef NORMALBOSSPANEL_H
#define NORMALBOSSPANEL_H

#include <QWidget>

namespace Ui {
class NormalBossPanel;
}

class NormalBossPanel : public QWidget
{
    Q_OBJECT

public:
    explicit NormalBossPanel(QWidget *parent = nullptr);
    ~NormalBossPanel();

private:
    Ui::NormalBossPanel *ui;
};

#endif // NORMALBOSSPANEL_H
