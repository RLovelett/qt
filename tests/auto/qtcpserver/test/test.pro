load(qttest_p4)
SOURCES  += ../tst_qtcpserver.cpp

win32: {
wince*: {
	LIBS += -lws2
	crashApp.sources = ../crashingServer/crashingServer.exe
	crashApp.path = crashingServer
	DEPLOYMENT += crashApp
} else {
	LIBS += -lws2_32
}
}

TARGET = ../tst_qtcpserver

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qtcpserver
} else {
    TARGET = ../../release/tst_qtcpserver
  }
}

QT = core network

MOC_DIR=tmp

DEFINES += TEST_QNETWORK_PROXY



