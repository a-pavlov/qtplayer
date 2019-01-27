#ifndef PIECETEST_H
#define PIECETEST_H

#include <QObject>

class PieceTest : public QObject
{
    Q_OBJECT
public:
    explicit PieceTest(QObject *parent = nullptr);

private slots:
    void testPieceMethods();
};

#endif // PIECETEST_H
