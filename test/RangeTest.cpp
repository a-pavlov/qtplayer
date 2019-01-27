#include "RangeTest.h"
#include <QTest>
#include <QList>

#include "Range.h"

RangeTest::RangeTest(QObject *parent) : QObject(parent) {
}

void RangeTest::testAlgorithm() {
    QList<int> list;
    QVERIFY(qLowerBound(list.begin(), list.end(), 5) == list.end());
    QVERIFY(qLowerBound(list.begin(), list.end(), 5) == list.begin());
    list << 3 << 6 << 10;
    QVERIFY(qLowerBound(list.begin(), list.end(), 5) != list.begin());
    QList<int>::iterator itr = qLowerBound(list.begin(), list.end(), 1);
    QVERIFY(itr == list.begin());
    QList<int>::iterator itr2 = qLowerBound(list.begin(), list.end(), 4);
    QVERIFY(itr2 != list.begin());
    QList<int>::iterator itr3 = qLowerBound(list.begin(), list.end(), 12);
    QVERIFY(itr3 == list.end());
}

void RangeTest::testSequentialRanges() {
    Range<qint32> range;
    range += qMakePair(10, 20);
    QCOMPARE(1, range.getSegments().size());
    range += qMakePair(20,30);
    QCOMPARE(1, range.getSegments().size());
    QCOMPARE(qMakePair(10,30), range.getSegments().at(0));
    range += qMakePair(5,10);
    QCOMPARE(1, range.getSegments().size());
    QCOMPARE(qMakePair(5,30), range.getSegments().at(0));
}

void RangeTest::testSparseRanges() {
    Range<int> range;
    range += qMakePair(5,10);
    range += qMakePair(20,25);
    QCOMPARE(2, range.getSegments().size());
    range += qMakePair(10,20);
    QCOMPARE(1, range.getSegments().size());
    QCOMPARE(qMakePair(5,25), range.getSegments().at(0));
    range += qMakePair(25, 50);
    QCOMPARE(1, range.getSegments().size());
    QCOMPARE(qMakePair(5,50), range.getSegments().at(0));
}

void RangeTest::testBackardRanges() {
    Range<int> range;
    range += qMakePair(100, 200);
    range += qMakePair(50, 100);
    QCOMPARE(1, range.getSegments().size());
    QCOMPARE(qMakePair(50,200), range.getSegments().at(0));
    range += qMakePair(20, 40);
    range += qMakePair(10, 20);
    QCOMPARE(2, range.getSegments().size());
    QCOMPARE(qMakePair(10,40), range.getSegments().at(0));
    QCOMPARE(qMakePair(50,200), range.getSegments().at(1));
    range += qMakePair(40, 45);
    QCOMPARE(2, range.getSegments().size());
    range += qMakePair(45, 50);
    QCOMPARE(1, range.getSegments().size());
    QCOMPARE(qMakePair(10,200), range.getSegments().at(0));
    range += qMakePair(0, 10);
    QCOMPARE(1, range.getSegments().size());
    QCOMPARE(qMakePair(0,200), range.getSegments().at(0));
}
