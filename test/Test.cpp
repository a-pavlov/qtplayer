#include <QCoreApplication>
#include <QtTest/QtTest>

#include "TorrentInfoTest.h"
#include "PieceMemoryStorageTest.h"
#include "RangeTest.h"
#include "PieceTest.h"

int main(int argc, char *argv[]) {
    //freopen("testing.log", "w", stdout);
    QCoreApplication app(argc, argv);
    TorrentInfoTest torrentInfoTest;
    PieceMemoryStorageTest pieceMemoryStorageTest;
    RangeTest rangeTest;
    PieceTest pieceTest;

    return QTest::qExec(&torrentInfoTest, argc, argv)
            | QTest::qExec(&pieceMemoryStorageTest, argc, argv)
            | QTest::qExec(&pieceTest, argc, argv)
            | QTest::qExec(&rangeTest, argc, argv);
}


