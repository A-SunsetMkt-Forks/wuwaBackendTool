#ifndef IMAGECAPTURERMONITOR_H
#define IMAGECAPTURERMONITOR_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"
#include "imagecapturer.h"


class ImageCapturerMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ImageCapturerMonitor(QObject *parent = nullptr);
    void stop();
    bool isBusy();

signals:
    void updateGameMonitorStatus(const bool& isBusy, const cv::Mat& mat);


public slots:
    void on_start_monitor(const ImageCapturer* imageCapturer);

private:
    QAtomicInt m_isBusy;

};

#endif // IMAGECAPTURERMONITOR_H
