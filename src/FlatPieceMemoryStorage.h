#ifndef FLATPIECEMEMORYSTORAGE_H
#define FLATPIECEMEMORYSTORAGE_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QPair>


#include <algorithm>

#include "Range.h"

typedef QPair<int, Range<int> > Slot;

class FlatPieceMemoryStorage: public QObject {
    Q_OBJECT
private:
    unsigned char* buffer;
    int pieceLen;
    int lastPieceLen;
    int cacheSizeInPieces;
    qlonglong fOffset;
    qlonglong fSize;
    qlonglong absRPos;
    qlonglong absWPos;
    QMutex mutex;

    /**
     * @brief updatingBuffer true when writing bytes in progress
     */
    bool updatingBuffer;
    QWaitCondition bufferNotEmpty;
    QWaitCondition bufferNotFull;
    QWaitCondition bufferNotUpdating;
    QList<Slot> slotList;
public:
    FlatPieceMemoryStorage(int pieceLength
        , int lastPieceLength
        , int maxCachePieces
        , qlonglong fileOffset
        , qlonglong fileSize);
    ~FlatPieceMemoryStorage();

    constexpr int firstPiece() const { return static_cast<int>((fOffset / pieceLen)); }
    constexpr int lastPiece() const { return static_cast<int>((fOffset + fSize - 1) / pieceLen); }
    constexpr int maxPieces() const { return std::min(cacheSizeInPieces, lastPiece() - firstPiece() + 1); }
    constexpr int cacheSize() const { return pieceLen * cacheSizeInPieces; }

    constexpr qlonglong pieceAbsPos(int pieceIndex) const {
        return static_cast<qlonglong>(pieceIndex)*pieceLen;
    }

    constexpr qlonglong firstPieceOffset() const {
        return pieceAbsPos(firstPiece());
    }

    constexpr int posInCacheByAbsPos(qlonglong absoluteOffset) const {
        return static_cast<int>((absoluteOffset - firstPieceOffset()) % cacheSize());
    }

    constexpr int posInCacheByPiece(int pieceIndex) const {
        return ((pieceIndex - firstPiece()) % cacheSizeInPieces) * pieceLen;
    }

    constexpr int getPieceLength(int index)  const {
        Q_ASSERT(index >= firstPiece());
        Q_ASSERT(index <= lastPiece());
        return index == lastPiece() ? lastPieceLen : pieceLen;
    }

    // just for testing purposes
    constexpr int bytesToBeCopied(int len) {
        int localRPos = posInCacheByAbsPos(absRPos);
        int localWPos = posInCacheByAbsPos(absWPos);
        int distance = localWPos - localRPos;
        return distance > 0 ? std::min(len, distance) : (cacheSize() - localRPos) + std::min(len - cacheSize() + localRPos, localWPos);
    }

    qlonglong absoluteReadingPosition();
    qlonglong absoluteWritingPosition();

    int read(unsigned char* buf, size_t len);

    void write(const unsigned char* buf
        , int len
        , int offset
        , int pieceIndex);

    int seek(quint64 pos);

    void requestPieces();
signals:
    void piecesRequested(QList<int>);
    void pieceOutOfRangeReceived(int length, int offset, int pieceIndex);
};

#endif // FLATPIECEMEMORYSTORAGE_H
