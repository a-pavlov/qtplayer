#include "PieceTest.h"

#include <QTest>

#include "PieceMemoryStorage.h"

PieceTest::PieceTest(QObject *parent) : QObject(parent)
{

}

void PieceTest::testPieceMethods() {
    PieceMemoryStorage pms(0,0,0,0,0);
    SPMem mem;
    Piece piece(0, 100, pms, mem);
    piece.range += qMakePair(30, 40);
    QVERIFY(!piece.isFull());
    QCOMPARE(0, piece.bytesAvailable());
    piece.range += qMakePair(20, 30);
    QVERIFY(!piece.isFull());
    QCOMPARE(0, piece.bytesAvailable());
    piece.range += qMakePair(0, 10);
    QVERIFY(!piece.isFull());
    QCOMPARE(10, piece.bytesAvailable());
    piece.range += qMakePair(10, 20);
    QVERIFY(!piece.isFull());
    QCOMPARE(40, piece.bytesAvailable());
    piece.range += qMakePair(40, 100);
    QVERIFY(piece.isFull());
    QCOMPARE(100, piece.bytesAvailable());
}
