#include "normalbosspanel.h"
#include "ui_normalbosspanel.h"

NormalBossPanel::NormalBossPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalBossPanel)
{
    ui->setupUi(this);
}

NormalBossPanel::~NormalBossPanel()
{
    delete ui;
}
