TARGET = qwbmp

include(../../qpluginbase.pri)

QTDIR_build:REQUIRES = "!contains(QT_CONFIG, no-wbmp)"

# Input files
HEADERS += qwbmphandler.h
SOURCES += qwbmphandler.cpp \
           qwbmpplugin.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

symbian:TARGET.UID3=0x2001E616
