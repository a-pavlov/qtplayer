TEMPLATE = app

QT += qml quick widgets svg network

QTPLUGN += qsvg

INCLUDEPATH += ../src ../src/vlcqt ../src/bittorrent

LIBS += -L../src -lsrc

HEADERS += mainwindow.h
SOURCES += main.cpp mainwindow.cpp

unix:!android {
  include(../unixconf.pri)
}

DESTDIR = .

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creators code model
#QML_IMPORT_PATH += qml

DISTFILES += \
    qml/SeekControl.qml



