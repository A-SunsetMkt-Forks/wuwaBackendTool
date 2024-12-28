#include "utils.h"

const QString Utils::wuwaWindowTitle = "鸣潮  ";
QMutex Utils::m_locker;
HWND Utils::hwnd = nullptr;

Utils::Utils()
{

}

QImage Utils::HBitmapToQImage(HBITMAP hBitmap, HDC hDC) {
    try {
        BITMAP bmp;
        GetObject(hBitmap, sizeof(BITMAP), &bmp);

        // 创建 QImage 对象
        QImage image(bmp.bmWidth, bmp.bmHeight, QImage::Format_ARGB32);

        BITMAPINFOHEADER bi;
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = bmp.bmWidth;
        bi.biHeight = -bmp.bmHeight; // Top-down DIB
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        // 获取位图数据并填充到 QImage
        GetDIBits(hDC, hBitmap, 0, bmp.bmHeight, image.bits(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        return image;
    } catch (...) {
        qWarning() << "failed to convert HBitmapToQImage";
        return QImage();
    }

}

bool Utils::initWuwaHwnd(){
    QMutexLocker locker(&m_locker);
    // 转换为宽字符（Unicode）
    Utils::hwnd = FindWindow(nullptr, (LPCWSTR)wuwaWindowTitle.utf16());
    if (!hwnd) {
        qWarning() << "Failed to find window with title: " << wuwaWindowTitle;
        return false;
    }
    else{
        qInfo() << QString("wuwa hwnd is bind, hwnd 0x%1").arg(reinterpret_cast<quintptr>(hwnd), 0, 16);
        return true;
    }
}

bool Utils::isWuwaRunning(){
    QMutexLocker locker(&m_locker);
    if(hwnd == nullptr){
        qWarning() << QString("hwnd is nullptr");
        return false;
    }

    if (!IsWindow(hwnd)) {
        qWarning() << QString("the window is no longer valid, hwnd 0x%1").arg(reinterpret_cast<quintptr>(hwnd), 0, 16);
        return false;
    }
    else{
        return true;
    }
}

// 捕获窗口内容并转换为 QImage
QImage Utils::captureWindowToQImage(HWND hwnd, const DWORD mode) {
    if(!isWuwaRunning()) {
        return QImage();
    }

    RECT rect;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // 获取窗口设备上下文
    HDC hdcWindow = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcWindow);

    // 创建兼容位图
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
    SelectObject(hdcMemDC, hBitmap);

    // 使用 PrintWindow 捕获窗口内容
    // 0x00000002; 捕获完整内容，包括非客户区
    const DWORD PW_RENDERFULLCONTENT = mode; // 0x00000003 捕获仅客户区  实测3可以实现
    if (!PrintWindow(hwnd, hdcMemDC, PW_RENDERFULLCONTENT)) {
        qWarning() << "PrintWindow failed. Unable to capture content." ;
        DeleteObject(hBitmap);
        DeleteDC(hdcMemDC);
        ReleaseDC(hwnd, hdcWindow);
        return QImage();
    }

    // 将 HBITMAP 转换为 QImages
    QImage image = Utils::HBitmapToQImage(hBitmap, hdcWindow);
    if(image.isNull()){
        qWarning() << QString("failed to capture window image, hwnd 0x%1").arg(reinterpret_cast<quintptr>(hwnd), 0, 16);
        return QImage();
    }

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;
    qDebug() << QString("client width %1, height %2").arg(clientWidth).arg(clientHeight);

    // 如果捕获的图像大于客户区，则裁剪图像
    qDebug() << "Original image size:" << image.size(); // 打印原始尺寸
    if (image.width() > clientWidth || image.height() > clientHeight) {
        image = image.copy(0, 0, clientWidth, clientHeight); // 裁剪图像，保留左上部分
        qDebug() << "Image cropped to client dimensions.";
        qDebug() << "Cropped image size:" << image.size(); // 打印裁剪后的尺寸
    }

    // 释放资源
    DeleteObject(hBitmap);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcWindow);

    return image;
}


bool Utils::sendKeyToWindow(HWND hwnd, int vkCode, int keyStatus) {
    if (!isWuwaRunning()) {
        return false;
    }

    // Send key down
    PostMessage(hwnd, keyStatus, vkCode, 0);
    return true;
}

bool Utils::isRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin;
}


// 模拟鼠标点击目标窗口的客户区坐标
bool Utils::clickWindowClientArea(HWND hwnd, int x, int y) {
    if (!isWuwaRunning()) {
        return false;
    }

    // 将客户区坐标转换为屏幕坐标
    POINT clientPoint = { x, y };
    if (!ClientToScreen(hwnd, &clientPoint)) {
        qWarning() << "Failed to convert client area coordinates to screen coordinates.";
        return false;
    }

    // 如果 x, y 是“客户区坐标”，则直接组合到 lParam
    // MAKELPARAM 的低 16 位是 X 坐标，高 16 位是 Y 坐标
    LPARAM lParam = MAKELPARAM(x, y);

    // 发送鼠标按下消息
    PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
    Sleep(200);  // 给消息处理留一点缓冲时间

    // 发送鼠标松开消息
    PostMessage(hwnd, WM_LBUTTONUP, 0, lParam);
    Sleep(200);

    qDebug() << "Simulated click at client area coordinates: ("
              << x << ", " << y << ")" ;

    return true;
}
