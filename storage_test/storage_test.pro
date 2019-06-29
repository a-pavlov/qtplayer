QT += testlib

SOURCES = StorageTest.cpp \
    FlatPieceMemoryStorageTest.cpp \
    ../src/FlatPieceMemoryStorage.cpp

HEADERS += FlatPieceMemoryStorageTest.h \
    ../src/FlatPieceMemoryStorage.h

INCLUDEPATH += ../src
