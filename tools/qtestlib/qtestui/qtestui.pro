QT += core \
    gui
TARGET = qtestui
TEMPLATE = app
SOURCES += src/mainwindow.cpp \
    src/main.cpp \
    src/tabwidget.cpp \
    src/testrunner.cpp \
    src/testcase.cpp \
    src/testconfig.cpp \
    src/testresult.cpp \
    src/pips.cpp \
    src/outputoptdlg.cpp \
    src/eventoptdlg.cpp \
    src/benchmarkoptdlg.cpp
HEADERS += qtestui.loc \
    inc/mainwindow.h \
    inc/tabwidget.h \
    inc/testrunner.h \
    inc/testconfig.h \
    inc/testresult.h \
    inc/testcase.h \
    inc/pips.h \
    inc/outputoptdlg.h \
    inc/eventoptdlg.h \
    inc/benchmarkoptdlg.h \
    inc/version.h
RESOURCES += resource/qtestui.qrc
symbian { 
    TARGET.UID3 = 0x200345D5
    TARGET.CAPABILITY += AllFiles \
        PowerMgmt
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 \
        0x800000
}
OTHER_FILES += resource/qss/blue.qss
