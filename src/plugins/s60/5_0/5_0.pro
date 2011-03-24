include(../s60pluginbase.pri)

TARGET  = qts60plugin_5_0$${QT_LIBINFIX}

contains(S60_VERSION, 3.1) {
    SOURCES += ../src/qlocale_3_1.cpp \
        ../src/qdesktopservices_3_1.cpp \
        ../src/qcoreapplication_3_1.cpp \
        ../src/ccpueditorproxy_3_1.cpp
} else {
    SOURCES += ../src/qlocale_3_2.cpp \
        ../src/qdesktopservices_3_2.cpp \
        ../src/qcoreapplication_3_2.cpp \
        ../src/ccpueditorproxy_3_2.cpp

    HEADERS += ../src/ccpueditorproxy.h

    LIBS += -lavkon \
            -leikcoctl \
            -leikctl \
            -lform \
            -luiklaf

    contains(CONFIG, is_using_gnupoc) {
        LIBS += -ldirectorylocalizer
    } else {
        LIBS += -lDirectoryLocalizer
    }
    LIBS += -lefsrv
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

HEADERS += ../src/qccpueditorinterface_p.h

TARGET.UID3=0x2001E622
