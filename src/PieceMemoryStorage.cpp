#include "PieceMemoryStorage.h"
#include <cassert>
#include <exception>

#include <QMutexLocker>

MemHolder::MemHolder(int size): ptr(new unsigned char[static_cast<size_t>(size)]), sz(size) {}

MemHolder::~MemHolder() {
    delete [] ptr;
}

void MemHolder::write(const unsigned char* src, int offset, int count) {
    assert(offset >= 0);
    assert(count >= 0);
    assert(offset + count <= sz);
    memcpy(ptr + offset, src, static_cast<size_t>(count));

}

void MemHolder::read(unsigned char* dst, int offset, int count) const {
    assert(offset + count <= sz);
    memcpy(dst, ptr + offset, static_cast<size_t>(count));
}

Piece::Piece(int index
             , int capacity
             , PieceMemoryStorage& s
             , SPMem m):
    index(index)
    , capacity(capacity)
    , storage(s)
    , mem(m)
   {
}

int Piece::bytesAvailable() const {
    return range.getSegments().isEmpty() || range.getSegments().at(0).first != 0?
                0:range.getSegments().at(0).second - range.getSegments().at(0).first;
}

Piece::~Piece() {
    storage.release(mem);
}

PieceMemoryStorage::PieceMemoryStorage(int pieceLength
                                       , int lastPieceLength
                                       , int maxCachePieces
                                       , qlonglong fileOffset
                                       , qlonglong fileSize) :
    pieceLength(pieceLength)
  , lastPieceLength(lastPieceLength)  
  , maxCachePieces(maxCachePieces)
  , memoryIndex(0)
  , fileSize(fileSize)
  , fileOffset(fileOffset)
  , readingPosition(0l) {
}

int PieceMemoryStorage::readImpl(unsigned char* buf, int len) {
    if (pieces.isEmpty()) return 0;
    auto piece = pieces.at(0);
    qlonglong currentReadingPosition = absoluteReadingPosition();
    int currentPieceIndex = readingPiece();
    Q_ASSERT(currentPieceIndex == piece->index);
    int readingCursorInPiece = static_cast<int>(currentReadingPosition - static_cast<qlonglong>(currentPieceIndex)*getPieceLength(currentPieceIndex));
    int takeBytes = 0;
    if (piece->bytesAvailable() > readingCursorInPiece) {
        takeBytes = qMin(len, piece->bytesAvailable() - readingCursorInPiece);
        piece->mem->read(buf, readingCursorInPiece, takeBytes);
        readingPosition += takeBytes;
        // check current piece
        if (readingPiece() > piece->index) {
            Q_ASSERT(piece->isFull());
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
    QList<SPPiece>::iterator itr = std::find_if(pieces.begin(), pieces.end(), [pieceIndex](const SPPiece& piece){ return piece->index == pieceIndex; });
    if (itr != pieces.end()) {
        SPPiece piece = *itr;
        Q_ASSERT(piece->capacity >= offset + len);
        piece->mem->write(buf, offset, len);
        piece->range += qMakePair(offset, offset + len);

        if (piece->bytesAvailable() > 0 && itr == pieces.begin()) {
            // first piece has got bytes, wake all
        }
    }
}

int PieceMemoryStorage::seek(quint64 pos) {
    Q_UNUSED(pos);
    QMutexLocker lock(&mutex);
    readingPosition = static_cast<qlonglong>(pos);
    int p = readingPiece();
    pieces.erase(std::remove_if(pieces.begin(), pieces.end(), [p](const SPPiece& piece){ return piece->index < p; }), pieces.end());
    return 0;
}

void PieceMemoryStorage::requestPieces() {
    if (pieces.size() == maxPieces()) return;
    int requestPiece = readingPiece();
    int requestPieces = std::min(maxPieces() - pieces.size(), lastPiece() - requestPiece + 1);
    for(int i = 0; i < requestPieces; ++i) {
        // assign correct capacity to last piece
        pieces.append(std::shared_ptr<Piece>(new Piece(requestPiece, getPieceLength(requestPiece), *this, allocate())));
        ++requestPiece;
    }
}

void PieceMemoryStorage::release(SPMem& mem) {
    memory.append(mem);
}

SPMem PieceMemoryStorage::allocate() {
    return (memory.isEmpty())?std::make_shared<MemHolder>(static_cast<size_t>(pieceLength)):memory.takeFirst();
}
