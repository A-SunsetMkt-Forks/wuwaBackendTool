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

RebootGameSetting GeneralPanel::getRebootGameSetting() const {
    RebootGameSetting setting;

    // Retrieve the values from the UI
    if (ui->rebootCycles) {
        setting.rebootCycles = static_cast<unsigned int>(ui->rebootCycles->value());
    }

    if (ui->rebootWaitS) {
        setting.rebootWaitS = static_cast<unsigned int>(ui->rebootWaitS->value());
    }

    if (ui->startBtnX && ui->startBtnY) {
        setting.startGameBtnPos = QPoint(ui->startBtnX->value(), ui->startBtnY->value());
    }

    return setting;
}

void GeneralPanel::on_testFindPic_clicked(){
    qInfo() << QString("GeneralPanel::on_testFindPic_clicked");

    QString tempImgFullPath = QFileDialog::getOpenFileName(this, "打开待匹配的小图像");
    if(tempImgFullPath.isEmpty()){
        return;
    }
    cv::Mat templateImg = cv::imread(tempImgFullPath.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(templateImg.empty()){
        qWarning() << QString("failed to load %1").arg(tempImgFullPath);
        return;
    }

    if(templateImg.channels() == 3){

    }
    else if(templateImg.channels() == 4)  {
        cv::cvtColor(templateImg, templateImg, cv::COLOR_BGRA2BGR);
    }
    else{
        qWarning() << QString("%1 is not rgb image").arg(tempImgFullPath);
        return;
    }

    QString capImgFullPath = Utils::IMAGE_DIR() + "/" + "capWuwa.png";
    cv::Mat capImg = cv::imread(capImgFullPath.toLocal8Bit().toStdString(), cv::IMREAD_UNCHANGED);
    if(capImg.empty()){
        qWarning() << QString("failed to load %1").arg(capImgFullPath);
        return;
    }

    if(capImg.channels() == 3){

    }
    else if(capImg.channels() == 4)  {
        cv::cvtColor(capImg, capImg, cv::COLOR_BGRA2BGR);
    }
    else{
        qWarning() << QString("%1 is not rgb image").arg(capImgFullPath);
        return;
    }

    int X, Y;
    QElapsedTimer timer;
    timer.start();
    bool isFind = Utils::findPic(capImg, templateImg, 0.9, X, Y);
    auto timeCostMs = timer.elapsed();
    qDebug() << QString("GeneralPanel::on_testFindPic_clicked is done, timeCost %1 ms, X %2, Y %3, isFound %4").arg(timeCostMs).arg(X).arg(Y).arg(isFind);

    // 如果找到模板，绘制蓝色矩形
    if (isFind) {
        cv::Mat markedImg = capImg.clone();
        cv::rectangle(
                    markedImg,
                    cv::Point(X, Y),
                    cv::Point(X + templateImg.cols, Y + templateImg.rows),
                    cv::Scalar(255, 0, 0), // 蓝色
                    2 // 线条宽度
                    );

        // 显示标记后的图像
        cv::imshow("Matched Image", markedImg);
        cv::waitKey(0); // 等待按键
        cv::destroyAllWindows(); // 关闭窗口
    } else {
        qDebug() << "Template not found in the source image.";
    }
}
