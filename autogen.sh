#!/bin/bash
set -e

QT_S60_DIR=$PWD
tmp=`mktemp -d`
trap "echo Removing temporary files...; cd /tmp && rm -rf $tmp" EXIT

cd $tmp
mkdir -p tools bin src/corelib/global

# Create missing headers
QTDIR=$QT_S60_DIR perl $QT_S60_DIR/bin/syncqt -outdir $PWD

# Create placeholders for qconfig.h/qconfig.cpp
echo '/* All features enabled while building qmake */' > src/corelib/global/qconfig.h
cat > src/corelib/global/qconfig.cpp << EOF
static const char qt_configure_licensee_str          [512 + 12] = "qt_lcnsuser=Open Source";
static const char qt_configure_licensed_products_str [512 + 12] = "qt_lcnsprod=OpenSource";
static const char qt_configure_prefix_path_str       [512 + 12] = "qt_prfxpath=$PWD";
static const char qt_configure_documentation_path_str[512 + 12] = "qt_docspath=$PWD/doc";
static const char qt_configure_headers_path_str      [512 + 12] = "qt_hdrspath=$PWD/include";
static const char qt_configure_libraries_path_str    [512 + 12] = "qt_libspath=$PWD/lib";
static const char qt_configure_binaries_path_str     [512 + 12] = "qt_binspath=$PWD/bin";
static const char qt_configure_plugins_path_str      [512 + 12] = "qt_plugpath=$PWD/plugins";
static const char qt_configure_data_path_str         [512 + 12] = "qt_datapath=$PWD";
static const char qt_configure_translations_path_str [512 + 12] = "qt_trnspath=$PWD/translations";
static const char qt_configure_examples_path_str     [512 + 12] = "qt_xmplpath=$PWD/example";
static const char qt_configure_demos_path_str        [512 + 12] = "qt_demopath=$PWD/demos";
#define QT_CONFIGURE_LICENSEE qt_configure_licensee_str + 12;
#define QT_CONFIGURE_LICENSED_PRODUCTS qt_configure_licensed_products_str + 12;
#define QT_CONFIGURE_PREFIX_PATH qt_configure_prefix_path_str + 12;
#define QT_CONFIGURE_DOCUMENTATION_PATH qt_configure_documentation_path_str + 12;
#define QT_CONFIGURE_HEADERS_PATH qt_configure_headers_path_str + 12;
#define QT_CONFIGURE_LIBRARIES_PATH qt_configure_libraries_path_str + 12;
#define QT_CONFIGURE_BINARIES_PATH qt_configure_binaries_path_str + 12;
#define QT_CONFIGURE_PLUGINS_PATH qt_configure_plugins_path_str + 12;
#define QT_CONFIGURE_DATA_PATH qt_configure_data_path_str + 12;
#define QT_CONFIGURE_TRANSLATIONS_PATH qt_configure_translations_path_str + 12;
#define QT_CONFIGURE_EXAMPLES_PATH qt_configure_examples_path_str + 12;
#define QT_CONFIGURE_DEMOS_PATH qt_configure_demos_path_str + 12;
EOF

# FIXME: find out why "make" is not looking for qmake source files in the
# source tree. For now, simply copy the sources
cp -a $QT_S60_DIR/qmake qmake

cat > qmake/Makefile << "EOF"
CC = gcc
CXX = g++
QMAKE_CFLAGS = -pipe
QMAKE_CXXFLAGS = $(QMAKE_CFLAGS)
QMAKE_LFLAGS =
EOF
sed -e "s,@SOURCE_PATH@,$QT_S60_DIR,g" \
    -e "s,@BUILD_PATH@,$PWD,g" \
    -e "s,@QMAKE_QTOBJS@,,g" \
    -e "s,@QMAKE_QTSRCS@,,g" \
    -e "s,@QMAKESPEC@,$QT_S60_DIR/mkspecs/linux-g++,g" \
    -e "s,@QMAKE_LFLAGS@,\$(QMAKE_LFLAGS),g" \
    -e "s,@QMAKE_CXXFLAGS@, \$(QMAKE_CXXFLAGS) -DQMAKE_OPENSOURCE_EDITION,g" \
    $QT_S60_DIR/qmake/Makefile.unix >> qmake/Makefile

make -C qmake

cp -a $QT_S60_DIR/tools/configure tools
sed -i -e "1i\\QT_SOURCE_TREE = $QT_S60_DIR" \
       -e '1i\QT_BUILD_TREE = ../..' \
       tools/configure/configure.pro

(cd tools/configure &&
QMAKESPEC=$QT_S60_DIR/mkspecs/linux-g++ $tmp/bin/qmake)

make -C tools/configure
cp configure $QT_S60_DIR/configure.bin
