A test application for verifying QKeyMapper implementation manually.

The test application contains a text edit widget for checking that the characters output from
different keys and key combinations are correct on different keyboard layouts. You need to set
the current keyboard layout before starting the application. For example in Windows 7 you can
change the current layout from Control Panel - Region and Language - Keyboards and Languages -
Change Keyboards - Default Input Language.

The application also shows the key and event from QWidget's keyPressEvent signal as one of the use
cases for the test application is to verify fix for QTBUG-19176.
