#include "FlatPieceMemoryStorage.h"

#include <QDebug>
#include <QMutexLocker>

#include <cassert>
#include <algorithm>

FlatPieceMemoryStorage::FlatPieceMemoryStorage(int pieceLength
    , int lastPieceLength
    , int maxCachePieces
    , qlonglong fileOffset
    , qlonglong fileSize) :
        buffer(new unsigned char[pieceLength * maxCachePieces])
        , pieceLen(pieceLength)
        , lastPieceLen(lastPieceLength)
        , cacheSizeInPieces(maxCachePieces)
        , fOffset(fileOffset)
        , fSize(fileSize)
        , absRPos(fileOffset)
        , absWPos(fileOffset)
        , updatingBuffer(false) {
}

FlatPieceMemoryStorage::~FlatPieceMemoryStorage() {
    delete[] buffer;
}

int FlatPieceMemoryStorage::read(unsigned char* buf, size_t len) {
    int length = static_cast<int>(len);

    mutex.lock();

    if (absRPos == absWPos) {
        bufferNotEmpty.wait(&mutex);
    }

    assert(absWPos > absRPos);

    // we have bytes
    int localRPos = posInCacheByAbsPos(absRPos);
    int localWPos = posInCacheByAbsPos(absWPos);

    mutex.unlock();

    int distance = localWPos - localRPos;
    int bytesToBeCopied = distance > 0 ? std::min(length, distance) : (cacheSize() - localRPos) + std::min(length - cacheSize() + localRPos, localWPos);
    assert(distance != 0);
    assert(bytesToBeCopied > 0);

    int obtainBytes = -1;

    if (distance > 0 || (cacheSize() - localRPos) > static_cast<int>(len)) {
        // forward direction to the writing position
        obtainBytes = std::min(static_cast<int>(len), distance > 0 ? localWPos - localRPos : cacheSize() - localRPos);
        memcpy(buf, buffer + localRPos, obtainBytes);
    }
    else {
        // forward direction to the end of cache
        int firstPieceLen = cacheSize() - localRPos;

        if (firstPieceLen > 0) {
            memcpy(buf, buffer + localRPos, firstPieceLen);
        }

        // forward direction from zero to writing position
        // calculate remain bytes
        obtainBytes = std::min(static_cast<int>(len) - firstPieceLen, localWPos);

        if (obtainBytes > 0) {
            memcpy(buf + firstPieceLen, buffer, obtainBytes);
        }

        obtainBytes += firstPieceLen;
    }

    assert(obtainBytes == bytesToBeCopied);
    assert(obtainBytes != -1);

    mutex.lock();
    absRPos += obtainBytes;
    // remove all read slots
    int lastSlotIndex = slotList.back().first;
    slotList.erase(std::remove_if(slotList.begin(), slotList.end()
                   , [=](Slot slot) {
        return absRPos >= (pieceAbsPos(slot.first) + getPieceLength(slot.first));
    })
    , slotList.end());

    // request new slots
    while(slotList.size() < cacheSizeInPieces && lastSlotIndex < lastPiece()) {
        slotList.append(qMakePair(++lastSlotIndex, Range<int>()));
    }

    // send signal to external system for request new slots here

    bufferNotFull.wakeAll();
    mutex.unlock();

    return obtainBytes;
}

void FlatPieceMemoryStorage::write(const unsigned char* buf
    , int len
    , int offset
    , int pieceIndex) {
    mutex.lock();
    // check this piece in our list
    auto itr = std::lower_bound(slotList.begin(), slotList.end()
                                , qMakePair(pieceIndex, Range<int>())
                                , [](const Slot& l, const Slot& r){ return l.first < r.first; });

    if (itr != slotList.end() && itr->first == pieceIndex) {
        updatingBuffer = true;
        qlonglong slotAbsPosition = pieceAbsPos(pieceIndex);
        int writingSlot = absWPos / pieceLen;
        int dataPos = posInCacheByPiece(pieceIndex) + offset;
        assert(dataPos >= 0);
        mutex.unlock();

        memcpy(buffer + dataPos, buf, len);

        mutex.lock();

        int bytesBefore = itr->second.bytesAvailable();
        itr->second += qMakePair(offset, offset + len);
        // write data
        int bytesAfter = itr->second.bytesAvailable();

        // writing position in current slot
        if (writingSlot == pieceIndex) {
            // set writing position to new position if bytes available greater than last writing position
            absWPos = std::max(slotAbsPosition + bytesAfter, absWPos);
        }

        //if (bytesAfter != bytesBefore && ((slotAbsPosition + bytesBefore) == absWPos)) {
        //    absWPos += len;
        //}

        bufferNotUpdating.wakeAll();
        updatingBuffer = false;
    } else {
        qDebug() << "skip piece " << pieceIndex << " data offset " << offset << " length " << len;
        emit pieceOutOfRangeReceived(len, offset, pieceIndex);
    }

    mutex.unlock();
}

int FlatPieceMemoryStorage::seek(quint64 pos) {
    mutex.lock();
    if (updatingBuffer) bufferNotUpdating.wait(&mutex);
    assert(!updatingBuffer);

    mutex.unlock();
    return 0;
}

qlonglong FlatPieceMemoryStorage::absoluteReadingPosition() {
    QMutexLocker lock(&mutex);
    return absRPos;
}

qlonglong FlatPieceMemoryStorage::absoluteWritingPosition() {
    QMutexLocker lock(&mutex);
    return absWPos;
}

void FlatPieceMemoryStorage::requestPieces() {
    int lastSlotIndex = std::max(static_cast<int>(absRPos/pieceLen), slotList.isEmpty()?-1:slotList.back().first + 1);
    assert(lastSlotIndex >= 0);
    QList<int> rp;
    std::transform(slotList.begin(), slotList.end(), std::back_inserter(rp), [](Slot s) { return s.first; });

    while(slotList.size() < cacheSizeInPieces && lastSlotIndex <= lastPiece()) {
        slotList.append(qMakePair(lastSlotIndex++, Range<int>()));
        rp.append(slotList.back().first);
    }

    emit piecesRequested(rp);
}
