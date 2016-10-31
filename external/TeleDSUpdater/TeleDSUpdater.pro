QT += core
QT -= gui

TARGET = TeleDSUpdater
target.path = /home/pi/
INSTALLS += target

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    notherfilesystem.cpp

HEADERS += \
    notherfilesystem.h

