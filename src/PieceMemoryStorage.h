#ifndef PIECEMEMORYSTORAGE_H
#define PIECEMEMORYSTORAGE_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QList>
#include <QPair>

#include <vector>
#include <memory>

#include "Range.h"

class MemHolder {
    unsigned char* ptr;
    int sz;
public:
    MemHolder(int size);
    ~MemHolder();
    void write(const unsigned char* src, int offset, int len);
    void read(unsigned char* dst, int offset, int len) const;
    constexpr int size() const { return sz; }
};

class PieceMemoryStorage;
typedef std::shared_ptr<MemHolder> SPMem;

struct Piece {
    int index;
    int capacity;
    Range<int> range;
    PieceMemoryStorage& storage;
    SPMem mem;

    Piece(int index
          , int capacity
          , PieceMemoryStorage& storage
          , SPMem mem);
    ~Piece();

    bool isFull() const {
        return capacity == bytesAvailable();
    }

    int bytesAvailable() const;
};

typedef std::shared_ptr<Piece> SPPiece;

class PieceMemoryStorage {
private:
    QMutex mutex;
    QWaitCondition bufferHasData;
    QList<SPPiece>  pieces;
    QList<SPMem>    memory;

    int pieceLength;
    int lastPieceLength;
    int maxCachePieces;

    int memoryIndex;
    qlonglong fileSize;         //!< file size
    qlonglong fileOffset;       //!< absolute file offset in torrent
    qlonglong readingPosition;  //!< current reading offset in file

    int readImpl(unsigned char* buf, int len);
public:
    PieceMemoryStorage(int pieceLength
                       , int lastPieceLength
                       , int maxCachePieces
                       , qlonglong fileOffset
                       , qlonglong fileSize);
    int read(unsigned char* buf, size_t len);
    void write(const unsigned char* buf
               , int len
               , int offset
               , int pieceIndex);

    int seek(quint64 pos);

    /**
     * @brief requestPieces register new pieces requests
     */
    void requestPieces();

    const QList<SPPiece>& requestedPieces() const {
        return pieces;
    }

    constexpr int getPieceLength(int index)  const {
        Q_ASSERT(index >= firstPiece());
        Q_ASSERT(index <= lastPiece());
        return index==lastPiece()?lastPieceLength:pieceLength;
    }

    /**
     * @brief absoluteReadingPosition
     * @return current absolute reading position in whole torrent
     */
    constexpr qlonglong absoluteReadingPosition() const { return fileOffset + readingPosition; }

    /**
     * @brief readingPiece
     * @return current reading piece index
     */
    constexpr int readingPiece() const { return static_cast<int>(absoluteReadingPosition() / static_cast<qlonglong>(pieceLength)); }

    constexpr int firstPiece() const { return static_cast<int>((fileOffset / pieceLength)); }
    constexpr int lastPiece() const { return static_cast<int>((fileOffset + fileSize - 1) / pieceLength); }
    constexpr int maxPieces() const { return std::min(maxCachePieces, lastPiece() - firstPiece() + 1); }

    void release(SPMem& mem);
    SPMem allocate();
};

#endif // PIECEMEMORYSTORAGE_H
