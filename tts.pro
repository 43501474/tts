#-------------------------------------------------
#
# Project created by QtCreator 2016-01-27T21:55:37
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += openssl-linked

TARGET = tts
TEMPLATE = app

HEADERS += $$PWD/mainwindow.h \
    $$PWD/ITTS.h \
    $$PWD/XunFeiTTS.h \
    $$PWD/BaiduTTS.h

SOURCES += $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/XunFeiTTS.cpp \
    $$PWD/BaiduTTS.cpp

FORMS    += mainwindow.ui

LIBS += $$PWD/libs/msc_x64.lib
LIBS += $$PWD/vld/lib/Win64/vld.lib

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

INCLUDEPATH += $$PWD/vld/include

DISTFILES += \
    Makefile
