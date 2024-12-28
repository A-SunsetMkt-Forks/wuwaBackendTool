#include "mainwindow.h"
#include <QApplication>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling); // 禁用高 DPI 缩放   AA_Use96Dpi
    //QApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
