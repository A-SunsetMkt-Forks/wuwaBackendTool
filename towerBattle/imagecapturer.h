#ifndef IMAGECAPTURER_H
#define IMAGECAPTURER_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>

#include "towerBattle/towerbattledatamanager.h"
#include "utils.h"

// 负责以一定帧率傻瓜式采图
class ImageCapturer : public QObject
{
    Q_OBJECT
public:
    explicit ImageCapturer(QObject *parent = nullptr);
    void stop();
    bool isBusy();

signals:

public slots:
    void on_start_capturer();

private:
    QAtomicInt m_isBusy;
};

#endif // IMAGECAPTURER_H
