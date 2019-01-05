TEMPLATE = app

QT += qml quick widgets svg network

RESOURCES += qml.qrc images.qrc

QTPLUGN += qsvg

# Windows specific configuration
win32 {
  include(winconf.pri)
}

# Mac specific configuration
macx {
  include(macxconf.pri)
}

# Unix specific configuration

unix:!android {
  include(unixconf.pri)
}


android {
  include(android.pri)
}

include(vlcqt/core/core.pri)
include(vlcqt/qml/qml.pri)
include(utils/utils.pri)
include(bittorrent/bittorrent.pri)
#include(net/net.pri)

INCLUDEPATH += vlcqt

HEADERS += mainwindow.h $$PWD/FileMediaStream.h
SOURCES += main.cpp mainwindow.cpp $$PWD/FileMediaStream.cpp

DESTDIR = .

# Additional import path used to resolve QML modules in Qt Creators code model
#QML_IMPORT_PATH += qml

DISTFILES += \
    qml/SeekControl.qml



