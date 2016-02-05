#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T00:40:19
#
#-------------------------------------------------

QT       +=  qml quick widgets multimedia core gui xml network sql
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = app

include(../src/core/core.pri)
include(../src/utils/utils.pri)
include(../src/widgets/widgets.pri)

SOURCES += main.cpp \

RESOURCES += qml.qrc

QML_IMPORT_PATH =

include(deployment.pri)
