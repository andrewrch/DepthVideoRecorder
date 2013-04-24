#-------------------------------------------------
#
# Project created by QtCreator 2013-04-23T11:01:00
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DepthVideoRecorder
TEMPLATE = app


SOURCES += main.cpp\
        depthvideorecorder.cpp \
    qcvmatwidget.cpp

HEADERS  += depthvideorecorder.h \
    qcvmatwidget.h \
    videostream.h

FORMS    += depthvideorecorder.ui \
    qcvmatwidget.ui

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv
