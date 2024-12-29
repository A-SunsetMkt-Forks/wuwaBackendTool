#ifndef UTILS_H
#define UTILS_H

/*
basic tool functions


 */

// qt lib
#include <QImage>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QDir>

// 获取窗体 句柄
#include <windows.h>
#include <wingdi.h>

#include <opencv2/opencv.hpp>

// C++标准库
#include <cmath>
#include <iostream>

class Utils
{
public:
    Utils();
    // try bind
    static bool initWuwaHwnd();

    // judge if wuwa process is still running
    static bool isWuwaRunning();

    static HWND hwnd;  // wuwa Handle to a Window

    // only support 1280 * 720 resolution
    static const int CLIENT_WIDTH;
    static const int CLIENT_HEIGHT;

    // store images dir
    // 懒加载静态变量
    static const QString& IMAGE_DIR() {
        static const QString imageDir = QDir(QCoreApplication::applicationDirPath()).filePath("noki");
        return imageDir;
    }

    // capture client image, time cost 5~15ms
    static QImage captureWindowToQImage(HWND hwnd, const DWORD mode = 0x00000003);

    // press sendKeyToWindow(HWND hwnd, int vkCode, WM_KEYDOWN)
    // release sendKeyToWindow(HWND hwnd, int vkCode, WM_KEYUP)
    static bool sendKeyToWindow(HWND hwnd, int vkCode, int keyStatus);

    static bool clickWindowClientArea(HWND hwnd, int x, int y);

    // get if it's admin
    static bool isRunningAsAdmin();

    // 对应按键精灵的FindPic
    static bool FindPic(const cv::Mat& sourceImage, const cv::Mat& templateImage, double threshold, int& outX, int& outY);

private:
    static const QString wuwaWindowTitle;
    static QMutex m_locker;  // multi thread locker

    // HBitmap -> QImage  不涉及成员变量或静态变量的修改，无需加锁
    static QImage HBitmapToQImage(HBITMAP hBitmap, HDC hDC);

    // QImage 和cvMat互转
    static QImage cvMat2QImage(const cv::Mat& mat);
    static cv::Mat qImage2CvMat(const QImage& image);



};

#endif // UTILS_H
