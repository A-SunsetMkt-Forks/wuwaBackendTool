#ifndef GLOBALHOTKEYFILTER_H
#define GLOBALHOTKEYFILTER_H

#include <windows.h>

#include <QApplication>
#include <QObject>
#include <QAbstractNativeEventFilter>

class GlobalHotKeyFilter : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    explicit GlobalHotKeyFilter(QObject* parent = nullptr)
        : QObject(parent) {}

    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override {
        Q_UNUSED(result)
        if (eventType == "windows_generic_MSG") {
            MSG* msg = static_cast<MSG*>(message);
            if (msg->message == WM_HOTKEY) {
                emit hotKeyPressed(static_cast<int>(msg->wParam)); // 发出信号，传递热键 ID
                return true; // 消耗该消息
            }
        }
        return false;
    }

signals:
    void hotKeyPressed(int id);

};
#endif // GLOBALHOTKEYFILTER_H
