INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/http_parser.c \
    $$PWD/qhttpconnection.cpp \
    $$PWD/qhttprequest.cpp \
    $$PWD/qhttpresponse.cpp \
    $$PWD/qhttpserver.cpp
HEADERS += \
    $$PWD/http_parser.h \
    $$PWD/qhttpconnection.h \
    $$PWD/qhttprequest.h \
    $$PWD/qhttpresponse.h \
    $$PWD/qhttpserver.h \
    $$PWD/qhttpserverapi.h \
    $$PWD/qhttpserverfwd.h
