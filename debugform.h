#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include <QWidget>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QRadioButton>
#include <QMessageBox>
#include <QCryptographicHash>

#include "settingparams.h"
#include "utils.h"

namespace Ui {
class DebugForm;
}

class DebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit DebugForm(QWidget *parent = nullptr);
    ~DebugForm();

private:
    Ui::DebugForm *ui;
    bool isVisibleShow = false;
    QVector<QWidget*> lockWidgetList;

signals:
    // 片段测试 测试速切战斗
    void startTestFastSwitch();

    // 测试重启游戏
    void startTestRebootGame(const QString& launcherWindowTitle);

public slots:
    // 测试图像匹配功能
    void on_testFindPic_clicked();
    // 测试图像匹配功能
    void on_testFindColor_clicked();
    // 输入密码 启用测试功能
    void on_confirmPwd_clicked();

    // 测试按数字2
    void on_testPress2_clicked();
    // 测试按字母M
    void on_testPressM_clicked();

    // 测试两种战斗
    void on_testFastSwitch_clicked();

    // 测试通过启动器重启游戏
    void on_testRebootGame_clicked();
};

#endif // DEBUGFORM_H
