isEmpty(RC_FILE):contains(TEMPLATE, ".*app") {
    RC_FILE = $$QT_SOURCE_TREE/examples/qt.rc
}