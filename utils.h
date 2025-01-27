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
#include <QTime>
#include <QProcess>

// 获取窗体 句柄
#include <windows.h>
#include <wingdi.h>
//#include <TlHelp32.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

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


    // 枚举获取所有的窗体
    static QStringList getAllWindowTitles();

    // 输入窗体名 获取句柄和PID
    static bool getWindowHandleAndPID(const QString& windowTitle, HWND& hwnd, DWORD& pid);

    static HWND hwnd;  // wuwa Handle to a Window

    // only support 1280 * 720 resolution
    static const int CLIENT_WIDTH;
    static const int CLIENT_HEIGHT;

    // 软件开启强制倒计时秒数
    static const int WELCOME_SEC;

    static QString EXE_PATH() {
        QMutexLocker locker(&m_locker);
        return m_exePath;
    }

    // store images dir
    // 懒加载静态变量
    static const QString& IMAGE_DIR() {
        static const QString imageDir = QDir(QCoreApplication::applicationDirPath()).filePath("noki");
        return imageDir;
    }

    static const QString& IMAGE_DIR_EI() {
        static const QString imageDir = QDir(QCoreApplication::applicationDirPath()).filePath("ElPsyKongroo");
        return imageDir;
    }

    static const QString& IMAGE_DIR_DEBUG(){
        static const QString imageDir = QDir(QCoreApplication::applicationDirPath()).filePath("bugreport");
        return imageDir;
    }


    // capture client image, time cost 5~15ms
    static QImage captureWindowToQImage(HWND hwnd, const DWORD mode = 0x00000003);

    // press sendKeyToWindow(HWND hwnd, int vkCode, WM_KEYDOWN)
    // release sendKeyToWindow(HWND hwnd, int vkCode, WM_KEYUP)
    static bool sendKeyToWindow(HWND hwnd, int vkCode, int keyStatus);

    // 模仿按键精灵连续（按下、松开）几次某按键
    static bool keyPress(HWND hwnd, int vkCode, int times);

    // 点击客户端的某个位置 鼠标左键
    static bool clickWindowClientArea(HWND hwnd, int x, int y);
    static bool clickWindowClientArea2(HWND hwnd, int x, int y);
    // 向窗体发送单击指令
    static bool clickWindowClient(HWND hwnd);

    // 向窗体发送滚轮指令 要求传送值是120的整数倍 可以是负数
    static bool scrollWindowClientArea(HWND hwnd, int x, int y, int delta);
    // 向窗体发送拖拽指令
    static bool dragWindowClient(HWND hwnd, int startx, int starty, int endx, int endy);
    static bool dragWindowClient2(HWND hwnd, int startx, int starty, int endx, int endy);
    static bool dragWindowClient3(HWND hwnd, int startx, int starty, int endx, int endy);
    // 点击客户端的某个位置 鼠标中键
    static bool middleClickWindowClientArea(HWND hwnd, int x, int y);

    // 移动鼠标到指定坐标
    static bool moveMouseToClientArea(HWND hwnd, int x, int y);

    // get if it's admin
    static bool isRunningAsAdmin();

    // 对应按键精灵的FindPic
    static bool findPic(const cv::Mat& sourceImage, const cv::Mat& templateImage, double threshold, int& outX, int& outY);
    static bool findPic(const cv::Mat& sourceImage, const cv::Mat& templateImage, double threshold, int& outX, int& outY, double& similarity);

    // 对应按键精灵的FindColorEx
    static bool findColorEx(const cv::Mat& image, int x1, int y1, int x2, int y2, const QString& hexColor, double tolerance, int& outX, int& outY);

    // QImage 和cvMat互转
    static QImage cvMat2QImage(const cv::Mat& mat);
    static cv::Mat qImage2CvMat(const QImage& image);

    // "F48A94" 转换为颜色
    static bool hexToBGR(const QString& hexColor, cv::Vec3b& outColor);

    // 调试使用 保存某个场景 做标记图并保存
    static bool saveDebugImg(const cv::Mat& scrShot, const cv::Rect& templateRoi, const int& clickX, const int& clickY, const QString& hint);

    // winapi 启动一个exe
    static bool myCreateProcess(const std::wstring &exePath, const std::wstring &workingDir);

private:
    static const QString wuwaWindowTitle;
    static QMutex m_locker;  // multi thread locker

    // HBitmap -> QImage  不涉及成员变量或静态变量的修改，无需加锁
    static QImage HBitmapToQImage(HBITMAP hBitmap, HDC hDC);


    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

    // 一旦启动后 绑定鸣潮exe路径
    static QString m_exePath;

};

#endif // UTILS_H
