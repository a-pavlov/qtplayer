#ifndef RANGETEST_H
#define RANGETEST_H

#include <QObject>

class RangeTest : public QObject
{
    Q_OBJECT
public:
    explicit RangeTest(QObject *parent = nullptr);

private slots:
    void testAlgorithm();
    void testSequentialRanges();
    void testSparseRanges();
    void testBackardRanges();
    void testSequentialAdd();
};

#endif // RANGETEST_H
