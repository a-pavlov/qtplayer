QT += testlib core

SOURCES = StorageTest.cpp \
    FlatPieceMemoryStorageTest.cpp \
    ../src/FlatPieceMemoryStorage.cpp \
    ../test/RangeTest.cpp

HEADERS += FlatPieceMemoryStorageTest.h \
    ../src/FlatPieceMemoryStorage.h \
    ../src/Range.h \
    ../test/RangeTest.h

INCLUDEPATH += ../src ../test
