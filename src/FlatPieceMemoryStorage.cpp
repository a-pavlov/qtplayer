#include "FlatPieceMemoryStorage.h"

#include <QDebug>
#include <QMutexLocker>

#include <cassert>
#include <algorithm>

FlatPieceMemoryStorage::FlatPieceMemoryStorage(int pieceLength
    , int maxCachePieces
    , qlonglong fileOffset
    , qlonglong fileSize) :
        buffer(new unsigned char[pieceLength * maxCachePieces])
        , pieceLen(pieceLength)
        , cacheSizeInPieces(maxCachePieces)
        , fOffset(fileOffset)
        , fSize(fileSize)
        , absRPos(fileOffset)
        , updatingBuffer(false) {
    absWPos = firstPieceOffset();
}

FlatPieceMemoryStorage::~FlatPieceMemoryStorage() {
    delete[] buffer;
}

int FlatPieceMemoryStorage::read(unsigned char* buf, size_t len) {
    int length = static_cast<int>(len);

    mutex.lock();

    if (absRPos >= absWPos) {
        // no bytes available for reading
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
    requestSlots(absRPos / pieceLen);
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
        int dataPos = posInCacheByPiece(pieceIndex) + offset;
        assert(dataPos >= 0);
        mutex.unlock();

        memcpy(buffer + dataPos, buf, len);

        mutex.lock();

        // update current slot bytes available
        itr->second += qMakePair(offset, offset + len);

        // move forward writing position as much as possible
        for (; itr != slotList.end(); ++itr) {
            int writingSlot = absWPos / pieceLen;

            // writing position in current slot
            if (writingSlot == itr->first) {
                // set writing position to new position if bytes available greater than last writing position
                absWPos = std::max(pieceAbsPos(itr->first) + itr->second.bytesAvailable(), absWPos);
            }

            // slot is not full, do not continue
            if (itr->second.bytesAvailable() < pieceLen) {
                break;
            }
        }

        if (absWPos > absRPos) {
            // wake up reading only if absolute writing position greater than absolute reading position
            bufferNotEmpty.wakeAll();
        }

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
    qlonglong newAbsPos = pos + fOffset;
    absRPos = newAbsPos;

    int currentPieceIndex = absWPos / pieceLen;
    int newPieceIndex = newAbsPos / pieceLen;
    absWPos = (currentPieceIndex == newPieceIndex) ? absWPos : pieceAbsPos(newPieceIndex);
    requestSlots(newPieceIndex);
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

void checkSlotsInvariant(const QList<Slot>& list) {
    auto startSlotIndex = -1;
    for (auto slot : list) {
        if (startSlotIndex != -1) {
            assert(++startSlotIndex == slot.first);
        }
        else {
            startSlotIndex = slot.first;
        }
    }
}

void FlatPieceMemoryStorage::requestSlots(int pieceIndexStartFrom) {
    slotList.erase(std::remove_if(slotList.begin(), slotList.end()
        , [=](Slot slot) {
            return slot.first < pieceIndexStartFrom;
        })
        , slotList.end());

    if (!slotList.isEmpty()) {
        assert(pieceIndexStartFrom <= slotList.begin()->first);
        int gapSize = slotList.begin()->first - pieceIndexStartFrom;

        // remove out of cache size elements in slot list
        if (gapSize >= cacheSizeInPieces) {
            slotList.clear();
        } else {
            while (gapSize + slotList.size() > cacheSizeInPieces) {
                slotList.removeLast();
            }
        }

        // prepend slots
        if (!slotList.isEmpty()) {
            int slotIndex = slotList.begin()->first;
            assert(pieceIndexStartFrom <= slotIndex);
            while (slotIndex != pieceIndexStartFrom) {
                slotList.prepend(qMakePair(--slotIndex, Range<int>()));
            }

            pieceIndexStartFrom = slotList.last().first + 1;
        }
    }

    // append slots
    while (slotList.size() < cacheSizeInPieces && pieceIndexStartFrom <= lastPiece()) {
        slotList.append(qMakePair(pieceIndexStartFrom++, Range<int>()));
    }

    checkSlotsInvariant(slotList);

    QList<int> rp;
    std::transform(slotList.begin(), slotList.end(), std::back_inserter(rp), [](Slot s) { return s.first; });
    emit piecesRequested(rp);
}
