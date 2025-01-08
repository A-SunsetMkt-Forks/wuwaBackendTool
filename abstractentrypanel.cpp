#include "abstractentrypanel.h"
#include "ui_abstractentrypanel.h"

AbstractEntryPanel::AbstractEntryPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbstractEntryPanel)
{
    ui->setupUi(this);
}

AbstractEntryPanel::~AbstractEntryPanel()
{
    delete ui;
}
