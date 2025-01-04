#include "utils.h"

const QString Utils::wuwaWindowTitle = "鸣潮  ";
QMutex Utils::m_locker;
HWND Utils::hwnd = nullptr;

const int Utils::CLIENT_WIDTH = 1280;
const int Utils::CLIENT_HEIGHT = 720;

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

    QMutexLocker locker(&m_locker);
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

    QMutexLocker locker(&m_locker);
    // Send key down
    PostMessage(hwnd, keyStatus, vkCode, 0);
    QString logMessage = QString("Key: %1 (%2), Action: %3")
            .arg(QChar(vkCode))
            .arg(vkCode)
            .arg(keyStatus == WM_KEYDOWN ? "Pressed" :
                                           keyStatus == WM_KEYUP ? "Released" :
                                                                   "Unknown");
    qDebug() << logMessage;
    return true;
}

bool Utils::keyPress(HWND hwnd, int vkCode, int times){
    if (!isWuwaRunning()) {
        return false;
    }

    for(int i = 0 ; i < times; i++){
        sendKeyToWindow(hwnd, vkCode, WM_KEYDOWN);
        Sleep(10);
        sendKeyToWindow(hwnd, vkCode, WM_KEYUP);
        Sleep(10);
    }

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

    QMutexLocker locker(&m_locker);
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
    Sleep(20);  // 给消息处理留一点缓冲时间

    // 发送鼠标松开消息
    PostMessage(hwnd, WM_LBUTTONUP, 0, lParam);
    Sleep(20);

    qDebug() << "Simulated click at client area coordinates: ("
             << x << ", " << y << ")" ;

    return true;
}

// 模拟鼠标中键点击目标窗口的客户区坐标
bool Utils::middleClickWindowClientArea(HWND hwnd, int x, int y) {
    if (!isWuwaRunning()) {
        return false;
    }

    QMutexLocker locker(&m_locker);
    // 将客户区坐标转换为屏幕坐标
    POINT clientPoint = { x, y };
    if (!ClientToScreen(hwnd, &clientPoint)) {
        qWarning() << "Failed to convert client area coordinates to screen coordinates.";
        return false;
    }

    // 如果 x, y 是“客户区坐标”，则直接组合到 lParam
    // MAKELPARAM 的低 16 位是 X 坐标，高 16 位是 Y 坐标
    LPARAM lParam = MAKELPARAM(x, y);

    // 发送鼠标中键按下消息
    PostMessage(hwnd, WM_MBUTTONDOWN, MK_MBUTTON, lParam);
    Sleep(20);  // 给消息处理留一点缓冲时间

    // 发送鼠标中键松开消息
    PostMessage(hwnd, WM_MBUTTONUP, 0, lParam);
    Sleep(20);

    qDebug() << "Simulated middle click at client area coordinates: ("
             << x << ", " << y << ")" ;

    return true;
}

// 实现 cv::Mat 转 QImage
QImage Utils::cvMat2QImage(const cv::Mat& mat) {
    if (mat.empty()) {
        return QImage();
    }

    switch (mat.type()) {
    case CV_8UC1: {
        // 单通道灰度图
        QImage image(mat.cols, mat.rows, QImage::Format_Grayscale8);
        memcpy(image.bits(), mat.data, static_cast<size_t>(mat.cols * mat.rows));
        return image;
    }
    case CV_8UC3: {
        // 三通道彩色图 (BGR to RGB)
        QImage image(mat.cols, mat.rows, QImage::Format_RGB888);
        for (int row = 0; row < mat.rows; ++row) {
            memcpy(image.scanLine(row), mat.ptr(row), static_cast<size_t>(mat.cols * 3));
        }
        return image.rgbSwapped(); // BGR -> RGB
    }
    case CV_8UC4: {
        // 四通道带透明度图 (BGRA to RGBA)
        QImage image(mat.cols, mat.rows, QImage::Format_ARGB32);
        for (int row = 0; row < mat.rows; ++row) {
            memcpy(image.scanLine(row), mat.ptr(row), static_cast<size_t>(mat.cols * 4));
        }
        return image;
    }
    default:
        return QImage();
    }
}

// 实现 QImage 转 cv::Mat
cv::Mat Utils::qImage2CvMat(const QImage& image) {
    if (image.isNull()) {
        return cv::Mat();
    }

    switch (image.format()) {
    case QImage::Format_Grayscale8: {
        // 单通道灰度图
        cv::Mat mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        return mat.clone(); // 返回一个深拷贝，避免原数据被更改
    }
    case QImage::Format_RGB888: {
        // 三通道彩色图 (RGB to BGR)
        cv::Mat mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat matBGR;
        cv::cvtColor(mat, matBGR, cv::COLOR_RGB2BGR); // RGB -> BGR
        return matBGR;
    }
    case QImage::Format_ARGB32: {
        // 四通道带透明度图 (RGBA to BGRA)
        cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat matBGRA;
        cv::cvtColor(mat, matBGRA, cv::COLOR_RGBA2BGRA); // RGBA -> BGRA
        return matBGRA;
    }
    default:
        return cv::Mat();
    }
}

bool Utils::findPic(const cv::Mat& sourceImage, const cv::Mat& templateImage, double threshold, int& outX, int& outY) {
    if (sourceImage.empty() || templateImage.empty()) {
        std::cerr << "Error: Image(s) could not be loaded." << std::endl;
        return false;
    }

    // 创建结果矩阵
    cv::Mat result;
    cv::matchTemplate(sourceImage, templateImage, result, cv::TM_CCOEFF_NORMED);

    // 查找结果中最大匹配值
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // 检查是否超过阈值
    if (maxVal >= threshold) {
        outX = maxLoc.x;
        outY = maxLoc.y;
        return true; // 匹配成功
    }

    outX = -1;
    outY = -1;
    return false; // 匹配失败
}

// 在图像中查找指定颜色
bool Utils::findColorEx(const cv::Mat& image, int x1, int y1, int x2, int y2, const QString& hexColor, double tolerance, int& outX, int& outY) {
    // 检查 tolerance 是否在有效范围 [0, 1]
    if (tolerance < 0.0 || tolerance > 1.0) {
        qWarning() << "Tolerance should be between 0 and 1.";
        outX = -1;
        outY = -1;
        return false;
    }

    // 转换颜色
    cv::Vec3b targetColor;
    bool isConvertColor = Utils::hexToBGR(hexColor, targetColor);
    if (!isConvertColor) {
        qWarning() << "Failed to convert hex color to BGR.";
        outX = -1;
        outY = -1;
        return false;
    }

    // 限制搜索区域，确保区域在图像范围内
    x1 = std::max(0, x1);
    y1 = std::max(0, y1);
    x2 = std::min(image.cols, x2);
    y2 = std::min(image.rows, y2);

    if (x2 <= x1 || y2 <= y1) {
        qWarning() << "Invalid search region.";
        outX = -1;
        outY = -1;
        return false;
    }

    cv::Rect searchRegion(x1, y1, x2 - x1, y2 - y1);
    cv::Mat region = image(searchRegion);

    // 计算最大可能的颜色距离
    const double max_distance = std::sqrt(3.0 * 255.0 * 255.0); // ~441.67

    // 计算允许的最大距离，根据 tolerance 反转逻辑
    double allowed_distance = (1.0 - tolerance) * max_distance;

    // 遍历搜索区域内的所有像素
    for (int y = 0; y < region.rows; ++y) {
        for (int x = 0; x < region.cols; ++x) {
            cv::Vec3b pixelColor = region.at<cv::Vec3b>(y, x);

            // 计算颜色差异（欧几里得距离）
            double distance = std::sqrt(
                        std::pow(static_cast<double>(pixelColor[0]) - targetColor[0], 2) + // B
                    std::pow(static_cast<double>(pixelColor[1]) - targetColor[1], 2) + // G
                    std::pow(static_cast<double>(pixelColor[2]) - targetColor[2], 2)   // R
                    );

            // 判断是否满足容忍误差
            if (distance <= allowed_distance) {
                outX = x1 + x; // 转换为原图的坐标
                outY = y1 + y;
                qDebug() << QString("distance = %1, <= allowed_distance %2. Found the color.").arg(distance).arg(allowed_distance);
                return true; // 找到匹配颜色
            }
        }
    }

    qDebug() << "Failed to find the color.";
    // 未找到匹配颜色
    outX = -1;
    outY = -1;
    return false;
}

bool Utils::hexToBGR(const QString& hexColor, cv::Vec3b& outColor) {
    // 检查长度是否为 6 个字符
    if (hexColor.length() != 6) {
        return false;
    }

    bool ok;
    int r, g, b;

    // 尝试解析 R、G、B 分量
    r = hexColor.mid(0, 2).toInt(&ok, 16);
    if (!ok) return false;

    g = hexColor.mid(2, 2).toInt(&ok, 16);
    if (!ok) return false;

    b = hexColor.mid(4, 2).toInt(&ok, 16);
    if (!ok) return false;

    // 设置输出 BGR 颜色（OpenCV 使用 BGR 格式）
    outColor = cv::Vec3b(b, g, r);
    return true;
}
