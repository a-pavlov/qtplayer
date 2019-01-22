#include "PieceMemoryStorage.h"

#include <QMutexLocker>

PieceMemoryStorage::PieceMemoryStorage(int pieceSize, int maxPieces) :
    pieceSize(pieceSize)
  , maxPieces(maxPieces)
{

}

int PieceMemoryStorage::read(unsigned char* buf, size_t len) {
    Q_UNUSED(buf);
    Q_UNUSED(len);
    QMutexLocker lock(&mutex);
    // obtain bytes
    bool bytes = false;
    // request bytes
    if (!bytes) {
        bufferHasData.wait(&mutex);
    }

    return -1;
}

void PieceMemoryStorage::write(unsigned char* buf, size_t len) {
    Q_UNUSED(buf);
    Q_UNUSED(len);
    QMutexLocker lock(&mutex);
    // write data to piece
    // if zero byte filled - wake up threads
}

int PieceMemoryStorage::seek(quint64 pos) {
    Q_UNUSED(pos);
    return -1;
}

QPair<Range, Range> PieceMemoryStorage::obtainRanges(size_t len) {
    quint64 absoluteReadingPosition = readingCursorPosition + fileOffset;
    int pieceIndex = static_cast<int>(absoluteReadingPosition / static_cast<quint64>(pieceSize));
    int readingCursorInPiece = static_cast<int>(absoluteReadingPosition - static_cast<quint64>(pieceIndex*pieceSize));

    assert(readingCursorInPiece < pieceSize);

    Range ranges[2] = {{nullptr, 0}, {nullptr, 0}};
    int rangeIndex = 0;
    QList<Piece>::iterator itr = pieces.begin();

    int bytesRemain = static_cast<int>(len);

    for(;itr != pieces.end(); ++itr) {
        // first piece reading started from some offset
        int takeBytes = qMin(bytesRemain, itr->size - readingCursorInPiece);

        // no available bytes
        if (takeBytes <= 0) {
            break;
        }

        if (isFirstMemoryBlock(*itr)) rangeIndex = 1;
        if (ranges[rangeIndex].first == nullptr) ranges[rangeIndex].first = itr->ptr;
        ranges[rangeIndex].second += takeBytes;
        bytesRemain -= takeBytes;

        // current piece is not full - do not move forward
        if (!itr->isFull()) break;

        if (bytesRemain == 0) {
            // increase iterator to remove it since
            if (itr->isFull()) ++itr;
            break;
        }

        // all next pieces reading starting from zero
        readingCursorInPiece = 0;
    }

    pieces.erase(pieces.begin(), itr);
    return {ranges[0], ranges[1]};
}
