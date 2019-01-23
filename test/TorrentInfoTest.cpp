#include <QtTest/QtTest>
#include "TorrentInfoTest.h"
#include "bittorrent/torrentinfo.h"

TorrentInfoTest::TorrentInfoTest(QObject *parent) : QObject(parent) {}

void TorrentInfoTest::testTorrentInfoFromFile() {
    QString error;
    BitTorrent::TorrentInfo torrentInfo = BitTorrent::TorrentInfo::loadFromFile(":/res/ubuntu-18.10-desktop-amd64.iso.torrent", &error);
    QCOMPARE(QString(), error);
    QVERIFY(torrentInfo.isValid());
}
