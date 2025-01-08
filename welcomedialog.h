#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>
#include <QTimer>
#include <utils.h>

namespace Ui {
class WelcomeDialog;
}

class WelcomeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeDialog(QWidget *parent = nullptr);
    ~WelcomeDialog();

private slots:
    void updateCountdown();

private:
    Ui::WelcomeDialog *ui;
    QTimer *countdownTimer;
    int countdownValue;

};

#endif // WELCOMEDIALOG_H
