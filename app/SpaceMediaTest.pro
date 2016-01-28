#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T00:40:19
#
#-------------------------------------------------

QT       += core gui multimediawidgets xml network quick
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpaceMediaTest
target.path = /home/pi/
INSTALLS += target
TEMPLATE = app

include(../src/core/core.pri)
include(../src/utils/utils.pri)
include(../src/widgets/widgets.pri)

OTHER_FILES += ../qml/simple_player.qml

SOURCES += main.cpp \

RESOURCES += \
    qml.qrc

DISTFILES += ../qml/simple_player.qml
