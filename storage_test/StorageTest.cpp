#include <QCoreApplication>
#include <QtTest/QtTest>

#include "FlatPieceMemoryStorageTest.h"

int main(int argc, char *argv[]) {
    //freopen("testing.log", "w", stdout);
    QCoreApplication app(argc, argv);
    FlatPieceMemoryStorageTest flatPieceMemoryStorageTest;
    return QTest::qExec(&flatPieceMemoryStorageTest, argc, argv);
}
