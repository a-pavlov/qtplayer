# COMPILATION SPECIFIC
!nox:dbus {
  QT += dbus
}

QMAKE_CXXFLAGS += -Wformat -Wformat-security -Werror=return-type -Wno-unused-parameter
QMAKE_LFLAGS_APP += -rdynamic
INCLUDEPATH += $$(LIBVLC_ROOT)/include $$(LIBVLC_ROOT)/include/vlc/plugins
LIBS += -L${LIBVLC_LIBS} -lvlc

ANDROID_EXTRA_LIBS += $$(LIBVLC_LIBS)/libvlc.so

# INSTALL
target.path = $$PREFIX/bin/
INSTALLS += target

