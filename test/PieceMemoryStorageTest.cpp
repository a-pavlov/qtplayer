#include <QtTest/QtTest>

#include "PieceMemoryStorageTest.h"
#include "PieceMemoryStorage.h"

PieceMemoryStorageTest::PieceMemoryStorageTest(QObject *parent): QObject (parent) {}

void PieceMemoryStorageTest::trivialTest() {
    PieceMemoryStorage pieceMemoryStorage(10, 4);
    QPair<Range, Range> empty = {{nullptr, 0}, {nullptr, 0}};
    QCOMPARE(empty, pieceMemoryStorage.obtainRanges(100));
    QCOMPARE(0, pieceMemoryStorage.nextPieceMemoryIndex((0)));
    QCOMPARE(1, pieceMemoryStorage.nextPieceMemoryIndex((1)));
    QCOMPARE(2, pieceMemoryStorage.nextPieceMemoryIndex((2)));
    QCOMPARE(3, pieceMemoryStorage.nextPieceMemoryIndex((3)));
    QCOMPARE(0, pieceMemoryStorage.nextPieceMemoryIndex((4)));
}
