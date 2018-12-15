#ifndef VLCMEDIAMEMORYSTREAM_H
#define VLCMEDIAMEMORYSTREAM_H

#include <QtCore/QObject>

// TODO - use later
#include <vlc/libvlc_version.h>

class VlcMediaMemoryStream {
public:
    virtual ~VlcMediaMemoryStream();
    virtual int open() = 0;
    virtual void close() = 0;
    virtual uint64_t totalBytes() = 0;
    virtual ssize_t read(unsigned char* buf, size_t len) = 0;
    virtual int seek(uint64_t) = 0;
};

#endif // VLCMEDIAMEMORYSTREAM_H
