QT += testlib
SOURCES = Test.cpp \
    TorrentInfoTest.cpp \
    RangeTest.cpp
RESOURCES += res.qrc

INCLUDEPATH += ../src
LIBS += -L../src -lsrc

include(../unixconf.pri)

HEADERS += \
    TorrentInfoTest.h \
    RangeTest.h
