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
    QWaitCondition bufferNotEmpty;      // wake this condition in case of absolute writing position moved forward
    QWaitCondition bufferNotFull;
    QWaitCondition bufferNotUpdating;   // wake this condition in case of write method finished
    QList<Slot> slotList;
public:
    FlatPieceMemoryStorage(int pieceLength
        , int maxCachePieces
        , qlonglong fileOffset
        , qlonglong fileSize);
    ~FlatPieceMemoryStorage();

    int firstPiece() const { return static_cast<int>((fOffset / pieceLen)); }
    int lastPiece() const { return static_cast<int>((fOffset + fSize - 1) / pieceLen); }
    int maxPieces() const { return std::min(cacheSizeInPieces, lastPiece() - firstPiece() + 1); }
    int cacheSize() const { return pieceLen * cacheSizeInPieces; }
    qlonglong bytesInLastPiece() const {
        return (fSize + fOffset - lastPiece()*pieceLen);
    }

    qlonglong pieceAbsPos(int pieceIndex) const {
        return static_cast<qlonglong>(pieceIndex)*pieceLen;
    }

    qlonglong firstPieceOffset() const {
        return pieceAbsPos(firstPiece());
    }

    int posInCacheByAbsPos(qlonglong absoluteOffset) const {
        return static_cast<int>((absoluteOffset - firstPieceOffset()) % cacheSize());
    }

    int posInCacheByPiece(int pieceIndex) const {
        return ((pieceIndex - firstPiece()) % cacheSizeInPieces) * pieceLen;
    }

    int getPieceLength()  const {
        return pieceLen;
    }

    // just for testing purposes
    int bytesToBeCopied(int len) {
        int localRPos = posInCacheByAbsPos(absRPos);
        int localWPos = posInCacheByAbsPos(absWPos);
        int distance = localWPos - localRPos;
        return distance > 0 ? std::min(len, distance) : (cacheSize() - localRPos) + std::min(len - cacheSize() + localRPos, localWPos);
    }

    qlonglong absoluteReadingPosition();
    qlonglong absoluteWritingPosition();


    /* reading bytes by in player's thread. Wait on bufferNotEmpty */
    int read(unsigned char* buf, size_t len);

    /*  move reading position in player's thread, expect it won't be in parallel with reading ^^^.
        Wait on bufferNotUpdating
    */
    int seek(quint64 pos);

    /* writing bytes to buffer in source thread */
    void write(const unsigned char* buf
        , int len
        , int offset
        , int pieceIndex);

    void requestSlots(int pieceIndexStartFrom);
    const QList<Slot>& getSlots() const {
        return slotList;
    }

signals:
    void piecesRequested(QList<int>);
    void pieceOutOfRangeReceived(int length, int offset, int pieceIndex);
};

#endif // FLATPIECEMEMORYSTORAGE_H
