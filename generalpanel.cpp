#include "generalpanel.h"
#include "ui_generalpanel.h"

GeneralPanel::GeneralPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralPanel)
{
    ui->setupUi(this);
}

GeneralPanel::~GeneralPanel()
{
    delete ui;
}
