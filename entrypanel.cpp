#include "entrypanel.h"
#include "ui_entrypanel.h"

EntryPanel::EntryPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryPanel)
{
    ui->setupUi(this);

    // 允许样式表生效
    setAttribute(Qt::WA_StyledBackground, true);

    // 设置背景透明
    setStyleSheet("background: transparent;");
}

EntryPanel::~EntryPanel()
{
    delete ui;
}
