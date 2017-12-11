#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T00:40:19
#
#-------------------------------------------------

QT       += qml quick widgets core gui xml network sql positioning svg
#QT       += widgets core gui xml network sql
CONFIG   += c++11

PKGCONFIG += openssl

KIT = Android
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = app

contains (KIT, Raspberry){
    TARGET = SpaceMediaTest
    target.path = /home/pi/teleds
    INSTALLS += target
    INCLUDEPATH += /home/nother/raspi/sysroot/usr/local/include
    LIBS += -L/home/nother/raspi/sysroot/usr/local/lib -lwiringPi
}
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

include(../src/core/core.pri)
include(../src/utils/utils.pri)
include(../src/widgets/widgets.pri)
include(../src/httpserver/httpserver.pri)

contains(KIT, Android){
    include(../src/zip/zip.pri)
}

OTHER_FILES += VideoPlayer.qml \
               main_player.qml \
               AndroidBrowser.qml \
               SideBrowser.qml \
               PlayerView.qml \
               SpaceMediaMenu.qml \
               android/src/org/qtproject/qt5/android/bindings/QtActivity.java

SOURCES += main.cpp

RESOURCES += \
    qml.qrc

DISTFILES += android/AndroidManifest.xml \
    android/src/org/qtproject/qt5/android/bindings/KioskService.java \
    TeleDSMenu.qml \
    PlayerView.qml \
    SpaceMediaMenu.qml \
    android/src/org/qtproject/qt5/android/bindings/yourActivityRunOnStartup.java
QML_IMPORT_PATH =


contains (KIT, Android) {
    include(deployment.pri)
    QT += androidextras
}

android: {
    INCLUDEPATH += /home/nother/openssl101t/include
    LIBS += -L/home/nother/openssl101t/
    LIBS += -lssl -lcrypto
}
ANDROID_PERMISSIONS = android.permission.READ_PHONE_STATE

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

HEADERS +=

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = /home/nother/openssl101t/libssl.so /home/nother/TeleDS/app/../../openssl101t/libcrypto.so
}


