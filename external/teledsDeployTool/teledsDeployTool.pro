#-------------------------------------------------
#
# Project created by QtCreator 2016-10-25T12:17:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = teledsDeployTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lfsrencoder.cpp \
    notherfilesystem.cpp

HEADERS  += mainwindow.h \
    lfsrencoder.h \
    notherfilesystem.h

FORMS    += mainwindow.ui
