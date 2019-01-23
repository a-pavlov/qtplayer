#ifndef TORRENTINFOTEST_H
#define TORRENTINFOTEST_H

#include <QObject>

class TorrentInfoTest : public QObject {
    Q_OBJECT
public:
    explicit TorrentInfoTest(QObject *parent = nullptr);
private slots:
    void testTorrentInfoFromFile();
};

#endif // TORRENTINFOTEST_H
