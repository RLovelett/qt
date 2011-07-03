TEMPLATE = subdirs

SUBDIRS	*= sqldrivers script bearer
!contains(QT_CONFIG, no-gui): SUBDIRS *= imageformats iconengines
!embedded:!qpa:!contains(QT_CONFIG, no-gui):SUBDIRS *= graphicssystems
embedded:SUBDIRS *=  gfxdrivers decorations mousedrivers kbddrivers
!win32:!embedded:!mac:!symbian:!contains(QT_CONFIG, no-gui):SUBDIRS *= inputmethods
!symbian:!contains(QT_CONFIG, no-gui):SUBDIRS += accessible
contains(QT_CONFIG, phonon): SUBDIRS *= phonon
qpa:SUBDIRS += platforms
contains(QT_CONFIG, declarative): SUBDIRS *= qmltooling
