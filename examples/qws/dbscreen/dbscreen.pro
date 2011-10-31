TEMPLATE    = lib
CONFIG     += plugin

TARGET      = dbscreen
target.path += $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS    += target

HEADERS     = dbscreen.h 
SOURCES     = dbscreendriverplugin.cpp \
              dbscreen.cpp 

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
wince*: include($$QT_SOURCE_TREE/examples/wincepkgrules.pri)
symbian: warning(This example does not work on Symbian platform)
simulator: warning(This example does not work on Simulator platform)
