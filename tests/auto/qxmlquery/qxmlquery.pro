load(qttest_p4)
SOURCES += tst_qxmlquery.cpp MessageValidator.cpp TestFundament.cpp
HEADERS += PushBaseliner.h                              \
           MessageSilencer.h                            \
           ../qsimplexmlnodemodel/TestSimpleNodeModel.h \
           MessageValidator.h                           \
           NetworkOverrider.h

RESOURCES = input.qrc

QT += network

include (../xmlpatterns.pri)

wince*: {
   addFiles.sources = pushBaselines input.xml
   addFiles.path    = .

   patternistFiles.sources = ../xmlpatterns/queries
   patternistFiles.path    = ../xmlpatterns

   DEPLOYMENT += addFiles patternistFiles
}
