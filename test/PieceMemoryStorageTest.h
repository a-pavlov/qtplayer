#ifndef PIECEMEMORYSTORAGETEST_H
#define PIECEMEMORYSTORAGETEST_H

#include <QObject>

class PieceMemoryStorageTest : public QObject {
    Q_OBJECT
public:
    PieceMemoryStorageTest(QObject *parent = nullptr);
private slots:
    void trivialTest();
};

#endif // PIECEMEMORYSTORAGETEST_H
