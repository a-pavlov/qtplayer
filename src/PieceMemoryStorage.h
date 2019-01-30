#ifndef PIECEMEMORYSTORAGE_H
#define PIECEMEMORYSTORAGE_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QList>
#include <QPair>

#include <vector>

#include "Range.h"

struct Piece {
    qint32 index;
    qint32 capacity;
    int pieceMemoryIndex;
    Range<int> range;

    Piece(int index, int capacity, int pieceMemoryIndex);

    bool isFull() const {
        return capacity == bytesAvailable();
    }

    int bytesAvailable() const;
};

typedef QPair<unsigned char*, int> MemoryBlock;

class PieceMemoryStorage {
private:
    QMutex mutex;
    QWaitCondition bufferHasData;
    QList<Piece> pieces;    

    int pieceLength;
    int lastPieceLength;
    int firstPiece;
    int lastPiece;
    int maxPieces;
    int lastRequestedPiece;


    qlonglong fileSize;         //!< file size
    qlonglong fileOffset;       //!< absolute file offset in torrent
    qlonglong fileReadOffset;   //!< current reading offset in file

    std::vector<unsigned char> buffer;

    int memoryIndex;

    unsigned char* getMemory(int);
public:
    PieceMemoryStorage(int pieceLength
                       , int lastPieceLength
                       , int firstPiece
                       , int lastPiece
                       , int maxPieces
                       , qlonglong fileSize
                       , qlonglong fileOffset);
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
    QPair<MemoryBlock, MemoryBlock> obtainRanges(size_t len);

    /**
     * @brief requestPieces register new pieces requests
     */
    void requestPieces();

    int nextPieceMemoryIndex(int) const;

    const QList<Piece>& requestedPieces() const {
        return pieces;
    }

    constexpr int getPieceLength(int index)  const{
        Q_ASSERT(index >= firstPiece);
        Q_ASSERT(index <= lastPiece);
        return index==lastPiece?lastPieceLength:pieceLength;
    }
};

#endif // PIECEMEMORYSTORAGE_H
