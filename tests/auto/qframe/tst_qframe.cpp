/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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


#include <QWindowsStyle>
#include <QtTest/QtTest>

#include <qpixmap.h>

#include <qimagereader.h>
#include <qimagewriter.h>

#include <qframe.h>

//TESTED_CLASS=QFrame
//TESTED_FILES=gui/widgets/qframe.cpp

class tst_QFrame : public QObject
{
    Q_OBJECT
public:
    tst_QFrame();
    virtual ~tst_QFrame();
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testPainting_data();
    void testPainting();

    void testDefaults();

private:
};


tst_QFrame::tst_QFrame()
{
}

tst_QFrame::~tst_QFrame()
{
}

void tst_QFrame::init()
{
}

void tst_QFrame::cleanup()
{
}

void tst_QFrame::initTestCase()
{
}

void tst_QFrame::cleanupTestCase()
{
}

void writeImage(const QString &fileName, QImage image)
{
    QImageWriter imageWriter(fileName);
    imageWriter.setFormat("png");
    qDebug() << "result " << imageWriter.write(image);
}

QImage readImage(const QString &fileName)
{
    QImageReader reader(fileName);
    return reader.read();
}

void comparePixmap(const QString &filename, const QPixmap &pixmap)
{
    QImage oldFile = readImage(filename);
    QPixmap oldPixmap = QPixmap::fromImage(oldFile);
    if (!oldFile.isNull())
        QVERIFY(pixmapsAreEqual(&pixmap, &oldPixmap));
    else
        writeImage(filename, pixmap.toImage());
}

Q_DECLARE_METATYPE(QFrame::Shape)
Q_DECLARE_METATYPE(QFrame::Shadow)

void tst_QFrame::testDefaults()
{
    QFrame frame;
    QCOMPARE(frame.frameStyle(), int(QFrame::NoFrame | QFrame::Plain));
    frame.setFrameShape(QFrame::Box);
    QCOMPARE(frame.frameStyle(), int(QFrame::Box | QFrame::Plain));
    frame.setFrameStyle(QFrame::Box); // no shadow specified!
    QCOMPARE(frame.frameStyle(), int(QFrame::Box));
}

void tst_QFrame::testPainting_data()
{
     QTest::addColumn<QString>("basename");
     QTest::addColumn<QFrame::Shape>("shape");
     QTest::addColumn<QFrame::Shadow>("shadow");

     QTest::newRow("box_noshadow") << "box_noshadow" << QFrame::Box << (QFrame::Shadow)0;
     QTest::newRow("box_plain") << "box_plain" << QFrame::Box << QFrame::Plain;
     QTest::newRow("box_raised") << "box_raised" << QFrame::Box << QFrame::Raised;
     QTest::newRow("box_sunken") << "box_sunken" << QFrame::Box << QFrame::Sunken;

     QTest::newRow("winpanel_noshadow") << "winpanel_noshadow" << QFrame::WinPanel << (QFrame::Shadow)0;
     QTest::newRow("winpanel_plain") << "winpanel_plain" << QFrame::WinPanel << QFrame::Plain;
     QTest::newRow("winpanel_raised") << "winpanel_raised" << QFrame::WinPanel << QFrame::Raised;
     QTest::newRow("winpanel_sunken") << "winpanel_sunken" << QFrame::WinPanel << QFrame::Sunken;
}

void tst_QFrame::testPainting()
{
    QFETCH(QString, basename);
    QFETCH(QFrame::Shape, shape);
    QFETCH(QFrame::Shadow, shadow);

    for (int lw = 0; lw < 3; ++lw) {
        for (int mlw = 0; mlw < 3; ++mlw) {

            const QString fileName = "images/" + basename + QString::number(lw) + QString::number(mlw) + ".png";

            QFrame frame;
            frame.setStyle(new QWindowsStyle());
            frame.setFrameStyle(shape | shadow);
            frame.setLineWidth(lw);
            frame.setMidLineWidth(mlw);
            frame.resize(16, 16);

            QPixmap pixmap = QPixmap::grabWidget(&frame);
            comparePixmap(fileName, pixmap);
        }
    }
}

QTEST_MAIN(tst_QFrame)

#include "tst_qframe.moc"
