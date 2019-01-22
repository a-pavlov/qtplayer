#ifndef PIECEMEMORYSTORAGE_H
#define PIECEMEMORYSTORAGE_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QList>
#include <QPair>

struct Piece {
    int index;
    int capacity;
    int size;
    unsigned char* ptr;
    bool isFull() const {
        return capacity == size;
    }
};

typedef QPair<unsigned char*, int> Range;

class PieceMemoryStorage {
private:
    QMutex mutex;
    QWaitCondition bufferHasData;
    QList<Piece> pieces;
    int pieceSize;
    int maxPieces;
    quint64 fileSize;
    quint64 fileOffset;
    quint64 readingCursorPosition;

    bool isFirstMemoryBlock(const Piece& piece) const {
        return false;
    }

public:
    PieceMemoryStorage(int pieceSize, int maxPieces);
    int read(unsigned char* buf, size_t len);
    void write(unsigned char* buf, size_t len);
    int seek(quint64 pos);

    /**
     * @brief obtainRanges obtain memory ranges to copy into output buffer base on current reading position
     * @param len in bytes
     * @return one or two memory ranges two ranges in case of no-continuous memory blocks
     */
    QPair<Range, Range> obtainRanges(size_t len);
};

#endif // PIECEMEMORYSTORAGE_H
