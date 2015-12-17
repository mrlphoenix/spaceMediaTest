#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T00:40:19
#
#-------------------------------------------------

QT       += core gui multimediawidgets xml network
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpaceMediaTest
TEMPLATE = app

include(../src/core/core.pri)
include(../src/utils/utils.pri)
include(../src/widgets/widgets.pri)


SOURCES += main.cpp \

RESOURCES +=


LIBS       += -lVLCQtCore -lVLCQtWidgets
LIBS       += -LC:/lib-vlc/lib -lVLCQtCore -lVLCQtWidgets

INCLUDEPATH += C:/lib-vlc/include
