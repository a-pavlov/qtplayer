#ifndef FILEMEDIASTREAM_H
#define FILEMEDIASTREAM_H

#include <QObject>
#include <QFile>

#include "core/VlcMediaMemoryStream.h"

class FileMediaStream : public VlcMediaMemoryStream {
private:
    char buff[1024*1024*2];
    qint64 currentBufferPos;
    qint64 totalBytesBuffer;
    QFile file;
public:
    explicit FileMediaStream(const QString&);
    virtual ~FileMediaStream();
    virtual int open();
    virtual void close();
    virtual uint64_t totalBytes();
    virtual ssize_t read(unsigned char* buf, size_t len);
    virtual int seek(uint64_t);

signals:

public slots:
};

#endif // FILEMEDIASTREAM_H
