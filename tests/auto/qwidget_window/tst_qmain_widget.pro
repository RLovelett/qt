load(qttest_p4)
SOURCES += tst_qmain_widget.cpp

x11 {
    LIBS += $$QMAKE_LIBS_X11
}
