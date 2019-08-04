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
    constexpr auto maxPieces = 3;
    constexpr auto fileOffset = 8ll;
    constexpr auto fileSize = 29ll;

    FlatPieceMemoryStorage pms(pieceLength
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

    QCOMPARE(pms.getPieceLength(), pieceLength);
    QCOMPARE(pms.getPieceLength(), pieceLength);

    constexpr auto lastPieceInTorrent = 3;

    // one byte file in the last piece of torrent
    FlatPieceMemoryStorage pmsLast(pieceLength
        , maxPieces
        , 8ll*pieceLength + 1
        , 1);
    QCOMPARE(pmsLast.firstPiece(), 8);
    QCOMPARE(pmsLast.lastPiece(), 8);
    QCOMPARE(pmsLast.firstPieceOffset(), 8ll*pieceLength);
    QCOMPARE(pmsLast.getPieceLength(), pieceLength);
    QCOMPARE(pmsLast.posInCacheByAbsPos(8ll*pieceLength), 0);
    QCOMPARE(pmsLast.posInCacheByAbsPos(8ll*pieceLength + 1), 1);
}

void FlatPieceMemoryStorageTest::testAuxiliaryMethods2() {

}

void FlatPieceMemoryStorageTest::testSyncOperating() {
    constexpr auto pieceLength = 10;
    constexpr auto maxPieces = 3;
    constexpr auto fileOffset = 12ll;
    constexpr auto fileSize = 50ll;

    FlatPieceMemoryStorage pms(pieceLength
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

    QCOMPARE(pms.bytesInLastPiece(), 2);

    QCOMPARE(pms.posInCacheByAbsPos(fileOffset), 2);
    pms.requestSlots(pms.firstPiece());
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

    QCOMPARE(3, rp.size());

    // first piece writing
    pms.write(&data[10], 1, 0, 1);
    QCOMPARE(pms.absoluteWritingPosition(), 11ll);
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
    QCOMPARE(3, rp.size());

    QCOMPARE(rp.at(0), 2);
    QCOMPARE(rp.at(1), 3);
    QCOMPARE(rp.at(2), 4);

    pms.write(&data[30], 1, 0, 3);
    QCOMPARE(pms.absoluteWritingPosition(), 31ll);
    pms.write(&data[31], 8, 1, 3);
    QCOMPARE(pms.absoluteWritingPosition(), 39ll);

    std::array<unsigned char, 12> rb2;
    pms.read(&rb2[0], 12);
    QCOMPARE(pms.absoluteReadingPosition(), 34ll);
    QCOMPARE(memcmp(&rb2[0], &data[22], 12), 0);
}

void FlatPieceMemoryStorageTest::testWritingPositionExansion() {
    constexpr auto pieceLength = 10;
    constexpr auto maxPieces = 4;
    constexpr auto fileOffset = 6ll;
    constexpr auto fileSize = 38ll;

    FlatPieceMemoryStorage pms(pieceLength
        , maxPieces
        , fileOffset
        , fileSize);

    QList<int> rp;
    QList<int> oorp;

    connect(&pms, &FlatPieceMemoryStorage::piecesRequested, [&](QList<int> pieces) {
        rp = pieces;
        });

    QCOMPARE(pms.bytesInLastPiece(), 4);
    QCOMPARE(pms.posInCacheByAbsPos(fileOffset), 6);
    pms.requestSlots(pms.firstPiece());
    QCOMPARE(rp.size(), 4);
    QCOMPARE(rp.at(0), 0);
    QCOMPARE(rp.at(1), 1);
    QCOMPARE(rp.at(2), 2);
    QCOMPARE(rp.at(3), 3);

    QCOMPARE(pms.absoluteWritingPosition(), 0ll);
    QCOMPARE(pms.absoluteReadingPosition(), 6ll);
    pms.write(&data[0], 8, 0, 0);
    QCOMPARE(pms.absoluteWritingPosition(), 8ll);
    QCOMPARE(pms.absoluteReadingPosition(), 6ll);

    pms.write(&data[0], 8, 0, 1);
    QCOMPARE(pms.absoluteWritingPosition(), 8ll);
    pms.write(&data[0], 2, 0, 2);
    QCOMPARE(pms.absoluteWritingPosition(), 8ll);
    pms.write(&data[0], 7, 0, 3);
    QCOMPARE(pms.absoluteWritingPosition(), 8ll);

    // piece: 0  1  2  3
    // bytes: 8  8  2  7
    pms.write(&data[0], 8, 2, 2);
    QCOMPARE(pms.absoluteWritingPosition(), 8ll);

    // piece: 0  1  2   3
    // bytes: 8  8  10  7
    pms.write(&data[0], 2, 8, 0);
    // piece: 0  1  2   3
    // bytes: 10 8  10  7
    QCOMPARE(pms.absoluteWritingPosition(), 18ll);
    pms.write(&data[0], 2, 8, 1);

    // piece: 0  1  2  3
    // bytes: 10 10 10 7
    QCOMPARE(pms.absoluteWritingPosition(), 37ll);
}

void FlatPieceMemoryStorageTest::testSlotsReq() {
    constexpr auto pieceLength = 10;
    constexpr auto maxPieces = 3;
    constexpr auto fileOffset = 0ll;
    constexpr auto fileSize = 383ll;

    FlatPieceMemoryStorage pms(pieceLength
        , maxPieces
        , fileOffset
        , fileSize);

    // simple request
    QVERIFY(pms.getSlots().isEmpty());
    pms.requestSlots(3);
    QCOMPARE(pms.getSlots().size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(pms.getSlots().at(i).first, 3 + i);
    }

    // gap less than size of cache
    pms.requestSlots(1);
    QCOMPARE(pms.getSlots().size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(pms.getSlots().at(i).first, 1 + i);
    }

    // totally remove all slots
    pms.requestSlots(10);
    QCOMPARE(pms.getSlots().size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(pms.getSlots().at(i).first, 10 + i);
    }

    // gap greater than cache size
    pms.requestSlots(5);
    QCOMPARE(pms.getSlots().size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(pms.getSlots().at(i).first, 5 + i);
    }

    // gap equals cache size
    pms.requestSlots(2);
    QCOMPARE(pms.getSlots().size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(pms.getSlots().at(i).first, 2 + i);
    }

    // request the same pieces again
    pms.requestSlots(2);
    QCOMPARE(pms.getSlots().size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(pms.getSlots().at(i).first, 2 + i);
    }

    // remove head of slots
    pms.requestSlots(3);
    QCOMPARE(pms.getSlots().size(), 3);
    for (int i = 0; i < 3; ++i) {
        QCOMPARE(pms.getSlots().at(i).first, 3 + i);
    }
}
