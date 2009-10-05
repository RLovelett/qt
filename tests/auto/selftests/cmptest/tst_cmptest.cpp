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
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtCore>
#include <QtTest/QtTest>

class tst_Cmptest: public QObject
{
    Q_OBJECT

private slots:
    void compare_boolfuncs();
    void compare_pointerfuncs();
    void compare_tostring();
    void compare_tostring_data();
    void qfuzz_compare_int();
    void qfuzz_compare_int_data();
    void qfuzz_compare_float();
    void qfuzz_compare_float_data();
    void qfuzz_compare_char();
    void qfuzz_compare_char_data();
};

static bool boolfunc() { return true; }
static bool boolfunc2() { return true; }

void tst_Cmptest::compare_boolfuncs()
{
    QCOMPARE(boolfunc(), boolfunc());
    QCOMPARE(boolfunc(), boolfunc2());
    QCOMPARE(!boolfunc(), !boolfunc2());
    QCOMPARE(boolfunc(), true);
    QCOMPARE(!boolfunc(), false);
}

static int i = 0;

static int *intptr() { return &i; }

void tst_Cmptest::compare_pointerfuncs()
{
    QCOMPARE(intptr(), intptr());
    QCOMPARE(&i, &i);
    QCOMPARE(intptr(), &i);
    QCOMPARE(&i, intptr());
}

Q_DECLARE_METATYPE(QVariant)

class PhonyClass
{};

void tst_Cmptest::compare_tostring_data()
{
    QTest::addColumn<QVariant>("actual");
    QTest::addColumn<QVariant>("expected");

    QTest::newRow("int, string")
        << QVariant::fromValue(123)
        << QVariant::fromValue(QString("hi"))
    ;

    QTest::newRow("both invalid")
        << QVariant()
        << QVariant()
    ;

    QTest::newRow("null hash, invalid")
        << QVariant(QVariant::Hash)
        << QVariant()
    ;

    QTest::newRow("string, null user type")
        << QVariant::fromValue(QString::fromLatin1("A simple string"))
        << QVariant(QVariant::Type(qRegisterMetaType<PhonyClass>("PhonyClass")))
    ;

    QTest::newRow("both non-null user type")
        << QVariant(qRegisterMetaType<PhonyClass>("PhonyClass"), (const void*)0)
        << QVariant(qRegisterMetaType<PhonyClass>("PhonyClass"), (const void*)0)
    ;
}

void tst_Cmptest::compare_tostring()
{
    QFETCH(QVariant, actual);
    QFETCH(QVariant, expected);

    QCOMPARE(actual, expected);
}


//for testing the int versions of a QFUZZ_COMPARE.
void tst_Cmptest::qfuzz_compare_int()
{
  QFETCH(int, expected);
  QFETCH(int, comparison);
  QFETCH(int, fuzz);

  QFUZZ_COMPARE(expected, comparison, fuzz);
}

void tst_Cmptest::qfuzz_compare_int_data()
{
  QTest::addColumn<int>("expected");
  QTest::addColumn<int>("comparison");
  QTest::addColumn<int>("fuzz");

  QTest::newRow("positive fuzz pass") << 1 << 2 << 1;
  QTest::newRow("positive swap pass") << 2 << 1 << 1;
  QTest::newRow("negative fuzz pass") << -1 << -2 << 1;
  QTest::newRow("negative swap pass") << -2 << -1 << 1;
  QTest::newRow("positive match pass") << 1 << 1 << 0;
  QTest::newRow("negative match pass") << -1 << -1 << 0;
  QTest::newRow("neg/pos fuzz pass") << 1 << -1 << 2;
  QTest::newRow("pos/neg swap pass") << -1 << 1 << 2;

  QTest::newRow("positive fuzz fail") << 1 << 3 << 1;
  QTest::newRow("positive swap fail") << 3 << 1 << 1;
  QTest::newRow("negative fuzz fail") << -1 << -3 << 1;
  QTest::newRow("negative swap fail") << -3 << -1 << 1;
  QTest::newRow("positive match fail") << 1 << 2 << 0;
  QTest::newRow("negative match fail") << -1 << -2 << 0;
  QTest::newRow("neg/pos fuzz fail") << 1 << -2 << 2;
  QTest::newRow("pos/neg swap fail") << -2 << 1 << 2;

  QTest::newRow("big value fuzz pass") << -10000 << 10000 << 20000;
  QTest::newRow("big value fuzz fail") << -10000 << 10000 << 19999;
}

//for testing the float versions of a QFUZZ_COMPARE.
void tst_Cmptest::qfuzz_compare_float()
{
  QFETCH(float, expected);
  QFETCH(float, comparison);
  QFETCH(float, fuzz);

  QFUZZ_COMPARE(expected, comparison, fuzz);
}

void tst_Cmptest::qfuzz_compare_float_data()
{
  QTest::addColumn<float>("expected");
  QTest::addColumn<float>("comparison");
  QTest::addColumn<float>("fuzz");

  QTest::newRow("positive fuzz pass") << 1.0f << 1.0f << 1.0f;
  QTest::newRow("positive swap pass") << 2.0f << 1.0f << 1.0f;
  QTest::newRow("negative fuzz pass") << -1.0f << -2.0f << 1.0f;
  QTest::newRow("negative swap pass") << -2.0f << -1.0f << 1.0f;
  QTest::newRow("positive match pass") << 1.0f << 1.0f << 0.0f;
  QTest::newRow("negative match pass") << -1.0f << -1.0f << 0.0f;
  QTest::newRow("neg/pos fuzz pass") << 1.0f << -1.0f << 2.0f;
  QTest::newRow("pos/neg swap pass") << -1.0f << 1.0f << 2.0f;

  QTest::newRow("positive fuzz fail") << 1.0f << 3.0f << 1.0f;
  QTest::newRow("positive swap fail") << 3.0f << 1.0f << 1.0f;
  QTest::newRow("negative fuzz fail") << -1.0f << -3.0f << 1.0f;
  QTest::newRow("negative swap fail") << -3.0f << -1.0f << 1.0f;
  QTest::newRow("positive match fail") << 1.0f << 2.0f << 0.0f;
  QTest::newRow("negative match fail") << -1.0f << -2.0f << 0.0f;
  QTest::newRow("neg/pos fuzz fail") << 1.0f << -2.0f << 2.0f;
  QTest::newRow("pos/neg swap fail") << -2.0f << 1.0f << 2.0f;

  QTest::newRow("big value fuzz pass") << -10000.0f << 10000.0f << 20000.0f;
  QTest::newRow("big value fuzz fail") << -10000.0f << 10000.0f << 19999.0f;
}

//for testing the char versions of a QFUZZ_COMPARE.
void tst_Cmptest::qfuzz_compare_char()
{
  QFETCH(char, expected);
  QFETCH(char, comparison);
  QFETCH(char, fuzz);

  QFUZZ_COMPARE(expected, comparison, fuzz);
}

void tst_Cmptest::qfuzz_compare_char_data()
{
  QTest::addColumn<char>("expected");
  QTest::addColumn<char>("comparison");
  QTest::addColumn<char>("fuzz");

  QTest::newRow("match") << 'a' << 'a' << (char) 1;
  QTest::newRow("pass") << 'a' << 'c' << (char) 2;
  QTest::newRow("fail") << 'a' << 'c' << (char) 1;
  QTest::newRow("A-z fail") << 'A' << 'z' << (char) 25;
  QTest::newRow("a-z pass") << 'a' << 'z' << (char) 25;
}

QTEST_MAIN(tst_Cmptest)

#include "tst_cmptest.moc"
