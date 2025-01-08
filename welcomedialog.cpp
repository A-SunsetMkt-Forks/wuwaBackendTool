#include "WelcomeDialog.h"
#include "ui_WelcomeDialog.h"
#include <QPushButton>
#include <QMessageBox>

WelcomeDialog::WelcomeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeDialog)
{
    ui->setupUi(this);
    countdownValue = Utils::WELCOME_SEC;

    // 禁用关闭功能和按钮
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    // 设置初始文本
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK (%1)").arg(countdownValue));

    // 创建并启动倒计时
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &WelcomeDialog::updateCountdown);
    countdownTimer->start(1000); // 每秒触发一次

    // 禁用所有关闭操作
    setWindowFlag(Qt::WindowCloseButtonHint, false);

    // 连接 OK 和 Cancel 按钮槽函数
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

WelcomeDialog::~WelcomeDialog()
{
    delete ui;
}

void WelcomeDialog::updateCountdown()
{
    countdownValue--;
    if (countdownValue > 0) {
        // 更新按钮文本
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK (%1)").arg(countdownValue));
    } else {
        // 倒计时结束，启用按钮和关闭功能
        countdownTimer->stop();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);

    }
}
