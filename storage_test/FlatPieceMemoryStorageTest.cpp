#include <QtTest/QtTest>
#include <QDebug>

#include "FlatPieceMemoryStorageTest.h"
#include "FlatPieceMemoryStorage.h"

FlatPieceMemoryStorageTest::FlatPieceMemoryStorageTest(QObject *parent): QObject (parent) {
    for(auto i = data.size() - data.size(); i < data.size(); ++i) {
        data[i] = static_cast<unsigned char>(i);
    }
}


/*
0       1       2       3       4       5       6       7       8
+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +---+
|     | |     | |     | |     | |     | |     | |     | |     | |   |
+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +---+
           +-----------------------------------------------+
        8  |XX|-|XXXXX|-|XXXXX|-|XXXXX|-|XXXXX|-|XXXXX|-|XX| 29
           +-----------------------------------------------+

           +-----------------------------------------------+
           file length  29 bytes
           offset       8 bytes
           piece size   5 bytes
           total pieces 9
           last piece   3 bytes
*/

void FlatPieceMemoryStorageTest::testAuxiliaryMethods1() {
    constexpr auto pieceLength = 5;
    constexpr auto lastPieceLength = 5; // in file
    constexpr auto maxPieces = 3;
    constexpr auto fileOffset = 8ll;
    constexpr auto fileSize = 29ll;

    FlatPieceMemoryStorage pms(pieceLength
        , lastPieceLength
        , maxPieces
        , fileOffset
        , fileSize);

    QCOMPARE(pms.firstPiece(), 1);
    QCOMPARE(pms.lastPiece(), 7);
    QCOMPARE(pms.cacheSize(), 15);
    QCOMPARE(pms.firstPieceOffset(), 5ll);

    QCOMPARE(pms.posInCacheByAbsPos(9), 4);
    QCOMPARE(pms.posInCacheByAbsPos(12), 7);
    QCOMPARE(pms.posInCacheByAbsPos(20), 0);
    QCOMPARE(pms.posInCacheByAbsPos(26), 6);
    QCOMPARE(pms.posInCacheByAbsPos(34), 14);

    QCOMPARE(pms.posInCacheByPiece(1), 0);
    QCOMPARE(pms.posInCacheByPiece(3), 10);
    QCOMPARE(pms.posInCacheByPiece(4), 0);
    QCOMPARE(pms.posInCacheByPiece(6), 10);

    QCOMPARE(pms.getPieceLength(6), pieceLength);
    QCOMPARE(pms.getPieceLength(7), pieceLength);

    constexpr auto lastPieceInTorrent = 3;

    // one byte file in the last piece of torrent
    FlatPieceMemoryStorage pmsLast(pieceLength
        , lastPieceInTorrent
        , maxPieces
        , 8ll*pieceLength + 1
        , 1);
    QCOMPARE(pmsLast.firstPiece(), 8);
    QCOMPARE(pmsLast.lastPiece(), 8);
    QCOMPARE(pmsLast.firstPieceOffset(), 8ll*pieceLength);
    QCOMPARE(pmsLast.getPieceLength(8), lastPieceInTorrent);
    QCOMPARE(pmsLast.posInCacheByAbsPos(8ll*pieceLength), 0);
    QCOMPARE(pmsLast.posInCacheByAbsPos(8ll*pieceLength + 1), 1);
}

void FlatPieceMemoryStorageTest::testAuxiliaryMethods2() {

}

void FlatPieceMemoryStorageTest::testSyncOperating() {
    constexpr auto pieceLength = 10;
    constexpr auto lastPieceLength = 5; // in file
    constexpr auto maxPieces = 3;
    constexpr auto fileOffset = 12ll;
    constexpr auto fileSize = 50ll;

    FlatPieceMemoryStorage pms(pieceLength
        , lastPieceLength
        , maxPieces
        , fileOffset
        , fileSize);

    QList<int> rp;
    QList<int> oorp;

    connect(&pms, &FlatPieceMemoryStorage::piecesRequested,[&](QList<int> pieces) {
        rp = pieces;
    });

    connect(&pms, &FlatPieceMemoryStorage::pieceOutOfRangeReceived, [&](int length, int offset, int pieceIndex) {
        Q_UNUSED(length);
        Q_UNUSED(offset);
        oorp.append(pieceIndex);
    });

    QCOMPARE(pms.posInCacheByAbsPos(fileOffset), 2);
    pms.requestPieces();
    QCOMPARE(rp.size(), 3);
    QCOMPARE(rp.at(0), 1);
    QCOMPARE(rp.at(1), 2);
    QCOMPARE(rp.at(2), 3);
    pms.requestPieces();
    QCOMPARE(rp.size(), 3);
    QCOMPARE(rp.at(0), 1);
    QCOMPARE(rp.at(1), 2);
    QCOMPARE(rp.at(2), 3);

    // do nothing since piece 0 not in range
    pms.write(&data[0], 4, 0, 0);
    pms.write(&data[4], 6, 4, 0);
    QCOMPARE(oorp.size(), 2);
    QCOMPARE(oorp.at(0), 0);
    QCOMPARE(oorp.at(1), 0);

    pms.requestPieces();
    QCOMPARE(3, rp.size());

    // first piece writing
    pms.write(&data[10], 1, 0, 1);
    QCOMPARE(pms.absoluteWritingPosition(), 12ll);
    pms.write(&data[11], 9, 1, 1);
    QCOMPARE(pms.absoluteWritingPosition(), 20ll);
    QCOMPARE(pms.absoluteReadingPosition(), 12ll);

    // write to the same place again - nothing happens
    pms.write(&data[11], 9, 1, 1);
    QCOMPARE(pms.absoluteWritingPosition(), 20ll);
    QCOMPARE(pms.absoluteReadingPosition(), 12ll);

    // full piece write
    pms.write(&data[20], 10, 0, 2);
    QCOMPARE(pms.absoluteWritingPosition(), 30ll);
    QCOMPARE(pms.absoluteReadingPosition(), 12ll);

    std::array<unsigned char, 5> rbuff;
    pms.read(&rbuff[0], 5);

    QCOMPARE(pms.absoluteWritingPosition(), 30ll);
    QCOMPARE(pms.absoluteReadingPosition(), 17ll);
    QCOMPARE(memcmp(&rbuff[0], &data[12], 5), 0);

    // cache status the same - no free pieces
    QCOMPARE(3, rp.size());
    QCOMPARE(1, rp.at(0));
    QCOMPARE(2, rp.at(1));
    QCOMPARE(3, rp.at(2));

    pms.read(&rbuff[0], 5);
    QCOMPARE(memcmp(&rbuff[0], &data[17], 5), 0);
    QCOMPARE(pms.absoluteReadingPosition(), 22ll);
    QCOMPARE(pms.absoluteWritingPosition(), 30ll);
    pms.requestPieces();
    QCOMPARE(3, rp.size());

    QCOMPARE(2, rp.at(0));
    QCOMPARE(3, rp.at(1));
    QCOMPARE(4, rp.at(2));

    pms.write(&data[30], 1, 0, 3);
    QCOMPARE(pms.absoluteWritingPosition(), 31ll);
    pms.write(&data[31], 8, 1, 3);
    QCOMPARE(pms.absoluteWritingPosition(), 39ll);

    std::array<unsigned char, 12> rb2;
    pms.read(&rb2[0], 12);
    QCOMPARE(pms.absoluteReadingPosition(), 34ll);
    QCOMPARE(memcmp(&rb2[0], &data[22], 12), 0);
}
