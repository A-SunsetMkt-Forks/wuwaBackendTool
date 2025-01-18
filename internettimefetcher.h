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

        // 启动定时器，设定10秒超时
        timer.start(10000);

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
    */

    void fetchInternetTime(QDateTime& _dateTime)
    {
        QUrl url("https://www.baidu.com");
        QNetworkRequest request(url);
        QNetworkReply *reply = manager->head(request);

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

        // 启动定时器，设定10秒超时
        timer.start(10000);

        // 阻塞事件循环直到超时或 reply 完成
        loop.exec();

        if (timer.isActive()) {
            // 定时器未超时，说明是 reply 完成
            timer.stop();
            auto error = reply->error();
            if (error == QNetworkReply::NoError) {
                // 从返回的 HTTP 头部中获取 "Date" 字段
                QByteArray dateStr = reply->rawHeader("Date");
                if (!dateStr.isEmpty()) {
                    QDateTime dt = QDateTime::fromString(QString(dateStr), Qt::RFC2822Date);
                    if (dt.isValid()) {
                        // 转换为本地时间
                        dt.setTimeSpec(Qt::UTC);
                        _dateTime = dt.toLocalTime();
                        qDebug() << "获取到的网络时间(本地时区):" << _dateTime.toString(Qt::ISODate);
                    } else {
                        qWarning() << "解析 Date 头失败, 原始字符串:" << dateStr;
                        _dateTime = QDateTime();
                    }
                } else {
                    qWarning() << "响应头中无 Date 字段！";
                    _dateTime = QDateTime();
                }
            } else {
                // 请求失败
                qWarning() << "获取时间失败:" << reply->errorString();
                _dateTime = QDateTime();
            }
        } else {
            // 超时退出
            _dateTime = QDateTime();
        }

        reply->deleteLater();
    }



private:
    QNetworkAccessManager *manager;
};


#endif // NETTIMER_H
