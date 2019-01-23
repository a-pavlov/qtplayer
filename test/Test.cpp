#include <QCoreApplication>
#include <QtTest/QtTest>

#include "TorrentInfoTest.h"
#include "PieceMemoryStorageTest.h"

int main(int argc, char *argv[]) {
    freopen("testing.log", "w", stdout);
    QCoreApplication app(argc, argv);
    TorrentInfoTest torrentInfoTest;
    PieceMemoryStorageTest pieceMemoryStorageTest;

    return QTest::qExec(&torrentInfoTest, argc, argv) | QTest::qExec(&pieceMemoryStorageTest, argc, argv);
}


