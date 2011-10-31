SOURCES = digiflip.cpp

symbian {
    TARGET.UID3 = 0xA000CF72
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
}
wince*: include($$QT_SOURCE_TREE/demos/wincepkgrules.pri)

target.path = $$[QT_INSTALL_DEMOS]/embedded/digiflip
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded/digiflip
INSTALLS += target sources
