#-------------------------------------------------
#
# Project created by QtCreator 2024-12-27T19:26:40
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WuwaBackend
TEMPLATE = app

DEFINES += QT_MESSAGELOGCONTEXT
#CONFIG += release
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -g


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

# moc obj不重要的文件放到指定路径
win32:CONFIG(release, debug|release): OBJECTS_DIR = $$PWD/obj/release
else:win32:CONFIG(debug, debug|release): OBJECTS_DIR = $$PWD/obj/debug

win32:CONFIG(release, debug|release): MOC_DIR = $$PWD/moc/release
else:win32:CONFIG(debug, debug|release): MOC_DIR = $$PWD/moc/debug

# opencv 库
INCLUDEPATH += $$PWD/3rd/opencv/include
LIBS += -L$$PWD/3rd/opencv/x64/mingw/lib/ -llibopencv_core410
LIBS += -L$$PWD/3rd/opencv/x64/mingw/lib/ -llibopencv_imgcodecs410
LIBS += -L$$PWD/3rd/opencv/x64/mingw/lib/ -llibopencv_imgproc410
LIBS += -L$$PWD/3rd/opencv/x64/mingw/lib/ -llibopencv_highgui410

#LIBS += -L$$PWD/3rd/ -lPsapi
# 链接 Windows GDI 和用户库
LIBS += -lgdi32 -luser32 -lPsapi

win32-g++ {
    QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    commonTools/customdelegate.cpp \
    commonTools/debugmessagehandler.cpp \
    generalpanel.cpp \
    utils.cpp \
    globalhotkeyfilter.cpp \
    speicalbosswidget.cpp \
    settingparams.cpp \
    debugform.cpp \
    echolockentriespanel.cpp \
    abstractentrypanel.cpp \
    entrypanel.cpp \
    autochangewallpaperbackendworker.cpp \
    welcomedialog.cpp \
    mainbackendworkernew.cpp \
    normalbosspanel.cpp \
    fightbackendworkernew.cpp \
    commonTools/encrypttools.cpp \
    debugbackendworker.cpp \
    wuwawatcher.cpp \
    towerofadversitywidget.cpp \
    towerBattle/towerbattledatamanager.cpp \
    towerBattle/imagecapturer.cpp \
    towerBattle/imagecapturermonitor.cpp \
    towerBattle/teamidxrecognitor.cpp \
    towerBattle/teamidxrecognitionmonitor.cpp \
    towerBattle/resonancecircuitjudger.cpp \
    towerBattle/resonancecircuitjudgemonitor.cpp

HEADERS += \
        mainwindow.h \
    commonTools/customdelegate.h \
    commonTools/debugmessagehandler.h \
    generalpanel.h \
    utils.h \
    mainbackendworker.h \
    globalhotkeyfilter.h \
    speicalbosswidget.h \
    settingparams.h \
    debugform.h \
    echolockentriespanel.h \
    abstractentrypanel.h \
    entrypanel.h \
    autochangewallpaperbackendworker.h \
    welcomedialog.h \
    wuwahwndwatcher.h \
    mainbackendworkernew.h \
    normalbosspanel.h \
    fightbackendworkernew.h \
    internettimefetcher.h \
    commonTools/encrypttools.h \
    debugbackendworker.h \
    wuwawatcher.h \
    towerofadversitywidget.h \
    towerBattle/towerbattledatamanager.h \
    towerBattle/imagecapturer.h \
    towerBattle/imagecapturermonitor.h \
    towerBattle/teamidxrecognitor.h \
    towerBattle/teamidxrecognitionmonitor.h \
    towerBattle/resonancecircuitjudger.h \
    towerBattle/resonancecircuitjudgemonitor.h

FORMS += \
        mainwindow.ui \
    generalpanel.ui \
    speicalbosswidget.ui \
    debugform.ui \
    echolockentriespanel.ui \
    abstractentrypanel.ui \
    entrypanel.ui \
    welcomedialog.ui \
    normalbosspanel.ui \
    towerofadversitywidget.ui \
    towerofadversitywidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
