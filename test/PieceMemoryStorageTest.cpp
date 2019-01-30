#include <QtTest/QtTest>

#include "PieceMemoryStorageTest.h"
#include "PieceMemoryStorage.h"

PieceMemoryStorageTest::PieceMemoryStorageTest(QObject *parent): QObject (parent) {}

void PieceMemoryStorageTest::trivialTest() {
    PieceMemoryStorage pieceMemoryStorage(10, 10, 0, 10, 4, 100, 0);
    QPair<MemoryBlock, MemoryBlock> empty = {{nullptr, 0}, {nullptr, 0}};
    QCOMPARE(empty, pieceMemoryStorage.obtainRanges(100));
    QCOMPARE(0, pieceMemoryStorage.nextPieceMemoryIndex((0)));
    QCOMPARE(1, pieceMemoryStorage.nextPieceMemoryIndex((1)));
    QCOMPARE(2, pieceMemoryStorage.nextPieceMemoryIndex((2)));
    QCOMPARE(3, pieceMemoryStorage.nextPieceMemoryIndex((3)));
    QCOMPARE(0, pieceMemoryStorage.nextPieceMemoryIndex((4)));
}

void PieceMemoryStorageTest::testBaseScenario() {
    PieceMemoryStorage pieceMemoryStorage(1024, 1024, 0, 10, 10, 1024*10, 100);
    const QList<Piece>& rp = pieceMemoryStorage.requestedPieces();
    QCOMPARE(rp.size(), 10);
    int index = 0;
    int memIndex = 0;
    foreach(const Piece& p, rp) {
        QVERIFY(!p.isFull());
        QCOMPARE(0, p.bytesAvailable());
        QCOMPARE(index++, p.index);
        QCOMPARE(memIndex++, p.pieceMemoryIndex);
    }
}
