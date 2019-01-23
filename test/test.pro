QT += testlib
SOURCES = Test.cpp \
    PieceMemoryStorageTest.cpp \
    TorrentInfoTest.cpp
RESOURCES += res.qrc

INCLUDEPATH += ../src
LIBS += -L../src -lsrc

include(../unixconf.pri)

HEADERS += \
    PieceMemoryStorageTest.h \
    TorrentInfoTest.h
