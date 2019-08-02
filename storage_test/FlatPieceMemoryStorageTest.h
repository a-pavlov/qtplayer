#ifndef FLATPIECEMEMORYSTORAGETEST_H
#define FLATPIECEMEMORYSTORAGETEST_H

#include <QObject>
#include <array>

class FlatPieceMemoryStorageTest : public QObject {
    Q_OBJECT
private:
    std::array<unsigned char, 256> data;
public:
    FlatPieceMemoryStorageTest(QObject *parent = nullptr);
private slots:
    void testAuxiliaryMethods1();
    void testAuxiliaryMethods2();
    void testSyncOperating();

    // check writing position moving correctly in case of unordered data writing
    void testWritingPositionExansion();
};

#endif // FLATPIECEMEMORYSTORAGETEST_H
