#include <QtTest/QtTest>

#include "bittorrent/torrentinfo.h"

class Test: public QObject
{
    Q_OBJECT
private slots:
    void testTorrentInfo();
};

void Test::testTorrentInfo() {
    QString error;
    BitTorrent::TorrentInfo torrentInfo = BitTorrent::TorrentInfo::loadFromFile(":/res/ubuntu-18.10-desktop-amd64.iso.torrent", &error);
    QCOMPARE(QString(), error);
    QVERIFY(torrentInfo.isValid());
}

QTEST_MAIN(Test)
#include "test.moc"
