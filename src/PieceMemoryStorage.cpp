#include "PieceMemoryStorage.h"

#include <QMutexLocker>

PieceMemoryStorage::PieceMemoryStorage(int pieceSize
                                       , int maxPieces) :
    pieceSize(pieceSize)
  , maxPieces(maxPieces)
  , buffer(pieceSize*maxPieces)
  , pieceMemoryCounter(0)
{

}

int PieceMemoryStorage::read(unsigned char* buf, size_t len) {
    Q_UNUSED(buf);
    Q_UNUSED(len);
    QMutexLocker lock(&mutex);
    // obtain bytes
    QPair<Range, Range> ranges = obtainRanges(len);



    if (ranges.first.first == nullptr && ranges.second.first == nullptr) {
        bufferHasData.wait(&mutex);
    }

    return -1;
}

void PieceMemoryStorage::write(unsigned char* buf
                               , int len
                               , int offset
                               , qint32 pieceIndex) {
    Q_UNUSED(buf);
    Q_UNUSED(len);
    QMutexLocker lock(&mutex);
    QList<Piece>::iterator itr = std::find_if(pieces.begin(), pieces.end(), [pieceIndex](const Piece& piece){ return piece.index == pieceIndex; });
    if (itr != pieces.end()) {
        Q_ASSERT(getMemory(itr->pieceMemoryIndex) != nullptr);
        Q_ASSERT(itr->size >= offset + len);
        memcpy(getMemory(itr->pieceMemoryIndex) + offset, buf, len);
    }
}

int PieceMemoryStorage::seek(quint64 pos) {
    Q_UNUSED(pos);
    return -1;
}

QPair<Range, Range> PieceMemoryStorage::obtainRanges(size_t len) {
    quint64 absoluteReadingPosition = readingCursorPosition + fileOffset;
    qint32 pieceIndex = static_cast<qint32>(absoluteReadingPosition / static_cast<quint64>(pieceSize));
    Q_ASSERT(absoluteReadingPosition >= static_cast<quint64>(pieceIndex*pieceSize));
    qint32 readingCursorInPiece = static_cast<qint32>(absoluteReadingPosition - static_cast<quint64>(pieceIndex*pieceSize));

    assert(readingCursorInPiece < pieceSize);

    Range ranges[2] = {{nullptr, 0}, {nullptr, 0}};
    int rangeIndex = 0;
    QList<Piece>::iterator itr = pieces.begin();    

    qint32 bytesRemain = static_cast<qint32>(len);

    for(;itr != pieces.end(); ++itr) {
        // not enough bytes
        if (itr->size <= readingCursorInPiece) {
            break;
        }

        // first piece reading started from some offset
        qint32 takeBytes = qMin(bytesRemain, static_cast<qint32>(itr->size - readingCursorInPiece));

        if (itr->pieceMemoryIndex == 0) rangeIndex = 1;
        if (ranges[rangeIndex].first == nullptr) ranges[rangeIndex].first = getMemory(itr->pieceMemoryIndex);
        ranges[rangeIndex].second += takeBytes;
        bytesRemain -= takeBytes;

        // current piece is not full - do not move forward
        if (!itr->isFull()) break;

        if (bytesRemain == 0) {
            // all bytes from piece were captured - piece has to be full
            if (takeBytes == (itr->size - readingCursorInPiece)) {
                Q_ASSERT(itr->isFull());
                ++itr;
            }
            break;
        }

        // all next pieces reading starting from zero
        readingCursorInPiece = 0;
    }

    pieces.erase(pieces.begin(), itr);
    return {ranges[0], ranges[1]};
}

void PieceMemoryStorage::requestPieces() {
    if (pieces.size() == maxPieces) return;

}

unsigned char* PieceMemoryStorage::getMemory(int index) {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < maxPieces);
    return &buffer[index * pieceSize];
}

int PieceMemoryStorage::nextPieceMemoryIndex(int currentIndex) const {
    return currentIndex % maxPieces;
}
