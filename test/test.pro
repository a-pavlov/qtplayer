QT += testlib
SOURCES = test.cpp
RESOURCES += res.qrc

INCLUDEPATH += ../src
LIBS += -L../src -lsrc

include(../unixconf.pri)
