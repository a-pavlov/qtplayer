#include <QtTest/QtTest>
#include <QDebug>

#include "PieceMemoryStorageTest.h"
#include "PieceMemoryStorage.h"

PieceMemoryStorageTest::PieceMemoryStorageTest(QObject *parent): QObject (parent) {}

void PieceMemoryStorageTest::testInitialRequest() {
    PieceMemoryStorage pieceMemoryStorage(1024, 1024, 10, 100, 1024*10);
    pieceMemoryStorage.requestPieces();
    const QList<SPPiece>& rp = pieceMemoryStorage.requestedPieces();
    QCOMPARE(rp.size(), 10);
    int index = 0;
    foreach(const SPPiece& p, rp) {
        QVERIFY(!p->isFull());
        QCOMPARE(0, p->bytesAvailable());
        QCOMPARE(p->index, index++);
    }
}

void PieceMemoryStorageTest::testMemory() {
    constexpr auto pieceLength = 128;
    constexpr auto lastPieceLength = 96;
    constexpr auto firstPiece = 3;
    constexpr auto lastPiece = 7;
    constexpr auto maxPieces = 3;
    constexpr auto fileOffset = 1ll * firstPiece * pieceLength + 47;
    constexpr auto fileSize = 1ll*(lastPiece - firstPiece)*pieceLength + 15;
    PieceMemoryStorage pieceMemoryStorage(pieceLength
                                              , lastPieceLength
                                              , maxPieces
                                              , fileOffset
                                              , fileSize);
    SPMem m1 = pieceMemoryStorage.allocate();
    QCOMPARE(pieceLength, m1->size());
    SPMem m2 = pieceMemoryStorage.allocate();
    QCOMPARE(pieceLength, m2->size());
    pieceMemoryStorage.release(m1);
    SPMem m3 = pieceMemoryStorage.allocate();
    QCOMPARE(m1.get(), m3.get());
    pieceMemoryStorage.release(m3);
    pieceMemoryStorage.release(m2);
    SPMem m4 = pieceMemoryStorage.allocate();
    QCOMPARE(m1.get(), m4.get());
}

void PieceMemoryStorageTest::testBaseScenario() {
    //<! |    |    |    |    |    |    |    |   |
    //<!  0    1    2    3    4    5    6    7
    //!<                       <==============>
    constexpr auto pieceLength = 128;
    constexpr auto lastPieceLength = 112;
    constexpr auto firstPiece = 4;
    constexpr auto lastPiece = 7;
    constexpr auto maxPieces = 2;
    constexpr auto fileOffset = 1ll * firstPiece * pieceLength + 47;
    constexpr auto fileSize = 1ll*(lastPiece - firstPiece)*pieceLength + 15;
    PieceMemoryStorage pieceMemoryStorage(pieceLength
                                              , lastPieceLength
                                              , maxPieces
                                              , fileOffset
                                              , fileSize);


    pieceMemoryStorage.requestPieces();

    QCOMPARE(pieceMemoryStorage.absoluteReadingPosition(), fileOffset);
    QCOMPARE(pieceMemoryStorage.readingPiece(), firstPiece);

    QCOMPARE(pieceMemoryStorage.firstPiece(), 4);
    QCOMPARE(pieceMemoryStorage.lastPiece(), 7);
    QCOMPARE(pieceMemoryStorage.getPieceLength(4), pieceLength);
    QCOMPARE(pieceMemoryStorage.getPieceLength(5), pieceLength);
    QCOMPARE(pieceMemoryStorage.getPieceLength(6), pieceLength);
    QCOMPARE(pieceMemoryStorage.getPieceLength(7), lastPieceLength);

    const QList<SPPiece>& rp = pieceMemoryStorage.requestedPieces();
    QCOMPARE(rp.size(), maxPieces);
    for(const SPPiece& p: rp) {
        QCOMPARE(p->bytesAvailable(), 0);
    }

    QCOMPARE(firstPiece, rp.at(0)->index);
    QCOMPARE(firstPiece + 1, rp.at(1)->index);


    const unsigned char data[2] = {'\x01', '\x02'};

    // do nothing here - we do not request piece 0
    for(int i = 0; i < pieceLength/2; ++i) {
        pieceMemoryStorage.write(data, 2, i*2, 0);
    }

    for(int i = 0; i < pieceLength/2; ++i) {
        pieceMemoryStorage.write(data, 2, i*2, firstPiece);        
        const QList<SPPiece>& requestedPieces = pieceMemoryStorage.requestedPieces();
        //qDebug() << "requested piece size " << requestedPieces.size();
        //qDebug() << "bytes available " << requestedPieces.at(0)->bytesAvailable();
        QCOMPARE(requestedPieces.size(), 2);
        QCOMPARE(requestedPieces.at(0)->bytesAvailable(), (i+1)*2);
        QCOMPARE(requestedPieces.at(1)->bytesAvailable(), 0);
    }

    QVERIFY(pieceMemoryStorage.requestedPieces().at(0)->isFull());
    QVERIFY(!pieceMemoryStorage.requestedPieces().at(1)->isFull());
}
