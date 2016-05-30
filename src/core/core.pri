INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/teledscore.cpp \
    $$PWD/teledssheduler.cpp \
    $$PWD/platformspecs.cpp
HEADERS += \
    $$PWD/teledscore.h \
    $$PWD/singleton.h \
    $$PWD/call_once.h \
    $$PWD/teledssheduler.h \
    $$PWD/platformdefines.h \
    $$PWD/platformspecs.h \
    $$PWD/version.h

FORMS += \
    $$PWD/mainwindow.ui
