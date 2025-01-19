#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include <QWidget>
#include <QThread>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QRadioButton>
#include <QMessageBox>
#include <QCryptographicHash>

#include "settingparams.h"
#include "utils.h"
#include "debugbackendworker.h"


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

    QThread m_backendThread;
    DebugBackendWorker m_debugBackendWorker;

signals:

    // 测试重启游戏
    void startTestRebootGame(const QString& launcherWindowTitle);

    // 开启后台不断截图工作
    void startActivateCap(const QString& imgFilePath, const unsigned int& waitMs);

public slots:
    // 测试图像匹配功能
    void on_testFindPic_clicked();
    // 测试图像匹配功能
    void on_testFindColor_clicked();
    // 输入密码 启用测试功能
    void on_confirmPwd_clicked();



    // 测试通过启动器重启游戏
    void on_testRebootGame_clicked();

    // 启动激活 不断截图 或停止
    void on_activateBtn_clicked();
    void on_deactivateBtn_clicked();
    void on_scrCap_clicked();

    // 后台线程停止激活
    void onActivateCapDone(const bool& isOK, const QString& msg);


};

#endif // DEBUGFORM_H
