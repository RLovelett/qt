load(qttest_p4)
TEMPLATE = app
TARGET = tst_javascript

QT += testlib script

# our test files are stored in our app as resources
RESOURCES = sunspider.qrc

SOURCES += main.cpp \
           json.cpp \
           sunspider.cpp
