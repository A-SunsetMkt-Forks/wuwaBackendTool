#include "autochangewallpaperbackendworker.h"

AutoChangeWallpaperBackendworker::AutoChangeWallpaperBackendworker(QObject *parent) : QObject(parent)
{


}


void AutoChangeWallpaperBackendworker::stopWorker(){
    m_isBusy.store(0);
}

bool AutoChangeWallpaperBackendworker::isBusy(){
    if(m_isBusy.load() == 1){
        return true;
    }
    else{
        return false;
    }
}

void AutoChangeWallpaperBackendworker::startWorker(){
    m_isBusy.store(1);
    qInfo() << QString("AutoChangeWallpaperBackendworker::startWorker()");



    // 读取图像
    // 获取 exe 路径
    QString exePath = QCoreApplication::applicationDirPath();
    QString wallpaperPath = exePath + "/wallpaper";
    QDir dir(wallpaperPath);

    // 检查路径是否存在
    if (!dir.exists()) {
        qWarning() << "Wall paper directory does not exist:" << wallpaperPath;
        return;
    }

    // 文件过滤器
    QStringList filters;
    filters << "*.bmp" << "*.png" << "*.jpg";

    // 获取符合条件的文件列表
    m_wallpaperFileList = dir.entryList(filters, QDir::Files);
    qDebug() << "找到壁纸 " << m_wallpaperFileList;

    // 读取文件
    m_wallpaperImages.clear();
    m_wallpaperFullPaths.clear();
    for (const QString &fileName : m_wallpaperFileList) {
        QString filePath = dir.absoluteFilePath(fileName);
        // 使用 QImage 加载图片
        QImage image(filePath);
        if (image.isNull()) {
            qWarning() << "Failed to load image:" << filePath;
            continue;
        }
        qDebug() << "Loading image:" << filePath;

        // 转换为 RGB 格式 (8UC3)
        QImage convertedImage = image.convertToFormat(QImage::Format_RGB888);


        // 按照 4:3格式进行居中切割
        int originalWidth = convertedImage.width();
        int originalHeight = convertedImage.height();

        int targetWidth;
        int targetHeight;

        if (originalWidth * m_wallpaperHeightR >= originalHeight * m_wallpaperWidthR) {
            // 宽度较长，裁剪宽度
            targetHeight = originalHeight;
            targetWidth = (originalHeight * m_wallpaperWidthR) / m_wallpaperHeightR;
        } else {
            // 高度较长，裁剪高度
            targetWidth = originalWidth;
            targetHeight = (originalWidth * m_wallpaperHeightR) / m_wallpaperWidthR;
        }

        int xOffset = (originalWidth - targetWidth) / 2;
        int yOffset = (originalHeight - targetHeight) / 2;

        QImage croppedImage = convertedImage.copy(xOffset, yOffset, targetWidth, targetHeight);

        // 存入 vector
        m_wallpaperImages.push_back(croppedImage);
        m_wallpaperFullPaths.push_back(filePath);
        qDebug() << "Successfully cropped and added image:" << filePath;

        if(!isBusy()){
            return;
        }
    }

    // 每隔若干秒发送一张图
    while(isBusy()){
        for(int i = 0; i < m_wallpaperImages.size(); i++){
            qDebug() << QString("change wallpaper : %1").arg(m_wallpaperFullPaths[i]);
            emit sendImageAsWallpaper(m_wallpaperImages[i]);
            for(int j = 0; j < m_eachWallpaperSec * 10 && isBusy(); j++){
                QThread::msleep(100);
            }

            if(!isBusy()){
                return;
            }
        }

        if(!isBusy()){
            return;
        }
    }

}
