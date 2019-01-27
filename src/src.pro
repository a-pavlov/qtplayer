TEMPLATE = lib
CONFIG += staticlib

QT += qml quick widgets svg network

RESOURCES += images.qrc

QTPLUGN += qsvg

# Unix specific configuration
unix:!android {
  include(../unixconf.pri)
}

# Android specific configuration
android {
  include(../android.pri)
}

include(vlcqt/core/core.pri)
include(vlcqt/qml/qml.pri)
include(utils/utils.pri)
include(bittorrent/bittorrent.pri)
#include(net/net.pri)

INCLUDEPATH += $$PWD/vlcqt $$PWD

HEADERS += FileMediaStream.h \
    PieceMemoryStorage.h \
    Range.h

SOURCES += FileMediaStream.cpp \
    PieceMemoryStorage.cpp


DESTDIR = .

# Additional import path used to resolve QML modules in Qt Creators code model
#QML_IMPORT_PATH += qml



