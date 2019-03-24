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
    int index;
    int capacity;
    int pieceMemoryIndex;
    Range<int> range;

    Piece(int index, int capacity, int pieceMemoryIndex);

    bool isFull() const {
        return capacity == bytesAvailable();
    }

    int bytesAvailable() const;
};

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
    int requestPiece;


    int memoryIndex;
    qlonglong fileSize;         //!< file size
    qlonglong fileOffset;       //!< absolute file offset in torrent
    qlonglong readingPosition;  //!< current reading offset in file

    std::vector<unsigned char> buffer;

    unsigned char* getMemory(int);

    int readImpl(unsigned char* buf, int len);
public:
    PieceMemoryStorage(int pieceLength
                       , int lastPieceLength
                       , int firstPiece
                       , int lastPiece
                       , int maxPieces
                       , qlonglong fileSize
                       , qlonglong fileOffset);
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

    int nextPieceMemoryIndex(int) const;

    const QList<Piece>& requestedPieces() const {
        return pieces;
    }

    constexpr int getPieceLength(int index)  const {
        Q_ASSERT(index >= firstPiece);
        Q_ASSERT(index <= lastPiece);
        return index==lastPiece?lastPieceLength:pieceLength;
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

};

#endif // PIECEMEMORYSTORAGE_H
