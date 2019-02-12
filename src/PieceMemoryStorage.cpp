#include "PieceMemoryStorage.h"

#include <QMutexLocker>

Piece::Piece(int index, int capacity, int pieceMemoryIndex):
    index(index)
  , capacity(capacity)
  , pieceMemoryIndex(pieceMemoryIndex) {
}

int Piece::bytesAvailable() const {
    return range.getSegments().isEmpty() || range.getSegments().at(0).first != 0?
                0:range.getSegments().at(0).second - range.getSegments().at(0).first;
}

PieceMemoryStorage::PieceMemoryStorage(int pieceLength
                                       , int lastPieceLength
                                       , int firstPiece
                                       , int lastPiece
                                       , int maxPieces
                                       , qlonglong fileSize
                                       , qlonglong fileOffset) :
    pieceLength(pieceLength)
  , lastPieceLength(lastPieceLength)
  , firstPiece(firstPiece)
  , lastPiece(lastPiece)
  , maxPieces(std::min(maxPieces, lastPiece - firstPiece + 1))
  , requestPiece(firstPiece)
  , buffer(std::min(maxPieces, lastPiece - firstPiece + 1)*pieceLength)
  , memoryIndex(0)
  , fileSize(fileSize)
  , fileOffset(fileOffset)
  , fileReadOffset(0l) {
    resetMemBlocks();
    requestPieces();    
}

int PieceMemoryStorage::read(unsigned char* buf, size_t len) {
    Q_UNUSED(buf);
    Q_UNUSED(len);
    QMutexLocker lock(&mutex);
    // obtain bytes
    obtainRanges(len);
    size_t totalBytesObtained = memBlocks[0].size + memBlocks[1].size;
    Q_ASSERT(len >= totalBytesObtained);
    fileReadOffset += totalBytesObtained;

    size_t offset = 0;
    for(int i = 0; i < 2; ++i) {
        if (memBlocks[i].ptr != nullptr) {
            memcpy(buf + offset, memBlocks[i].ptr, memBlocks[i].size);
            offset = memBlocks[i].size;
        }
    }

    // wait
    if (memBlocks[0].ptr == nullptr && memBlocks[1].ptr == nullptr) {
        bufferHasData.wait(&mutex);
    }

    return -1;
}

void PieceMemoryStorage::write(const unsigned char* buf
                               , int len
                               , int offset
                               , int pieceIndex) {
    Q_UNUSED(buf);
    Q_UNUSED(len);
    QMutexLocker lock(&mutex);
    QList<Piece>::iterator itr = std::find_if(pieces.begin(), pieces.end(), [pieceIndex](const Piece& piece){ return piece.index == pieceIndex; });
    if (itr != pieces.end()) {
        Q_ASSERT(getMemory(itr->pieceMemoryIndex) != nullptr);
        Q_ASSERT(itr->capacity >= offset + len);
        memcpy(getMemory(itr->pieceMemoryIndex) + offset, buf, static_cast<size_t>(len));
        itr->range += qMakePair(offset, offset + len);

        if (itr->bytesAvailable() > 0 && itr == pieces.begin()) {
            // first piece has got bytes, wake all
        }
    }
}

int PieceMemoryStorage::seek(quint64 pos) {
    Q_UNUSED(pos);
    return -1;
}

void PieceMemoryStorage::obtainRanges(size_t len) {
    qlonglong absoluteReadingPosition = fileReadOffset + fileOffset;
    qint32 pieceIndex = static_cast<qint32>(absoluteReadingPosition / static_cast<qlonglong>(pieceLength));
    Q_ASSERT(absoluteReadingPosition >= static_cast<qlonglong>(pieceIndex*pieceLength));
    qint32 readingCursorInPiece = static_cast<qint32>(absoluteReadingPosition - static_cast<qlonglong>(pieceIndex)*pieceLength);

    assert(readingCursorInPiece < getPieceLength(pieceIndex));
    resetMemBlocks();
    int rangeIndex = 0;
    QList<Piece>::iterator itr = pieces.begin();    

    qint32 bytesRemain = static_cast<qint32>(len);

    for(;itr != pieces.end(); ++itr) {
        // not enough bytes
        if (itr->bytesAvailable() <= readingCursorInPiece) {
            break;
        }

        // first piece reading started from some offset
        qint32 takeBytes = qMin(bytesRemain, static_cast<qint32>(itr->bytesAvailable() - readingCursorInPiece));

        if (itr->pieceMemoryIndex == 0) rangeIndex = 1;
        if (memBlocks[rangeIndex].ptr == nullptr) memBlocks[rangeIndex].ptr = getMemory(itr->pieceMemoryIndex);
        memBlocks[rangeIndex].size += static_cast<size_t>(takeBytes);
        bytesRemain -= takeBytes;

        // current piece is not full - do not move forward
        if (!itr->isFull()) break;

        if (bytesRemain == 0) {
            // all bytes from piece were captured - piece has to be full
            if (takeBytes == (itr->bytesAvailable() - readingCursorInPiece)) {
                Q_ASSERT(itr->isFull());
                ++itr;
            }
            break;
        }

        // all next pieces reading starting from zero
        readingCursorInPiece = 0;
    }

    pieces.erase(pieces.begin(), itr);
}

void PieceMemoryStorage::requestPieces() {
    if (pieces.size() == maxPieces) return;
    int requestPieces = std::min(maxPieces - pieces.size(), lastPiece - requestPiece + 1);
    for(int i = 0; i < requestPieces; ++i) {
        // assign correct capacity to last piece
        pieces.append(Piece(requestPiece, getPieceLength(requestPiece), nextPieceMemoryIndex(memoryIndex++)));
        ++requestPiece;
    }
}

unsigned char* PieceMemoryStorage::getMemory(int index) {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < maxPieces);
    return &buffer[static_cast<unsigned int>(index) * static_cast<unsigned int>(pieceLength)];
}

int PieceMemoryStorage::nextPieceMemoryIndex(int currentIndex) const {
    return currentIndex % maxPieces;
}

void PieceMemoryStorage::resetMemBlocks() {
    memBlocks[0] = {nullptr, 0};
    memBlocks[1] = {nullptr, 0};
}
