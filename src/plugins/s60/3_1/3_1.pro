include(../s60pluginbase.pri)

TARGET  = qts60plugin_3_1$${QT_LIBINFIX}

SOURCES += ../src/qlocale_3_1.cpp \
    ../src/qdesktopservices_3_1.cpp \
    ../src/qcoreapplication_3_1.cpp \
    ../src/ccpueditorproxy_3_1.cpp

HEADERS += ../src/qccpueditorinterface_p.h
TARGET.UID3=0x2001E620
