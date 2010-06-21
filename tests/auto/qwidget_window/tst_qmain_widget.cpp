/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QTimer>
#include <string.h>

#include <qlistwidget.h>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <QX11Info>
#include <unistd.h>
#endif // Q_WS_X11

// Display a widget, printing the location it is displayed.
// This is to test the X11 -geometry command line argument.
// output format is "Width Height X Y"
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
#ifdef Q_WS_X11
    // default to quering the X-server directly, but allow an override
    bool query_qt = false;
    for (int i = 0; i < argc; ++i)
        if (!strcmp(argv[i], "--query-qt"))
            query_qt = true;
#else
    // must use qt to get the geometry
    bool query_qt = true;
#endif

    QWidget w;
    w.show();
    QTimer timer;
    w.connect(&timer, SIGNAL(timeout()), &app, SLOT(quit()));
    timer.start(2000);
    app.exec();

    if (query_qt) {
        QRect rect = w.frameGeometry();
        printf("frame  %d %d %d %d\n", rect.width(), rect.height(), rect.x(), rect.y());
        rect = w.geometry();
        printf("widget %d %d %d %d\n", rect.width(), rect.height(), rect.x(), rect.y());
        return 0;
    }

#ifdef Q_WS_X11
    // Granted there's a race condition between Qt setting up and requesting the
    // window be mapped, X Server, and Window manager giving the final
    // window location and size.  But even after waiting something like
    // 2 seconds w.frameGeometry() and w.geometry() still give the same
    // answer, even if it's wrong and I can't find any way to tell Qt to
    // go back to the X server to find out what the real positions are.
    // If the border width is 4 and title height is 23 and -geometry +0+0
    // is given sometimes frameGeometry() comes back with 0, 0 geometry 4, 23
    // and sometimes it's -4, -23, and 0, 0.
    Display *dpy = QX11Info::display();
    Window parent;
    Window child = w.internalWinId();
    Window root;
    Window *children;
    unsigned int nchildren;
    XWindowAttributes pattr;
    XWindowAttributes cattr;
    if (!child || !XGetWindowAttributes(dpy, child, &cattr))
        return 1;

    // Query the X server directly.  Window positions are cummulative.  Assume
    // all windows between the widget and the window that is offset from the
    // screen 0, 0 point have all have the same width and height.  If that's
    // not true this won't work properly, it would be better to use a toolkit
    // like Qt to get this information, O-wait, see above.
    parent = child;
    do {
        if (!XQueryTree(dpy, parent, &root, &parent, &children, &nchildren)
            || !parent)
            return 1;
        //printf("Window %lx parent %lx\n", child, parent);
        if (children)
            XFree(children);

        if (!XGetWindowAttributes(dpy, parent, &pattr))
            return 1;
        /*
        printf("frame  %d %d %d %d\n", pattr.width, pattr.height,
            pattr.x, pattr.y);
        */
        cattr.x += pattr.x;
        cattr.y += pattr.y;
    } while (pattr.width == cattr.width && pattr.height == cattr.height);

    printf("frame  %d %d %d %d\n", pattr.width, pattr.height, pattr.x, pattr.y);
    printf("widget %d %d %d %d\n", cattr.width, cattr.height, cattr.x, cattr.y);
#endif // Q_WS_X11

    return 0;
}
