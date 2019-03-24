#include "PieceMemoryStorage.h"

#include <QMutexLocker>

Piece::Piece(int index
             , int capacity
             , int pieceMemoryIndex):
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
  , memoryIndex(0)
  , fileSize(fileSize)
  , fileOffset(fileOffset)
  , readingPosition(0l)
  , buffer(static_cast<size_t>(std::min(maxPieces, lastPiece - firstPiece + 1)*pieceLength)) {
    requestPieces();    
}

int PieceMemoryStorage::readImpl(unsigned char* buf, int len) {
    if (pieces.isEmpty()) return 0;
    auto piece = pieces.at(0);
    qlonglong currentReadingPosition = absoluteReadingPosition();
    int currentPieceIndex = readingPiece();
    Q_ASSERT(currentPieceIndex == piece.index);
    int readingCursorInPiece = static_cast<int>(currentReadingPosition - static_cast<qlonglong>(currentPieceIndex)*getPieceLength(currentPieceIndex));
    int takeBytes = 0;
    if (piece.bytesAvailable() > readingCursorInPiece) {
        takeBytes = qMin(len, piece.bytesAvailable() - readingCursorInPiece);
        memcpy(buf, getMemory(piece.pieceMemoryIndex), static_cast<size_t>(takeBytes));
        readingPosition += takeBytes;
        // check current piece
        if (readingPiece() > piece.index) {
            Q_ASSERT(piece.isFull());
            pieces.pop_front();
            if (len > takeBytes) {
                takeBytes += readImpl(buf + takeBytes, len - takeBytes);
            }
        }
    }

    return takeBytes;
}

int PieceMemoryStorage::read(unsigned char* buf, size_t len) {
    Q_UNUSED(buf);
    Q_UNUSED(len);
    QMutexLocker lock(&mutex);
    int bytes = readImpl(buf, static_cast<int>(len));

    // wait
    //if (memBlocks[0].ptr == nullptr && memBlocks[1].ptr == nullptr) {
    //    bufferHasData.wait(&mutex);
    //}

    // TODO - fix this
    return bytes;
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
