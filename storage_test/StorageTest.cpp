#include <QCoreApplication>
#include <QtTest/QtTest>

#include "FlatPieceMemoryStorageTest.h"
#include "RangeTest.h"

int main(int argc, char *argv[]) {
    //freopen("testing.log", "w", stdout);
    QCoreApplication app(argc, argv);
    FlatPieceMemoryStorageTest flatPieceMemoryStorageTest;
    RangeTest rangeTest;
    return QTest::qExec(&flatPieceMemoryStorageTest, argc, argv) | QTest::qExec(&rangeTest, argc, argv) ;
}
