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

void PieceMemoryStorageTest::testInitialRequest() {
    PieceMemoryStorage pieceMemoryStorage(1024, 1024, 0, 10, 10, 1024*10, 100);
    const QList<Piece>& rp = pieceMemoryStorage.requestedPieces();
    QCOMPARE(rp.size(), 10);
    int index = 0;
    int memIndex = 0;
    foreach(const Piece& p, rp) {
        QVERIFY(!p.isFull());
        QCOMPARE(0, p.bytesAvailable());
        QCOMPARE(p.index, index++);
        QCOMPARE(p.pieceMemoryIndex, memIndex++);
    }
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
                                              , firstPiece
                                              , lastPiece
                                              , maxPieces
                                              , fileOffset
                                              , fileSize);

    const QList<Piece>& rp = pieceMemoryStorage.requestedPieces();
    QCOMPARE(rp.size(), maxPieces);
    for(const Piece& p: rp) {
        QCOMPARE(p.bytesAvailable(), 0);
    }

    QCOMPARE(firstPiece, rp.at(0).index);
    QCOMPARE(firstPiece + 1, rp.at(1).index);

    const unsigned char data[2] = {'\x01', '\x02'};

    // do nothing here - we do not request piece 0
    for(int i = 0; i < pieceLength/2; ++i) {
        pieceMemoryStorage.write(data, 2, i*2, 0);
    }

    for(int i = 0; i < pieceLength/2; ++i) {
        pieceMemoryStorage.write(data, 2, i*2, firstPiece);
        const QList<Piece>& requestedPieces = pieceMemoryStorage.requestedPieces();
        QCOMPARE(requestedPieces.size(), 2);
        QCOMPARE(requestedPieces.at(0).bytesAvailable(), (i+1)*2);
        QCOMPARE(requestedPieces.at(1).bytesAvailable(), 0);
    }

    QVERIFY(pieceMemoryStorage.requestedPieces().at(0).isFull());


}
