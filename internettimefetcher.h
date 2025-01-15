#ifndef NETTIMER_H
#define NETTIMER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>


class InternetTimeFetcher : public QObject
{
    Q_OBJECT

public:
    explicit InternetTimeFetcher(QObject *parent = nullptr)
        : QObject(parent)
    {
        manager = new QNetworkAccessManager(this);

    }
    /*
    // 该接口保持不变，函数内进行阻塞式操作
    void fetchInternetTime(QDateTime& _dateTime)
    {
        // 使用 timeapi.io 的接口
        QUrl url("https://timeapi.io/api/Time/current/zone?timeZone=UTC");
        QNetworkRequest request(url);
        QNetworkReply *reply = manager->get(request);



        connect(reply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError> &errors){
            for (auto &e : errors) {
                qCritical() << "SSL Error:" << e.errorString();
            }
            // 测试用，先 ignore 掉
            reply->ignoreSslErrors();
        });
        // 使用 QEventLoop 阻塞直到 reply 完成或超时
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);

        // reply 完成信号连接到事件循环退出
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

        // exec() 在这里阻塞，
        loop.exec();


        // 如果是 reply 完成，则停止定时器
        timer.stop();
        auto error = reply->error();
        if (error == QNetworkReply::NoError) {
            // 解析返回数据
            const QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();

                // 先从 JSON 里取出对应字段
                int year         = jsonObj.value("year").toInt();
                int month        = jsonObj.value("month").toInt();
                int day          = jsonObj.value("day").toInt();
                int hour         = jsonObj.value("hour").toInt();
                int minute       = jsonObj.value("minute").toInt();
                int seconds      = jsonObj.value("seconds").toInt();
                int milliSeconds = jsonObj.value("milliSeconds").toInt();

                // 组装成 QDateTime（指定 UTC）
                QDate date(year, month, day);
                QTime time(hour, minute, seconds, milliSeconds);
                if (date.isValid() && time.isValid()) {
                    QDateTime dateTime(date, time, Qt::UTC);
                    dateTime = dateTime.toLocalTime();
                    _dateTime = dateTime;
                    qDebug() << "获取到的UTC时间:" << _dateTime.toString(Qt::ISODate)
                             << "（毫秒:" << milliSeconds << ")";
                } else {
                    _dateTime = QDateTime();
                    qDebug() << "时间解析失败: year/month/day等字段无效或缺失";
                }
            } else {
                _dateTime = QDateTime();
                qDebug() << "JSON 格式错误";
            }
        } else {
            _dateTime = QDateTime();
            qDebug() << "获取时间失败:" << reply->errorString();
        }


        reply->deleteLater();
    }
    */

    void fetchInternetTime(QDateTime& _dateTime)
    {
        QUrl url("https://timeapi.io/api/Time/current/zone?timeZone=UTC");
        QNetworkRequest request(url);
        QNetworkReply *reply = manager->get(request);

        connect(reply, &QNetworkReply::sslErrors, this, [=](const QList<QSslError>& errors){
            for (const auto& e : errors) {
                qCritical() << "SSL Error:" << e.errorString();
            }
            reply->ignoreSslErrors(); // 测试用，忽略 SSL 错误
        });

        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);

        // 定时器超时信号连接到事件循环退出
        connect(&timer, &QTimer::timeout, &loop, [&](){
            qDebug() << "获取时间超时";
            loop.quit();
        });

        // reply 完成信号连接到事件循环退出
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

        // 启动定时器，设定30秒超时
        timer.start(30000);

        // 阻塞事件循环直到超时或 reply 完成
        loop.exec();

        if (timer.isActive()) {
            // 定时器未超时，说明是 reply 完成
            timer.stop();
            auto error = reply->error();
            if (error == QNetworkReply::NoError) {
                // 解析返回数据
                const QByteArray response = reply->readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
                if (jsonDoc.isObject()) {
                    QJsonObject jsonObj = jsonDoc.object();

                    // 从 JSON 中获取时间字段
                    int year         = jsonObj.value("year").toInt();
                    int month        = jsonObj.value("month").toInt();
                    int day          = jsonObj.value("day").toInt();
                    int hour         = jsonObj.value("hour").toInt();
                    int minute       = jsonObj.value("minute").toInt();
                    int seconds      = jsonObj.value("seconds").toInt();
                    int milliSeconds = jsonObj.value("milliSeconds").toInt();

                    QDate date(year, month, day);
                    QTime time(hour, minute, seconds, milliSeconds);
                    if (date.isValid() && time.isValid()) {
                        QDateTime dateTime(date, time, Qt::UTC);
                        dateTime = dateTime.toLocalTime();
                        _dateTime = dateTime;
                        qDebug() << "获取到的UTC时间:" << _dateTime.toString(Qt::ISODate)
                                 << "（毫秒:" << milliSeconds << ")";
                    } else {
                        _dateTime = QDateTime();
                        qDebug() << "时间解析失败: year/month/day等字段无效或缺失";
                    }
                } else {
                    _dateTime = QDateTime();
                    qDebug() << "JSON 格式错误";
                }
            } else {
                _dateTime = QDateTime();
                qDebug() << "获取时间失败:" << reply->errorString();
            }
        } else {
            // 定时器超时处理
            _dateTime = QDateTime();
            qDebug() << "获取时间超时";
        }

        reply->deleteLater();
    }

private:
    QNetworkAccessManager *manager;
};


#endif // NETTIMER_H
