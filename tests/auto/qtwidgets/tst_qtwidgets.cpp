/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtGui>
#include <QtNetwork>

#include <QtTest/QtTest>


#include "mainwindow.h"

class tst_QtWidgets: public QObject
{
    Q_OBJECT

private slots:
    void snapshot();
};


void tst_QtWidgets::snapshot()
{
    StyleWidget widget(0, Qt::X11BypassWindowManagerHint);
    widget.show();

    QPixmap pix = QPixmap::grabWidget(&widget);

    QVERIFY(!pix.isNull());

    QBuffer buf;
    pix.save(&buf, "PNG");
    QVERIFY(buf.size() > 0);

    QString filename = "qtwidgets_" + QHostInfo::localHostName() + "_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_hh.mm.ss") + ".png";

    QFtp ftp;
    ftp.connectToHost("kramer.troll.no");
    ftp.login("anonymous");
    ftp.cd("pics");
    ftp.put(buf.data(), filename, QFtp::Binary);
    ftp.close();

    int i = 0;
    while (i < 100 && ftp.hasPendingCommands()) {
        QCoreApplication::instance()->processEvents();
        QTest::qWait(250);
        ++i;
    }
    QVERIFY2(ftp.error() == QFtp::NoError, ftp.errorString().toLocal8Bit().constData());
    QVERIFY(!ftp.hasPendingCommands());
}



QTEST_MAIN(tst_QtWidgets)

#include "tst_qtwidgets.moc"
