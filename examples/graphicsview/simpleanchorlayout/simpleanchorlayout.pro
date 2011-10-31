SOURCES   = main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/simpleanchorlayout
sources.files = $$SOURCES $$HEADERS $$RESOURCES simpleanchorlayout.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/simpleanchorlayout
INSTALLS += target sources

TARGET = simpleanchorlayout

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
wince*: include($$QT_SOURCE_TREE/examples/wincepkgrules.pri)
simulator: warning(This example might not fully work on Simulator platform)
