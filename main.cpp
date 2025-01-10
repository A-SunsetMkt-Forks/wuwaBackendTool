#include "mainwindow.h"
#include "welcomedialog.h"
#include <QApplication>
#include <QGuiApplication>

int main(int argc, char *argv[])
{


    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling); // 禁用高 DPI 缩放   AA_Use96Dpi
    //QApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication a(argc, argv);


    // 显示欢迎对话框
    WelcomeDialog welcomeDialog;
    welcomeDialog.exec(); // 等待欢迎对话框完成，无论结果如何

    // 显示主窗口
    MainWindow w;
    w.show();

    return a.exec();
}
