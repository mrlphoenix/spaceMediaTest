INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/teledscore.cpp \
    $$PWD/teledssheduler.cpp \
    $$PWD/androidspecs.cpp
HEADERS += \
    $$PWD/teledscore.h \
    $$PWD/singleton.h \
    $$PWD/call_once.h \
    $$PWD/teledssheduler.h \
    $$PWD/platformdefines.h \
    $$PWD/androidspecs.h

FORMS += \
    $$PWD/mainwindow.ui
