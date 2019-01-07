TEMPLATE += app
CONFIG += console

INCLUDEPATH += ../src ../src/vlcqt ../src/bittorrent
LIBS += -L../src -lsrc

include(../unixconf.pri)

SOURCES += main.cpp
