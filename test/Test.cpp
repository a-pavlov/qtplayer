#include <QCoreApplication>
#include <QtTest/QtTest>

#include "TorrentInfoTest.h"
#include "RangeTest.h"

int main(int argc, char *argv[]) {
    //freopen("testing.log", "w", stdout);
    QCoreApplication app(argc, argv);
    TorrentInfoTest torrentInfoTest;
    RangeTest rangeTest;

    return QTest::qExec(&torrentInfoTest, argc, argv)            
            | QTest::qExec(&rangeTest, argc, argv);
}


