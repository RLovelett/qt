TEMPLATE  = subdirs

printers.path = $$[QT_INSTALL_DATA]/gdb-python/libQtCore
printers.files = libQtCore/printers.py libQtCore/__init__.py
INSTALLS += printers

hooksed.target = libQtCore.so.$$[QT_VERSION]-gdb.py
hooksed.commands = sed -e 's,@pythondir@,$$[QT_INSTALL_DATA]/gdb-python,' \
        -e 's,@toolexeclibdir@,$$[QT_INSTALL_LIBS],' < $$QT_SOURCE_TREE/tools/gdb-python/hook.in > libQtCore.so.$$[QT_VERSION]-gdb.py
QMAKE_EXTRA_TARGETS += hooksed

hook.path = $$[QT_INSTALL_LIBS]
hook.files = libQtCore.so.$$[QT_VERSION]-gdb.py
hook.depends = hooksed
INSTALLS += hook

