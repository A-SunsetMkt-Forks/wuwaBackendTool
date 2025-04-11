
#ifndef DEBUGMESSAGEHANDLER_H
#define DEBUGMESSAGEHANDLER_H

#include <QApplication>
#include <QPlainTextEdit>
#include <QMutex>
#include <QDebug>
#include <QObject>
#include <QMetaObject>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextBlock>
#include <QDateTime>
#include <QDir>

class DebugMessageHandler : public QObject {
    Q_OBJECT
public:

    static DebugMessageHandler& instance() {
        static DebugMessageHandler handler;
        return handler;
    }

    void setPlainTextEdit(QPlainTextEdit* editor) {
        QMutexLocker locker(&mutex);
        if (editor) {
            this->editor = editor;
            this->editor->setReadOnly(true);  // 设置为只读
            this->editor->setStyleSheet("background-color: white;");  // 设置背景颜色为白色
        } else {
            this->editor = nullptr;  // 取消绑定重定向
        }
    }

    void setLogLevel(QtMsgType type) {
        QMutexLocker locker(&mutex);
        logLevel = type;
    }
    /*
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        Q_UNUSED(context);

        DebugMessageHandler& handler = DebugMessageHandler::instance();
        QMutexLocker locker(&handler.mutex);

        if (handler.editor && handler.shouldLog(type)) {
            QString formattedMsg = msg;
            QTextCharFormat format;

            switch (type) {
                case QtDebugMsg:
                    format.setForeground(Qt::black);
                    format.setBackground(Qt::white);
                    break;
                case QtInfoMsg:
                    format.setForeground(Qt::blue);
                    format.setBackground(Qt::white);
                    break;
                case QtWarningMsg:
                    format.setForeground(Qt::yellow);
                    format.setBackground(Qt::black);
                    break;
                case QtCriticalMsg:
                    format.setForeground(Qt::red);
                    format.setBackground(Qt::white);
                    break;
                case QtFatalMsg:
                    format.setForeground(Qt::red);
                    format.setBackground(Qt::black);
                    format.setFontWeight(QFont::Bold);
                    break;
            }

            QMetaObject::invokeMethod(&handler, "appendFormattedText", Qt::QueuedConnection, Q_ARG(QString, formattedMsg), Q_ARG(QTextCharFormat, format));
        }
    }
    */

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        DebugMessageHandler& handler = DebugMessageHandler::instance();
        QMutexLocker locker(&handler.mutex);

        if (handler.editor && handler.shouldLog(type)) {
            QString formattedMsg = msg;  // 只在 UI 上显示原始消息
            QTextCharFormat format;

            switch (type) {
                case QtDebugMsg:
                    format.setForeground(Qt::black);
                    format.setBackground(Qt::white);
                    break;
                case QtInfoMsg:
                    format.setForeground(Qt::blue);
                    format.setBackground(Qt::white);
                    break;
                case QtWarningMsg:
                    format.setForeground(Qt::yellow);
                    format.setBackground(Qt::black);
                    break;
                case QtCriticalMsg:
                    format.setForeground(Qt::red);
                    format.setBackground(Qt::white);
                    break;
                case QtFatalMsg:
                    format.setForeground(Qt::red);
                    format.setBackground(Qt::black);
                    format.setFontWeight(QFont::Bold);
                    break;
            }

            // 将日志追加到 UI
            QMetaObject::invokeMethod(&handler, "appendFormattedText", Qt::QueuedConnection, Q_ARG(QString, formattedMsg), Q_ARG(QTextCharFormat, format));

            // 将日志写入文件，包含更多详细信息
            handler.writeToLogFile(msg, type, context);
        }
    }
public slots:
    void appendFormattedText(const QString& text, const QTextCharFormat& format) {
        if (editor) {
            QTextCursor cursor(editor->document());
            cursor.movePosition(QTextCursor::End);
            cursor.insertText(text + '\n', format);
        }
        // 写入日志文件
        //writeToLogFile(text);
    }

private:
    DebugMessageHandler()
        : editor(nullptr), logLevel(QtDebugMsg), logFile(nullptr), logStream(nullptr) {
        createLogFile();  // 初始化时尝试创建日志文件
    }

    ~DebugMessageHandler() {
        // 析构时关闭文件句柄
        if (logStream) {
            logStream->flush();
            delete logStream;
        }
        if (logFile) {
            logFile->close();
            delete logFile;
        }
    }

    void createLogFile() {
        // 创建日志目录和文件
        QDir logDir(QCoreApplication::applicationDirPath() + "/log");
        if (!logDir.exists()) {
            logDir.mkpath(".");
        }

        QString logFileName = logDir.absoluteFilePath(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".log");
        logFile = new QFile(logFileName);
        if (logFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
            logStream = new QTextStream(logFile);
            logStream->setCodec("UTF-8");
        } else {
            delete logFile;
            logFile = nullptr;
            qWarning() << "Failed to create log file:" << logFileName;
        }
    }
    /*
    void writeToLogFile(const QString& text) {
        if (logStream) {
            *logStream << QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss] ") << text << '\n';
            logStream->flush();
        }
    }
    */

    void writeToLogFile(const QString& text, QtMsgType type, const QMessageLogContext& context) {
        if (logStream) {
            // 构造日志等级的字符串表示
            QString logLevel;
            switch (type) {
                case QtDebugMsg: logLevel = "DEBUG"; break;
                case QtInfoMsg: logLevel = "INFO"; break;
                case QtWarningMsg: logLevel = "WARNING"; break;
                case QtCriticalMsg: logLevel = "CRITICAL"; break;
                case QtFatalMsg: logLevel = "FATAL"; break;
            }

            // 写入日志文件，包含时间戳、等级、文件、行号等
            *logStream << QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss] ")
                       << "[" << logLevel << "] "
                       << "[" << context.file << ":" << context.line << "] "
                       << text << '\n';
            logStream->flush();
        }
    }


    bool shouldLog(QtMsgType type) {
        // 定义枚举优先级映射
        static const std::map<QtMsgType, int> logPriority = {
            {QtDebugMsg, 0},
            {QtInfoMsg, 1},
            {QtWarningMsg, 2},
            {QtCriticalMsg, 3},
            {QtFatalMsg, 4}
        };

        // 确保 logLevel 也有一个对应的优先级值
        int logLevelPriority = logPriority.at(logLevel);

        // 比较当前消息类型的优先级是否 >= logLevel 的优先级
        auto it = logPriority.find(type);
        if (it != logPriority.end()) {
            return it->second >= logLevelPriority;
        }

        // 如果消息类型未定义，默认不记录
        return false;
    }

    QPlainTextEdit* editor;
    QMutex mutex;
    QtMsgType logLevel;
    QFile* logFile;               // 日志文件
    QTextStream* logStream;       // 用于写入日志的流
};

#endif // DEBUGMESSAGEHANDLER_H

