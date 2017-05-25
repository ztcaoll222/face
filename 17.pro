#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T22:25:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32 {
INCLUDEPATH+=..\17\include\opencv
INCLUDEPATH+=..\17\include\opencv2
INCLUDEPATH+=..\17\include

LIBS += ..\17\libs\win\libopencv_*.dll.a
}

unix {
INCLUDEPATH+=../17/include/opencv
INCLUDEPATH+=../17/include/opencv2
INCLUDEPATH+=../17/include

LIBS += ../17/libs/unix/
}

TARGET = 17
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
        mainwindow.cpp \
    camerathread.cpp \
    showphotodialog.cpp \
    savephotothread.cpp \
    learndialog.cpp \
    leanrnthread.cpp

HEADERS  += \
    base.h \
    camerathread.h \
    mainwindow.h \
    showphotodialog.h \
    savephotothread.h \
    learndialog.h \
    leanrnthread.h

FORMS    += mainwindow.ui \
    showphotodialog.ui \
    learndialog.ui

RESOURCES += \
    src.qrc
