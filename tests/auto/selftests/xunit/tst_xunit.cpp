/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>

class tst_Xunit : public QObject
{
    Q_OBJECT

public:
    tst_Xunit();

private slots:
    void testFunc1();
    void testFunc2();
    void testFunc3();
    void testFunc4();
    void testFunc5();
    void testFunc6();
    void testFunc7();
    void testFunc8();
};

tst_Xunit::tst_Xunit()
{
}

void tst_Xunit::testFunc1()
{
    QWARN("just a QWARN() !");
    QCOMPARE(1,1);
}

void tst_Xunit::testFunc2()
{
    qDebug("a qDebug() call with comment-ending stuff -->");
    QCOMPARE(2, 3);
}

void tst_Xunit::testFunc3()
{
    QSKIP("skipping this function!", SkipAll);
}

void tst_Xunit::testFunc4()
{
    QFAIL("a forced failure!");
}

/*
    Note there are two testfunctions which give expected failures.
    This is so we can test that expected failures don't add to failure
    counts and unexpected passes do.  If we had one xfail and one xpass
    testfunction, we couldn't test which one of them adds to the failure
    count.
*/

void tst_Xunit::testFunc5()
{
    QEXPECT_FAIL("", "this failure is expected", Abort);
    QVERIFY(false);
}

void tst_Xunit::testFunc6()
{
    QEXPECT_FAIL("", "this failure is also expected", Abort);
    QVERIFY(false);
}

void tst_Xunit::testFunc7()
{
    QEXPECT_FAIL("", "this pass is unexpected", Abort);
    QVERIFY(true);
}

void tst_Xunit::testFunc8()
{
  const int integerTwo = 2;
  const int compareIntegerTwo = 1+1;
  const int compareIntegerThree = 3;
  const int compareIntegerFour = 4;
  
  const float floatTwo = 2.0f;
  const float compareFloatTwo = 1.0f + 1.0f;
  const float compareFloatThree = 3.0f;
  const float compareFloatFour = 4.0f;
 
  const double doubleTwo = 2.0;
  const double compareDoubleTwo = 1.0 + 1.0f;
  const double compareDoubleThree = 3.0;
  const double compareDoubleFour = 4.0;

  QCOMPARE( integerTwo, compareIntegerTwo );
  QCOMPARE( floatTwo, compareFloatTwo );
  QCOMPARE( doubleTwo, compareDoubleTwo );

  QEXPECT_FAIL( "", "Testing correct failure condition of QCOMPARE...",
		     Continue );
  QCOMPARE( integerTwo, compareIntegerThree );
  QEXPECT_FAIL( "", "Testing correct failure condition of QCOMPARE...",
		    Continue );
  QCOMPARE( floatTwo, compareFloatThree );
  QEXPECT_FAIL( "", "Testing correct failure condition of QCOMPARE...",
		    Continue );
  QCOMPARE( doubleTwo, compareDoubleThree );
  
  QFUZZ_COMPARE( integerTwo, compareIntegerTwo, 0 );
  QFUZZ_COMPARE( floatTwo, compareFloatTwo, 0.0f );
  QFUZZ_COMPARE( doubleTwo, compareDoubleTwo, 0.0 );
  
  QFUZZ_COMPARE( integerTwo, compareIntegerThree, 1 );
  QFUZZ_COMPARE( floatTwo, compareFloatThree, 1.0f );
  QFUZZ_COMPARE( doubleTwo, compareDoubleThree, 1.0 );
  
  QEXPECT_FAIL( "", "Testing correct failure condition of QFUZZ_COMPARE...", Continue );
  QFUZZ_COMPARE( integerTwo, compareIntegerFour, 1 );
  QEXPECT_FAIL( "", "Testing correct failure condition of QFUZZ_COMPARE...", Continue );
  QFUZZ_COMPARE( floatTwo, compareFloatFour, 1.0f );
  QEXPECT_FAIL( "", "Testing correct failure condition of QFUZZ_COMPARE...", Continue );
  QFUZZ_COMPARE( doubleTwo, compareDoubleFour, 1.0 );
}


QTEST_APPLESS_MAIN(tst_Xunit)
#include "tst_xunit.moc"
