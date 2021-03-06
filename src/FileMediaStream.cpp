#include "FileMediaStream.h"
#include <QDebug>

FileMediaStream::FileMediaStream(const QString& fn) :
    currentBufferPos(0)
  , totalBytesBuffer(0)
  , file(fn) {

}

FileMediaStream::~FileMediaStream() {
    if (file.isOpen()) file.close();
    qDebug() << "file memory stread removed";
}

int FileMediaStream::open() {
    qDebug() << "open file";
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "unable to open file ";
        return -1;
    }

    qDebug() << "file is opened";
    return 0;
}

void FileMediaStream::close() {
    if (file.isOpen()) file.close();
}


uint64_t FileMediaStream::totalBytes() {
    uint64_t res = file.isOpen()?static_cast<uint64_t>(file.size()):UINT64_MAX;    
    return res;
}

ssize_t FileMediaStream::read(unsigned char* buf, size_t len) {
    //qDebug() << Q_FUNC_INFO << " len: " << len;
    if (!file.isOpen()) return -1;

    Q_ASSERT(currentBufferPos <= totalBytesBuffer);
    if (totalBytesBuffer - currentBufferPos == 0) {
        totalBytesBuffer = file.read(buff, sizeof (buff));
        if (totalBytesBuffer == -1) totalBytesBuffer = 0;
        currentBufferPos = 0;
    }

    size_t copyBytes = qMin(len, static_cast<size_t>(totalBytesBuffer - currentBufferPos));
    //qDebug() << "copy bytes " << copyBytes << " offset " << currentBufferPos << " remain " << (totalBytesBuffer - currentBufferPos);
    memcpy(buf, buff + currentBufferPos, copyBytes);
    currentBufferPos += copyBytes;
    Q_ASSERT(currentBufferPos <= totalBytesBuffer);
    return static_cast<ssize_t>(copyBytes);
}

int FileMediaStream::seek(uint64_t offset) {
    if (!file.isOpen()) return -1;
    totalBytesBuffer = 0;
    currentBufferPos = 0;
    if (file.seek(0l) && file.seek(offset)) {
        qDebug() << "seeks completed " << offset;
        return 0;
    } else {
        qDebug() << "seek failed " << offset;
        return -1;
    }
}
