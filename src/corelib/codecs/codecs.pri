# Qt core library codecs module

HEADERS += \
	codecs/qisciicodec_p.h \
	codecs/qlatincodec_p.h \
	codecs/qsimplecodec_p.h \
	codecs/qtextcodec_p.h \
	codecs/qtextcodec.h \
	codecs/qtsciicodec_p.h \
	codecs/qutfcodec_p.h \
	codecs/qtextcodecplugin.h

SOURCES += \
	codecs/qisciicodec.cpp \
	codecs/qlatincodec.cpp \
	codecs/qsimplecodec.cpp \
	codecs/qtextcodec.cpp \
	codecs/qtsciicodec.cpp \
	codecs/qutfcodec.cpp \
	codecs/qtextcodecplugin.cpp

unix {
	SOURCES += codecs/qfontlaocodec.cpp

        contains(QT_CONFIG,iconv) {
                HEADERS += codecs/qiconvcodec_p.h
                SOURCES += codecs/qiconvcodec.cpp
        } else:contains(QT_CONFIG,gnu-libiconv) {
                HEADERS += codecs/qiconvcodec_p.h
                SOURCES += codecs/qiconvcodec.cpp

                DEFINES += GNU_LIBICONV
                !mac:LIBS_PRIVATE *= -liconv
        } else:contains(QT_CONFIG,sun-libiconv) {
                HEADERS += codecs/qiconvcodec_p.h
                SOURCES += codecs/qiconvcodec.cpp
                DEFINES += GNU_LIBICONV
        }
}
symbian:LIBS += -lcharconv
else {
HEADERS += \
	codecs/qgb18030codec.h \
	codecs/qeucjpcodec.h \
	codecs/qjiscodec.h \
	codecs/qsjiscodec.h \
	codecs/qeuckrcodec.h \
	codecs/qbig5codec.h \
	codecs/qfontjpcodec.h

SOURCES += \
	codecs/qgb18030codec.cpp \
	codecs/qjpunicode.cpp \
	codecs/qeucjpcodec.cpp \
	codecs/qjiscodec.cpp \
	codecs/qsjiscodec.cpp \
	codecs/qeuckrcodec.cpp \
	codecs/qbig5codec.cpp \
	codecs/qfontjpcodec.cpp
}
