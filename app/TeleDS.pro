#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T00:40:19
#
#-------------------------------------------------

QT       += qml quick widgets multimedia core gui xml network sql positioning svg
CONFIG   += c++11

PKGCONFIG += openssl

KIT = Android
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = app

contains (KIT, Raspberry){
    TARGET = SpaceMediaTest
    target.path = /home/pi/
    INSTALLS += target
}

include(../src/core/core.pri)
include(../src/utils/utils.pri)
include(../src/widgets/widgets.pri)

contains(KIT, Android){
    include(../src/zip/zip.pri)
}

OTHER_FILES += ../qml/simple_player.qml \
           VideoPlayer.qml

SOURCES += main.cpp \

RESOURCES += \
    qml.qrc

DISTFILES += android/AndroidManifest.xml
QML_IMPORT_PATH =

LIBS += -lssl -lcrypto

contains (KIT, Android) {
    include(deployment.pri)
    QT += androidextras
}

android: {
    INCLUDEPATH += /usr/local/ssl/android-9/include
    LIBS += -L/usr/local/ssl/android-9/lib
}
ANDROID_PERMISSIONS = android.permission.READ_PHONE_STATE


    ANDROID_EXTRA_LIBS = \
        $$PWD/../../../../usr/local/ssl/android-9/lib/libcrypto_1_0_0.so \
        $$PWD/../../../../usr/local/ssl/android-9/lib/libssl_1_0_0.so

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


