#include "echolockentriespanel.h"
#include "ui_echolockentriespanel.h"

EchoLockEntriesPanel::EchoLockEntriesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EchoLockEntriesPanel)
{
    ui->setupUi(this);


}

EchoLockEntriesPanel::~EchoLockEntriesPanel()
{
    delete ui;
}
