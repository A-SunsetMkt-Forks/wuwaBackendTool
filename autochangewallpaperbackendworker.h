#ifndef AUTOCHANGEWALLPAPERBACKENDWORKER_H
#define AUTOCHANGEWALLPAPERBACKENDWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <QThread>

class AutoChangeWallpaperBackendworker : public QObject
{
    Q_OBJECT
public:
    explicit AutoChangeWallpaperBackendworker(QObject *parent = nullptr);
    void stopWorker();
    bool isBusy();

signals:
    void sendImageAsWallpaper(const QImage& wallpaper);

public slots:
    void startWorker();


private:
    QAtomicInt m_isBusy;
    QStringList m_wallpaperFileList;  // 准备读取的图像列表
    QVector<QImage> m_wallpaperImages;
    QVector<QString> m_wallpaperFullPaths;  // 实际读取成功的

    const int m_eachWallpaperSec = 3;
    const int m_wallpaperWidthR = 4;
    const int m_wallpaperHeightR = 3;

};

#endif // AUTOCHANGEWALLPAPERBACKENDWORKER_H
