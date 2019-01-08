TEMPLATE += app
CONFIG += console
QT += network

RESOURCES += res.qrc

INCLUDEPATH += ../src ../src/vlcqt ../src/bittorrent
LIBS += -L../src -lsrc

include(../unixconf.pri)

SOURCES += main.cpp \
    application.cpp

HEADERS += \
    application.h
