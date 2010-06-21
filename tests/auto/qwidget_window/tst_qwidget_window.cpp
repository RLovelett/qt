/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QtGui/QtGui>
#include <qeventloop.h>
#include <qlist.h>

#include <qlistwidget.h>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <QX11Info>
#include <stdio.h>
#include <unistd.h>
#endif // Q_WS_X11

#include "../../shared/util.h"


class tst_QWidget_window : public QWidget
{
    Q_OBJECT

public:
    tst_QWidget_window(){};

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void tst_showGeometry();
    void tst_getGeometry();
    void tst_move_show();
    void tst_show_move();
    void tst_show_move_hide_show();

    void tst_resize_show();
    void tst_show_resize();
    void tst_show_resize_hide_show();

    void tst_windowFilePathAndwindowTitle_data();
    void tst_windowFilePathAndwindowTitle();
    void tst_windowFilePath_data();
    void tst_windowFilePath();

    void tst_showWithoutActivating();
    void tst_paintEventOnSecondShow();
};

void tst_QWidget_window::initTestCase()
{
}

void tst_QWidget_window::cleanupTestCase()
{
}

// helper class to print the parameters for the current window if the
// test fails.
class PrintParams
{
public:
    PrintParams(const char *n) : name(n), exit_print(true),
        fw(-1), fh(-1), fx(-1), fy(-1),
        ww(-1), wh(-1), wx(-1), wy(-1) {}
    ~PrintParams()
    {
        if (exit_print)
            print();
    }
    void print()
    {
        printf("frame  %s w %d h %d x %d y %d\n", name, fw, fh, fx, fy);
        printf("widget %s w %d h %d x %d y %d\n", name, ww, wh, wx, wy);
        QWidget w;
        QRect screenRect = QApplication::desktop()->screenGeometry(&w);
        printf("screen geometry    %d x %d\n", screenRect.width(), screenRect.height());
    }
    void parse(QProcess &p)
    {
        QByteArray out=p.readAllStandardOutput();
        QVERIFY2(out.constData(), "pointer not NULL");
        QVERIFY2(*out.constData(), "not empty string");
        QVERIFY(sscanf(out.constData(), "frame %d %d %d %d widget %d %d %d %d",
            &fw, &fh, &fx, &fy, &ww, &wh, &wx, &wy) == 8);
    }
    const char * name;
    bool exit_print;
    int fw, fh, fx, fy;
    int ww, wh, wx, wy;
};

void tst_QWidget_window::tst_showGeometry()
{
#ifndef Q_WS_X11
    QSKIP("This test is X11-only.", SkipAll);
#else
    // Note, the tests assume that any toolbar that will reduce the avaiable
    // desktop geometry (and window placement) does so less than 50 pixels
    // so the requested absolute positions will place the window at that
    // location.
    QString program("./tst_qmain_widget");
    QStringList args;
    // common to all
    args.push_back("-geometry");
    // replaced per window
    args.push_back("");
    // Startup multiple windows at the same time, and do some validation on
    // where they appear.
    const int count=10;
    QProcess p[count];
    QString geometry[count]={
        "+50+50",
        "200x200",
        "320x240+50+50",
        "320x240-50+50",
        "320x240+50-50",
        "320x240-50-50",
        "320x240+60+60",
        "320x240-60+60",
        "320x240+60-80",
        "320x240-60-80"};
    // start them all at once
    for (int i=0; i<count; ++i)
    {
        args[1]=geometry[i];
        p[i].start(program, args);
        // Pause slightly to serialize startup to make sure the first window is
        // up before the second starts.
        usleep(20*1000);
    }
    bool process_fail=false;
    // wait for them to finish
    for (int i=0; i<count; ++i)
    {
        p[i].waitForFinished(3000);
        // If the prgoram failed to start, it's
        // NotRunning, NormalExit, and FailedToSTart
        // If the program started, ran, and exited without any errors, it's
        // NotRunning, NormalExit, and UnknownError
        if (p[i].state() != QProcess::NotRunning
            || p[i].exitStatus() != QProcess::NormalExit
            || p[i].error() != QProcess::UnknownError) {
            process_fail=true;
        }
    }
    // Can't verify in the loop, or the remaining QProcess objects will
    // complain "Destroyed while process is still running."
    QVERIFY(!process_fail);

    QWidget w;
    QRect screenRect = QApplication::desktop()->screenGeometry(&w);

    PrintParams pp1("pp1");
    PrintParams pp2("pp2");
    pp1.parse(p[0]);
    // Should be placed an exact location, verify.
    // Note, there's a race condition in the window manager placing the
    // window and Qt getting the frame and widget geometry.  tst_qmain_widget
    // gets around this by quering the X server directly.
    // geometry from Qt
    QVERIFY2(pp1.fx == 50 && pp1.fy == 50, "position for +50+50");
    pp2.parse(p[1]);
    // Should be placed an exact size, verify.
    QVERIFY2(pp2.ww == 200 && pp2.wh == 200, "size for 200x200");
    // Can't be at the same location (can't be at 0, 0), or at least it's
    // unlikely the window manager would do that, before this changeset Qt
    // would force the window to 0,0 when a width and height was specified even
    // if +0+0 wasnt.
    QVERIFY2(pp1.fx != pp2.fx || pp1.fy != pp2.fy, "not the same location");

    pp2.parse(p[2]);
    QVERIFY2(pp2.fx == 50 && pp2.fy == 50, "position for 320x240+50+50");
    QVERIFY2(pp2.ww == 320 && pp2.wh == 240, "size for 320x240+50+50");

    pp2.parse(p[3]);
    QVERIFY2(pp2.fy == 50 && pp2.fx + pp2.fw == screenRect.width() - 50,
        "top right 320x240-50+50");
    pp2.parse(p[4]);
    QVERIFY2(pp2.fy + pp2.fh == screenRect.height() - 50 && pp2.fx == 50,
        "bottom left 320x240+50-50");
    pp2.parse(p[5]);
    QVERIFY2(pp2.fy + pp2.fh == screenRect.height() - 50
        && pp2.fx + pp2.fw == screenRect.width() - 50,
        "bottom right 320x240-50-50");

    pp2.parse(p[6]);
    QVERIFY2(pp2.fx == 60 && pp2.fy == 60, "position for 320x240+60+60");
    QVERIFY2(pp2.ww == 320 && pp2.wh == 240, "size for 320x240+60+60");

    pp2.parse(p[7]);
    QVERIFY2(pp2.fy == 60 && pp2.fx + pp2.fw == screenRect.width() - 60,
        "top right 320x240-10+10");
    pp2.parse(p[8]);
    QVERIFY2(pp2.fy + pp2.fh == screenRect.height() - 80 && pp2.fx == 60,
        "bottom left 320x240+60-80");
    pp2.parse(p[9]);
    QVERIFY2(pp2.fy + pp2.fh == screenRect.height() - 80
        && pp2.fx + pp2.fw == screenRect.width() - 60,
        "bottom right 320x240-60-80");

    // disable printing now that none of the tests failed
    pp1.exit_print=pp2.exit_print=false;
#endif // Q_WS_X11
}

void tst_QWidget_window::tst_getGeometry()
{
#ifndef Q_WS_X11
    QSKIP("This test is X11-only.", SkipAll);
#else
    // Startup multiple windows at the same location and check to make sure
    // they all give the same results.  Qt 4.6 fails and there doesn't seem
    // to be any way to invalidate the geometry and try again later.
    QSKIP("Test would fail, tst_qmain_widget.cpp", SkipAll);
    QString program("./tst_qmain_widget");
    QStringList args;
    args.push_back("--query-qt");
    args.push_back("-geometry");
    args.push_back("100x100+0+0");
    const int count=20;
    QProcess p[count];
    // start them all at once
    for (int i=0; i<count; ++i)
    {
        p[i].start(program, args);
    }
    bool process_fail=false;
    // wait for them to finish
    for (int i=0; i<count; ++i)
    {
        p[i].waitForFinished(4000);
        // If the prgoram failed to start, it's
        // NotRunning, NormalExit, and FailedToSTart
        // If the program started, ran, and exited without any errors, it's
        // NotRunning, NormalExit, and UnknownError
        if (p[i].state() != QProcess::NotRunning
            || p[i].exitStatus() != QProcess::NormalExit
            || p[i].error() != QProcess::UnknownError) {
            process_fail=true;
        }
    }
    // Can't verify in the loop, or the remaining QProcess objects will
    // complain "Destroyed while process is still running."
    QVERIFY(!process_fail);

    PrintParams pp2("pp2");
    PrintParams pp1("pp1");
    pp1.parse(p[0]);
    //pp1.print();
    QString message;
    QByteArray buffer;
    for (int i=1; i<count; ++i)
    {
        pp2.parse(p[i]);
        //pp2.print();
        message.sprintf("comparing 0 and %d positions", i);
        buffer=message.toAscii();
        QVERIFY2(pp1.fw == pp2.fw && pp1.fh == pp2.fh, buffer.constData());
        QVERIFY2(pp1.fx == pp2.fx && pp1.fy == pp2.fy, buffer.constData());
        QVERIFY2(pp1.ww == pp2.ww && pp1.wh == pp2.wh, buffer.constData());
        QVERIFY2(pp1.wx == pp2.wx && pp1.wy == pp2.wy, buffer.constData());
    }

    // disable printing now that none of the tests failed
    pp1.exit_print=pp2.exit_print=false;
#endif // Q_WS_X11
}

void tst_QWidget_window::tst_move_show()
{
    QWidget w;
    w.move(100, 100);
    w.show();
    QCOMPARE(w.pos(), QPoint(100, 100));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 3000);
}

void tst_QWidget_window::tst_show_move()
{
    QWidget w;
    w.show();
    w.move(100, 100);
    QCOMPARE(w.pos(), QPoint(100, 100));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_show_move_hide_show()
{
    QWidget w;
    w.show();
    w.move(100, 100);
    w.hide();
    w.show();
    QCOMPARE(w.pos(), QPoint(100, 100));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_resize_show()
{
    QWidget w;
    w.resize(200, 200);
    w.show();
    QCOMPARE(w.size(), QSize(200, 200));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_show_resize()
{
    QWidget w;
    w.show();
    w.resize(200, 200);
    QCOMPARE(w.size(), QSize(200, 200));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_show_resize_hide_show()
{
    QWidget w;
    w.show();
    w.resize(200, 200);
    w.hide();
    w.show();
    QCOMPARE(w.size(), QSize(200, 200));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

class TestWidget : public QWidget
{
public:
    int m_first, m_next;
    bool paintEventReceived;

    void reset(){ m_first = m_next = 0; paintEventReceived = false; }
    bool event(QEvent *event)
    {
        switch (event->type()) {
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::Hide:
        case QEvent::Show:
            if (m_first)
                m_next = event->type();
            else
                m_first = event->type();
            break;
        case QEvent::Paint:
            paintEventReceived = true;
            break;
        default:
            break;
        }
        return QWidget::event(event);
    }
};

void tst_QWidget_window::tst_windowFilePathAndwindowTitle_data()
{
    QTest::addColumn<bool>("setWindowTitleBefore");
    QTest::addColumn<bool>("setWindowTitleAfter");
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("applicationName");
    QTest::addColumn<QString>("indyWindowTitle");
    QTest::addColumn<QString>("finalTitleBefore");
    QTest::addColumn<QString>("finalTitleAfter");

    QString validPath = QApplication::applicationFilePath();
    QString appName = QLatin1String("Killer App");
    QString fileNameOnly = QFileInfo(validPath).fileName() + QLatin1String("[*]");
    QString fileAndApp = fileNameOnly + QLatin1String(" ") + QChar(0x2014) + QLatin1String(" ") + appName;
    QString windowTitle = QLatin1String("Here is a Window Title");

    QTest::newRow("never Set Title nor AppName") << false << false << validPath << QString() << windowTitle << fileNameOnly << fileNameOnly;
    QTest::newRow("set title after only, but no AppName") << false << true << validPath << QString() << windowTitle << fileNameOnly << windowTitle;
    QTest::newRow("set title before only, not AppName") << true << false << validPath << QString() << windowTitle << windowTitle << windowTitle;
    QTest::newRow("always set title, not appName") << true << true << validPath << QString() << windowTitle << windowTitle << windowTitle;

    QString platString =
#ifdef Q_WS_MAC
        fileNameOnly;
#else
        fileAndApp;
#endif

    QTest::newRow("never Set Title, yes AppName") << false << false << validPath << appName << windowTitle << platString << platString;
    QTest::newRow("set title after only, yes AppName") << false << true << validPath << appName << windowTitle << platString << windowTitle;
    QTest::newRow("set title before only, yes AppName") << true << false << validPath << appName << windowTitle << windowTitle << windowTitle;
    QTest::newRow("always set title, yes appName") << true << true << validPath << appName << windowTitle << windowTitle << windowTitle;
}

void tst_QWidget_window::tst_windowFilePathAndwindowTitle()
{
    QFETCH(bool, setWindowTitleBefore);
    QFETCH(bool, setWindowTitleAfter);
    QFETCH(QString, filePath);
    QFETCH(QString, applicationName);
    QFETCH(QString, indyWindowTitle);
    QFETCH(QString, finalTitleBefore);
    QFETCH(QString, finalTitleAfter);


    QWidget widget;
    QCOMPARE(widget.windowFilePath(), QString());

    if (!applicationName.isEmpty())
        qApp->setApplicationName(applicationName);
    else
        qApp->setApplicationName(QString());

    if (setWindowTitleBefore) {
        widget.setWindowTitle(indyWindowTitle);
    }
    widget.setWindowFilePath(filePath);
    QCOMPARE(finalTitleBefore, widget.windowTitle());
    QCOMPARE(widget.windowFilePath(), filePath);

    if (setWindowTitleAfter) {
        widget.setWindowTitle(indyWindowTitle);
    }
    QCOMPARE(finalTitleAfter, widget.windowTitle());
    QCOMPARE(widget.windowFilePath(), filePath);
}

void tst_QWidget_window::tst_windowFilePath_data()
{
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("result");
    QTest::addColumn<bool>("again");
    QTest::addColumn<QString>("filePath2");
    QTest::addColumn<QString>("result2");

    QString validPath = QApplication::applicationFilePath();
    QString invalidPath = QLatin1String("::**Never a Real Path**::");

    QTest::newRow("never Set Path") << QString() << QString() << false << QString() << QString();
    QTest::newRow("never EVER Set Path") << QString() << QString() << true << QString() << QString();
    QTest::newRow("Valid Path") << validPath << validPath << false << QString() << QString();
    QTest::newRow("invalid Path") << invalidPath << invalidPath << false << QString() << QString();
    QTest::newRow("Valid Path then empty") << validPath << validPath << true << QString() << QString();
    QTest::newRow("invalid Path then empty") << invalidPath << invalidPath << true << QString() << QString();
    QTest::newRow("invalid Path then valid") << invalidPath << invalidPath << true << validPath << validPath;
    QTest::newRow("valid Path then invalid") << validPath << validPath << true << invalidPath << invalidPath;
}

void tst_QWidget_window::tst_windowFilePath()
{
    QFETCH(QString, filePath);
    QFETCH(QString, result);
    QFETCH(bool, again);
    QFETCH(QString, filePath2);
    QFETCH(QString, result2);

    QWidget widget;
    QCOMPARE(widget.windowFilePath(), QString());
    widget.setWindowFilePath(filePath);
    QCOMPARE(widget.windowFilePath(), result);
    if (again) {
        widget.setWindowFilePath(filePath2);
        QCOMPARE(widget.windowFilePath(), result2);
    }
}

void tst_QWidget_window::tst_showWithoutActivating()
{
#ifndef Q_WS_X11
    QSKIP("This test is X11-only.", SkipAll);
#else
    QWidget w;
    w.show();
    QTest::qWaitForWindowShown(&w);
    QApplication::processEvents();

    QApplication::clipboard();
    QLineEdit *lineEdit = new QLineEdit;
    lineEdit->setAttribute(Qt::WA_ShowWithoutActivating, true);
    lineEdit->show();
    lineEdit->setAttribute(Qt::WA_ShowWithoutActivating, false);
    lineEdit->raise();
    lineEdit->activateWindow();

    Window window;
    int revertto;
    QTRY_COMPARE(lineEdit->winId(),
                 (XGetInputFocus(QX11Info::display(), &window, &revertto), window) );
    // Note the use of the , before window because we want the XGetInputFocus to be re-executed
    //     in each iteration of the inside loop of the QTRY_COMPARE macro

#endif // Q_WS_X11
}

void tst_QWidget_window::tst_paintEventOnSecondShow()
{
    TestWidget w;
    w.show();
    w.hide();

    w.reset();
    w.show();
    QTest::qWaitForWindowShown(&w);
    QApplication::processEvents();
    QTRY_VERIFY(w.paintEventReceived);
}

QTEST_MAIN(tst_QWidget_window)
#include "tst_qwidget_window.moc"
