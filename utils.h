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

// 获取窗体 句柄
#include <windows.h>
#include <wingdi.h>


class Utils
{
public:
    Utils();
    // try bind
    static bool initWuwaHwnd();

    // judge if wuwa process is still running
    static bool isWuwaRunning();

    static HWND hwnd;  // wuwa Handle to a Window

    // capture client image, time cost 5~15ms
    static QImage captureWindowToQImage(HWND hwnd, const DWORD mode = 0x00000003);

    // press sendKeyToWindow(HWND hwnd, int vkCode, WM_KEYDOWN)
    // release sendKeyToWindow(HWND hwnd, int vkCode, WM_KEYUP)
    static bool sendKeyToWindow(HWND hwnd, int vkCode, int keyStatus);

    static bool clickWindowClientArea(HWND hwnd, int x, int y);

    // get if it's admin
    static bool isRunningAsAdmin();

private:
    static const QString wuwaWindowTitle;
    static QMutex m_locker;  // multi thread locker

    // HBitmap -> QImage  不涉及成员变量或静态变量的修改，无需加锁
    static QImage HBitmapToQImage(HBITMAP hBitmap, HDC hDC);

};

#endif // UTILS_H
