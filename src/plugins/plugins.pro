TEMPLATE = subdirs

SUBDIRS	*= sqldrivers script
unix:!symbian {
        contains(QT_CONFIG,iconv)|contains(QT_CONFIG,gnu-libiconv):SUBDIRS *= codecs
} else {
        SUBDIRS *= codecs
}
!contains(QT_CONFIG, no-gui): SUBDIRS *= accessible imageformats iconengines
!embedded:SUBDIRS *= graphicssystems
embedded:SUBDIRS *=  gfxdrivers decorations mousedrivers kbddrivers
!win32:!embedded:!mac:!symbian:SUBDIRS *= inputmethods
symbian:SUBDIRS += s60
contains(QT_CONFIG, phonon): SUBDIRS *= phonon
contains(QT_CONFIG, multimedia): SUBDIRS *= audio
