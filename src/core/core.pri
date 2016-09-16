INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/teledscore.cpp \
    $$PWD/teledssheduler.cpp \
    $$PWD/platformspecific.cpp \
    $$PWD/statictext.cpp \
    $$PWD/systeminfoprovider.cpp
HEADERS += \
    $$PWD/teledscore.h \
    $$PWD/singleton.h \
    $$PWD/call_once.h \
    $$PWD/teledssheduler.h \
    $$PWD/platformdefines.h \
    $$PWD/version.h \
    $$PWD/platformspecific.h \
    $$PWD/statictext.h \
    $$PWD/systeminfoprovider.h

FORMS += \
    $$PWD/mainwindow.ui
