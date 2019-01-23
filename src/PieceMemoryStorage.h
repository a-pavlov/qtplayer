#ifndef PIECEMEMORYSTORAGE_H
#define PIECEMEMORYSTORAGE_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QList>
#include <QPair>

#include <vector>

struct Piece {
    qint32 index;
    qint32 capacity;
    qint32 size;
    int pieceMemoryIndex;

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
    qint32 pieceSize;
    qint32 maxPieces;
    quint64 fileSize;
    quint64 fileOffset;
    quint64 readingCursorPosition;

    std::vector<unsigned char> buffer;
    int pieceMemoryCounter;

    unsigned char* getMemory(int);
public:
    PieceMemoryStorage(int pieceSize, int maxPieces);
    int read(unsigned char* buf, size_t len);
    void write(unsigned char* buf
               , int len
               , int offset
               , qint32 pieceIndex);
    int seek(quint64 pos);

    /**
     * @brief obtainRanges obtain memory ranges to copy into output buffer base on current reading position
     * @param len in bytes
     * @return one or two memory ranges two ranges in case of no-continuous memory blocks
     */
    QPair<Range, Range> obtainRanges(size_t len);

    void requestPieces();
};

#endif // PIECEMEMORYSTORAGE_H
