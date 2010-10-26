/****************************************************************************
**
** Copyright (C) 2011 Andre Somers, Sean Harmer.
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
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

#include "tst_qtimespan.h"

//comment out the line below to make non-implemented tests or tests that could use more cases pass instead of fail
//#define DEFAULTFALSE QVERIFY(false);
//#define DEFAULTFALSE QSKIP("Test not implemented yet", SkipAll);
#ifndef DEFAULTFALSE
#define DEFAULTFALSE
#endif

TestQTimeSpan::TestQTimeSpan()
{
}

TestQTimeSpan::~TestQTimeSpan()
{

}

void TestQTimeSpan::initTestCase()
{
}

void TestQTimeSpan::cleanupTestCase()
{
}

void TestQTimeSpan::init()
{
}

void TestQTimeSpan::cleanup()
{
}

void TestQTimeSpan::constructionEmpty()
{
    //test default constructor
    QTimeSpan s1;
    QCOMPARE(s1.referenceDate(), QDateTime());
    QCOMPARE(s1.toMSecs(), 0LL);
    QCOMPARE(s1.hasValidReference(), false);
    QCOMPARE(s1.isNull(), true);
    QCOMPARE(s1.isEmpty(), true);

    //test msecs constructor
    /* //moved to constructionNonEmpty
    QTimeSpan s2(1000);
    QCOMPARE(s2.hasValidReference(), false);
    QCOMPARE(s2.toMSecs(), 1000LL);
    QCOMPARE(s2.isNull(), false);
    QCOMPARE(s2.isEmpty(), false);
    */

    QDate date(1975,10,10);
    QTime time(2,5,0,0);
    QDateTime dateTime(date, time);

    //test date constructor
    QTimeSpan s3(date);
    QCOMPARE(s3.hasValidReference(), true);
    QCOMPARE(s3.isEmpty(), true);
    QCOMPARE(s3.referenceDate().date(), date);
    QCOMPARE(s3.referenceDate().time(), QTime(0,0,0));
    QCOMPARE(s3.toMSecs(), 0LL);

    //test time constructor
    QTimeSpan s4(time);
    QCOMPARE(s4.hasValidReference(), true);
    QCOMPARE(s4.isEmpty(), true);
    QCOMPARE(s4.referenceDate().date(), QDate::currentDate());
    QCOMPARE(s4.referenceDate().time(), time);
    QCOMPARE(s4.toMSecs(), 0LL);

    //test datetime constructor
    QTimeSpan s5(dateTime);
    QCOMPARE(s5.hasValidReference(), true);
    QCOMPARE(s5.isEmpty(), true);
    QCOMPARE(s5.referenceDate(), dateTime);
    QCOMPARE(s5.toMSecs(), 0LL);


}

void TestQTimeSpan::constructionNonEmpty()
{
    QDate date(1975,10,10);
    QTime time(2,5,0,0);
    QDateTime dateTime(date, time);

    //test length only constructor
    QTimeSpan s1(1000);
    QCOMPARE(s1.hasValidReference(), false);
    QCOMPARE(s1.toMSecs(), 1000LL);
    QCOMPARE(s1.isNull(), false);
    QCOMPARE(s1.isEmpty(), false);

    //test date constructor
    QTimeSpan s2(date, 1000);
    QCOMPARE(s2.hasValidReference(), true);
    QCOMPARE(s2.referenceDate().date(), date);
    QCOMPARE(s2.toMSecs(), 1000LL);
    QCOMPARE(s2.isNull(), false);
    QCOMPARE(s2.isEmpty(), false);

    //test time constructor
    QTimeSpan s4(time, 1000);
    QCOMPARE(s4.hasValidReference(), true);
    QCOMPARE(s4.isEmpty(), false);
    QCOMPARE(s4.referenceDate().date(), QDate::currentDate());
    QCOMPARE(s4.referenceDate().time(), time);
    QCOMPARE(s4.toMSecs(), 1000LL);

    //test datetime constructor
    QTimeSpan s5(dateTime, 1000);
    QCOMPARE(s5.hasValidReference(), true);
    QCOMPARE(s5.isEmpty(), false);
    QCOMPARE(s5.referenceDate(), dateTime);
    QCOMPARE(s5.toMSecs(), 1000LL);

    QTimeSpan ref(QDateTime::currentDateTime(), 5000);

    //test date constructor with existing QTimeSpan
    QTimeSpan s6(date, ref);
    QCOMPARE(s6.hasValidReference(), true);
    QCOMPARE(s6.referenceDate().date(), date);
    QCOMPARE(s6.toMSecs(), 5000LL);
    QCOMPARE(s6.isNull(), false);
    QCOMPARE(s6.isEmpty(), false);

    //test time constructor with existing QTimeSpan
    QTimeSpan s7(time, ref);
    QCOMPARE(s7.hasValidReference(), true);
    QCOMPARE(s7.isEmpty(), false);
    QCOMPARE(s7.referenceDate().date(), QDate::currentDate());
    QCOMPARE(s7.referenceDate().time(), time);
    QCOMPARE(s7.toMSecs(), 5000LL);

    //test datetime constructor with existing QTimeSpan
    QTimeSpan s8(dateTime, ref);
    QCOMPARE(s8.hasValidReference(), true);
    QCOMPARE(s8.isEmpty(), false);
    QCOMPARE(s8.referenceDate(), dateTime);
    QCOMPARE(s8.toMSecs(), 5000LL);

}

void TestQTimeSpan::msecsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::secsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::minutesPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::hoursPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::daysPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::weeksPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::monthsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::yearsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::part()
{
    DEFAULTFALSE
}

void TestQTimeSpan::parts()
{
    QDateTime dt1(QDate(1975, 10, 10), QTime(2, 0, 0));
    QDateTime dt2(QDate(2010, 9, 9), QTime(15, 45, 0));
    QTimeSpan s1 = dt2 - dt1;

    // qDebug() << s1.toMSecs() << s1.referenceDate() << s1.referencedDate();

    int years = 0;
    int months = 0;
    int weeks = 0;
    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int milliseconds = 0;
    qreal fractionalPart = 0.0;

    bool result = s1.parts(0, 0, 0, 0, &days, 0, &months, &years);
    QCOMPARE(result, true);
    qDebug() << years << "years" << months << "months" << days << "days";
    result = s1.parts(0, 0, 0, 0, &days, 0, &months, &years, &fractionalPart);
    QCOMPARE(result, true);
    qDebug() << years << "years" << months << "months" << fractionalPart << "days";

    result = s1.parts(0, 0, 0, 0, &days, &weeks, &months, &years);
    QCOMPARE(result, true);
    qDebug() << years << "years" << months << "months" << weeks << "weeks" << days << "days";
    result = s1.parts(0, 0, 0, 0, &days, &weeks, &months, &years, &fractionalPart);
    QCOMPARE(result, true);
    qDebug() << years << "years" << months << "months" << weeks << "weeks" << fractionalPart << "days";

    result = s1.parts(0, 0, &minutes, &hours, &days, &weeks, &months, &years);
    QCOMPARE(result, true);
    qDebug() << years << "years" << months << "months" << weeks << "weeks"
             << days << "days" << hours << "hours" << minutes << "minutes";
    // qDebug() << years << months << weeks << days << hours << minutes;
    result = s1.parts(0, 0, &minutes, &hours, &days, &weeks, &months, &years, &fractionalPart);
    QCOMPARE(result, true);
    qDebug() << years << "years" << months << "months" << weeks << "weeks"
             << days << "days" << hours << "hours" << fractionalPart << "minutes";

    //test not finished, so should fail if unimplemented tests fail
    Q_UNUSED(seconds)
    Q_UNUSED(milliseconds)
    DEFAULTFALSE

}

void TestQTimeSpan::magnitude_data()
{
    QTest::addColumn<QTimeSpan>("span");
    QTest::addColumn<Qt::TimeSpanUnit>("unit");

    QTest::newRow("default construcuted") << QTimeSpan() << Qt::Milliseconds;
    QTest::newRow("no ref 156 ms") << QTimeSpan(156) << Qt::Milliseconds;
    QTest::newRow("no ref 999 ms") << QTimeSpan(999) << Qt::Milliseconds;
    QTest::newRow("no ref 1000 ms") << QTimeSpan(1000) << Qt::Seconds;
    QTest::newRow("no ref 5000 ms") << QTimeSpan(5000) << Qt::Seconds;
    QTest::newRow("no ref 1 m") << QTimeSpan::minute() << Qt::Minutes;
    QTest::newRow("no ref 59 m") << QTimeSpan::minute() * 59 << Qt::Minutes;
    QTest::newRow("no ref 1 h") << QTimeSpan::hour() << Qt::Hours;
    QTest::newRow("no ref 23:59:59.999 h") << QTimeSpan(QTimeSpan::day().toMSecs() - 1) << Qt::Hours;
    QTest::newRow("no ref 1 d") << QTimeSpan::day() << Qt::Days;
    QTest::newRow("no ref 1 w") << QTimeSpan::week() << Qt::Weeks;
    QTest::newRow("no ref 6 w") << QTimeSpan::week() * 6 << Qt::Weeks;
    QTest::newRow("no ref 60 w") << QTimeSpan::week() * 60 << Qt::Weeks;

    QTest::newRow("ref default construcuted") << QTimeSpan(QDate::currentDate()) << Qt::Milliseconds;
    QTest::newRow("ref 156 ms") << QTimeSpan(QDate::currentDate(), 156) << Qt::Milliseconds;
    QTest::newRow("ref 999 ms") << QTimeSpan(QDate::currentDate(), 999) << Qt::Milliseconds;
    QTest::newRow("ref 1000 ms") << QTimeSpan(QDate::currentDate(), 1000) << Qt::Seconds;
    QTest::newRow("ref 5000 ms") << QTimeSpan(QDate::currentDate(), 5000) << Qt::Seconds;
    QTest::newRow("ref 1 m") << QTimeSpan(QDate::currentDate(), QTimeSpan::minute()) << Qt::Minutes;
    QTest::newRow("ref 59 m") << QTimeSpan(QDate::currentDate(), QTimeSpan::minute() * 59) << Qt::Minutes;
    QTest::newRow("ref 1 h") << QTimeSpan(QDate::currentDate(), QTimeSpan::hour()) << Qt::Hours;
    QTest::newRow("ref 23:59:59.999 h") << QTimeSpan(QDate::currentDate(), QTimeSpan::day().toMSecs() - 1) << Qt::Hours;
    QTest::newRow("ref 1 d") << QTimeSpan(QDate::currentDate(), QTimeSpan::day()) << Qt::Days;
    QTest::newRow("ref 1 w") << QTimeSpan(QDate::currentDate(), QTimeSpan::week()) << Qt::Weeks;
    QTest::newRow("ref 6 w") << QTimeSpan(QDate::currentDate(), QTimeSpan::week() * 6) << Qt::Months;
    QTest::newRow("ref 60 w") << QTimeSpan(QDate::currentDate(), QTimeSpan::week() * 60) << Qt::Years;


}

void TestQTimeSpan::magnitude()
{
    QFETCH(QTimeSpan, span);
    QFETCH(Qt::TimeSpanUnit, unit);

    //qDebug() << span.magnitude() << unit;
    QCOMPARE(span.magnitude(), unit);
}

void TestQTimeSpan::setMSecsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setSecsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setMinutesPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setHoursPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setDaysPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setWeeksPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setMonthsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setYearsPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setPart()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toMSecs()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toSecs()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toMinutes()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toHours()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toDays()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toWeeks()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toMonths()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toYears()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toTimeUnit()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setFromMSecs()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setFromSecs()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setFromMinutes()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setFromHours()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setFromDays()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setFromWeeks()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setFromMonths()
{
    //we can not test an assert using a unit test
    //QEXPECT_FAIL("", "Can not set interval from time unit month if there is no reference date.", Continue);
    //QTimeSpan s1;
    //s1.setFromMonths(1.5);

    QTimeSpan s2(QDate(2010,1,1));
    QTimeSpan s3(QDate(2008,1,1)); //leap year!
    s2.setFromMonths(1.5);
    s3.setFromMonths(1.5);

    QCOMPARE(s2.endDate(), QDateTime(QDate(2010,2,15)));
    QCOMPARE(s3.endDate(), QDateTime(QDate(2008,2,15), QTime(12,0,0))); //februari is one day longer, so we end up 12 hours further

    DEFAULTFALSE
}

void TestQTimeSpan::setFromYears()
{
    QTimeSpan s2(QDateTime(QDate(2010,1,1), QTime(0,0,0), Qt::UTC));
    QTimeSpan s3(QDateTime(QDate(2008,1,1), QTime(0,0,0), Qt::UTC)); //leap year!
    s2.setFromYears(0.5);
    s3.setFromYears(0.5);

    QCOMPARE(s2.endDate(), QDateTime(QDate(2010,7,2), QTime(12,0,0), Qt::UTC));
    QCOMPARE(s3.endDate(), QDateTime(QDate(2008,7,2), QTime( 0,0,0), Qt::UTC)); //februari is one day longer, so we end up 12 hours earlier: the middle of the year is half a day further, but we have 24 hours extra at februari 29 already

    QTimeSpan s4(QDateTime(QDate(2010,1,1), QTime(0,0,0), Qt::UTC));
    QTimeSpan s5(QDateTime(QDate(2008,1,1), QTime(0,0,0), Qt::UTC)); //leap year!
    QTimeSpan s6(QDateTime(QDate(2009,1,1), QTime(0,0,0), Qt::UTC)); //jumping into a leap year
    s4.setFromYears(-0.5);
    s5.setFromYears(-0.5);
    s6.setFromYears(-0.5);

    QCOMPARE(s4.referencedDate(), QDateTime(QDate(2009,7,2), QTime(12,0,0), Qt::UTC));
    QCOMPARE(s5.referencedDate(), QDateTime(QDate(2007,7,2), QTime(12,0,0), Qt::UTC)); //leap year should not influence this date at all!
    QCOMPARE(s6.referencedDate(), QDateTime(QDate(2008,7,2), QTime( 0,0,0), Qt::UTC)); //februari is one day longer, so we end up 12 hours earlier: the middle of the year is half a day further, but we have 24 hours extra at februari 29 already

    DEFAULTFALSE
}

void TestQTimeSpan::setFromTimeUnit()
{
    DEFAULTFALSE
}

void TestQTimeSpan::hasValidReferenceDate()
{
    QTimeSpan s1(6000);
    QTimeSpan s2(QDate::currentDate(), 0);
    QTimeSpan s3(QTime::currentTime(), -6000);
    QTimeSpan s4(QDateTime::currentDateTime(), 6000);

    QVERIFY(!s1.hasValidReference());
    QVERIFY(s2.hasValidReference());
    QVERIFY(s3.hasValidReference());
    QVERIFY(s4.hasValidReference());

    s2.setReferenceDate(QDateTime()); //set an invalid reference date
    QVERIFY(!s2.hasValidReference());
}

void TestQTimeSpan::referenceDate()
{
    QTimeSpan s1;
    QTimeSpan s2(3452);
    QDateTime dt = QDateTime::currentDateTime();
    QTimeSpan s3(dt);

    QCOMPARE(s1.referenceDate(), QDateTime());
    QCOMPARE(s2.referenceDate(), QDateTime());
    QCOMPARE(s3.referenceDate(), dt);
}


void TestQTimeSpan::startAndEndDate()
{
    QTimeSpan s1(QDate(1975,10,10), QTimeSpan::day() * -2);
    QTimeSpan s2(QDate(1975,10,10), QTimeSpan::day() *  2);

    QDate d1(1975, 10,  8);
    QDate d2(1975, 10, 10);
    QDate d3(1975, 10, 12);

    QCOMPARE(s1.startDate().date() , d1 );
    QCOMPARE(s2.startDate().date() , d2 );
    QCOMPARE(s1.endDate().date() , d2 );
    QCOMPARE(s2.endDate().date() , d3);
}

void TestQTimeSpan::referencedDate()
{
    QTimeSpan s1(QDate(1975,10,10), QTimeSpan::day() * -2);
    QTimeSpan s2(QDate(1975,10,10), QTimeSpan::day() * 2);
    QTimeSpan s3;

    QDate d1(1975, 10,  8);
    QDate d2(1975, 10, 10);
    QDate d3(1975, 10, 12);

    QCOMPARE(s1.referencedDate().date(), d1);
    QCOMPARE(s2.referencedDate().date(), d3);
    QCOMPARE(s3.referencedDate(), QDateTime()); //test with invalid reference date
    Q_UNUSED(d2);
}

void TestQTimeSpan::setReferenceDate()
{
    //basic testing
    QTimeSpan s1(1000);
    QDate d2 = QDate(1975,10,10);
    QTimeSpan s2(d2, 2000);
    QCOMPARE(s1.hasValidReference(), false);
    QCOMPARE(s2.hasValidReference(), true);

    s1.setReferenceDate(s2.referenceDate());
    s2.setReferenceDate(QDateTime());
    QCOMPARE(s1.hasValidReference(), true);
    QCOMPARE(s2.hasValidReference(), false);

    QCOMPARE(s1.referenceDate().date(), d2);
    QCOMPARE(s1.toMSecs(), 1000LL);
    QCOMPARE(s2.toMSecs(), 2000LL);

    //setting the reference date without there already being one is the same as moving
    QTimeSpan s3 = 2 * QTimeSpan::week();
    QTimeSpan s4 = s3;
    QDateTime dt = QDateTime::currentDateTime();
    s3.moveReferenceDate(dt);
    s4.setReferenceDate(dt);
    QCOMPARE(s3, s4);

    //setting the reference date if there already is one should not affect the current
    //referenced date
    QTimeSpan s5 = dt.date() - d2;
    QTimeSpan s6 = s5;
    s5.setReferenceDate(QDateTime(dt.date())); // should result in a QTimeSpan of length 0
    QCOMPARE(s5.toMSecs(), 0LL);
    QCOMPARE(s5.referencedDate(), s6.referencedDate());

}

void TestQTimeSpan::moveReferenceDate()
{
    DEFAULTFALSE
}

void TestQTimeSpan::setReferencedDate()
{
    DEFAULTFALSE
}

void TestQTimeSpan::moveReferencedDate()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toString()
{
    QTimeSpan ts = QDateTime::currentDateTime() - QDateTime(QDate(1975, 10, 10), QTime(1,45));
    ts.setMSecsPart(0, Qt::DaysAndTime | Qt::Milliseconds);
    qDebug() << ts.toString("y' years, 'M' months, 'ddd' days, 'hh' hours, 'mm' minutes, 'ss' seconds ('dd' days)'");
    QString string = ts.toString("y' years, 'M 'months, 'dd' days, 'hh:mm:ss");
    qDebug() << string;

    QTimeSpan ts2 = QTimeSpan::fromString(string, "yy' years, 'M 'months, 'dd' days, 'hh:mm:ss", ts.referenceDate());
    qDebug() << "ts" << ts;
    qDebug() << "ts2" << ts2;

    QRegExp pattern("(\\d+)\\D+(\\d+)\\D+(\\d+)\\D+(\\d+)\\D+(\\d+)\\D+(\\d+)");
    QTimeSpan ts3 = QTimeSpan::fromString(string, pattern, ts.referenceDate(), Qt::Years, Qt::Months, Qt::Days, Qt::Hours, Qt::Minutes, Qt::Seconds);
    qDebug() << "ts3" << ts3;

    QCOMPARE(ts, ts2);
    QCOMPARE(ts, ts3);

    DEFAULTFALSE
}

void TestQTimeSpan::fromString()
{
    DEFAULTFALSE
}

void TestQTimeSpan::toApproximateString()
{
    QTimeSpan ts = QTimeSpan::hour() * 3 + QTimeSpan::minute() * 37 + QTimeSpan::second() * 12;
    QCOMPARE(ts.toApproximateString(), QString("3 hour(s)"));
    QCOMPARE(ts.toApproximateString(5), QString("3 hour(s), 37 minute(s)"));
    QCOMPARE(ts.toApproximateString(-1, Qt::Minutes | Qt::Seconds), QString("217 minute(s), 12 second(s)"));

    QTimeSpan ts2 = QTimeSpan::fromTimeUnit(Qt::Years, 31.876352, QDateTime::currentDateTime());
    QCOMPARE(ts2.toApproximateString(), QString("1663 week(s)"));
    QCOMPARE(ts2.toApproximateString(3, Qt::AllUnits), QString("31 year(s)"));
    QCOMPARE(ts2.toApproximateString(40, Qt::AllUnits), QString("31 year(s), 10 month(s)"));
    QCOMPARE(ts2.toApproximateString(1, Qt::Months), QString("382 month(s)"));

    //the above is only a minimal test
    DEFAULTFALSE
}

/*
// disabled because they are commented out from QTimeSpan itself at the moment
void TestQTimeSpan::fromMSecs()
{
    DEFAULTFALSE
}

void TestQTimeSpan::fromSecs()
{
    DEFAULTFALSE
}

void TestQTimeSpan::fromMinutes()
{
    DEFAULTFALSE
}

void TestQTimeSpan::fromHours()
{
    DEFAULTFALSE
}

void TestQTimeSpan::fromDays()
{
    DEFAULTFALSE
}

void TestQTimeSpan::fromWeeks()
{
    DEFAULTFALSE
}

void TestQTimeSpan::fromMonths()
{
    DEFAULTFALSE
}

void TestQTimeSpan::fromYears()
{
    DEFAULTFALSE
}
*/

void TestQTimeSpan::fromTimeUnit()
{
    QDateTime cur = QDateTime::currentDateTime();
    QTimeSpan s1 = QTimeSpan::fromTimeUnit(Qt::Hours, 2.5);
    QTimeSpan s2 = QTimeSpan::fromTimeUnit(Qt::Hours, 2.5, cur);
    QTimeSpan s3 = QTimeSpan::fromTimeUnit(Qt::Months, 3, cur);
    QTimeSpan s4 = QTimeSpan::fromTimeUnit(Qt::Years, 2, cur);

    QTimeSpan s5(QTimeSpan::hour() * 2.5);

    QVERIFY(s1.matchesLength(s5));
    QVERIFY(s1.matchesLength(s2));
    QVERIFY(s1 == s5);
    QVERIFY(s1 != s2);
    QCOMPARE(s1.toMSecs(), 2500LL * 60LL * 60LL);

    QDate d = cur.date();
    QDate d2 = d.addMonths(3);
    QDate d3 = d.addYears(2);

    QCOMPARE(s3.referencedDate().date(), d2);
    QCOMPARE(s4.referencedDate().date(), d3);

}

void TestQTimeSpan::operator_eq()
{
    //not sure how to test this properly. Is the below a reasonable test?

    QTimeSpan s1(QDateTime::currentDateTime(), QTimeSpan::hour() * 6);
    QTimeSpan s2(QDate(2010,9,9), QTimeSpan::minute() * 45);

    QCOMPARE(s1 == s2, false);

    QTimeSpan s3 = s1; //test assignment in constructor
    QCOMPARE(s1, s3);
    QCOMPARE(s3, s1);

    s3 = s2; //test normal assignment
    QCOMPARE(s1 == s3, false);
    QCOMPARE(s2, s3);
    QCOMPARE(s3, s2);

    s3 = s3; //self assignment test
    QCOMPARE(s2, s3);
}

void TestQTimeSpan::operator_eq_eq_data()
{
    //data also used for testing matchesLength()
    QTimeSpan s1;
    QTimeSpan s2;
    QTimeSpan s3(2000);
    QTimeSpan s4(5000);
    QTimeSpan s5(5000);
    QTimeSpan s6(QDate::currentDate(), 5000);
    QTimeSpan s7(QDate::currentDate(), 5000);
    QTimeSpan s8(QDate(1975,10,10), 5000);
    QTimeSpan s9(-5000);

    QTest::addColumn<QTimeSpan>("span1");
    QTest::addColumn<QTimeSpan>("span2");
    QTest::addColumn<bool>("result_eq_eq");
    QTest::addColumn<bool>("result_matchesLength");
    QTest::addColumn<bool>("result_matchesLengthNormal");

    QTest::newRow("default construcuted") << s1 << s2 << true << true << true;
    QTest::newRow("default construcuted vs length") << s1 << s3 << false << false << false;
    QTest::newRow("length vs length (different)") << s3 << s4 << false << false << false;
    QTest::newRow("date vs date (same)") << s6 << s7 << true << true << true;
    QTest::newRow("length vs date (different)") << s5 << s6 << false << true << true;
    QTest::newRow("length vs negative length") << s4 << s9 << false << false << true;
    QTest::newRow("different dates") << s6 << s8 << false << true << true;
}

void TestQTimeSpan::operator_eq_eq()
{
    QFETCH(QTimeSpan, span1);
    QFETCH(QTimeSpan, span2);
    QFETCH(bool, result_eq_eq);
    QFETCH(bool, result_matchesLength);

    QCOMPARE(span1 == span2, result_eq_eq);
    Q_UNUSED(result_matchesLength);
}

void TestQTimeSpan::matchesLength_data()
{
    operator_eq_eq_data();
}

void TestQTimeSpan::matchesLength()
{
    QFETCH(QTimeSpan, span1);
    QFETCH(QTimeSpan, span2);
    QFETCH(bool, result_eq_eq);
    QFETCH(bool, result_matchesLength);
    QFETCH(bool, result_matchesLengthNormal);

    QCOMPARE(span1.matchesLength(span2), result_matchesLength);
    QCOMPARE(span1.matchesLength(span2, true), result_matchesLengthNormal);

    Q_UNUSED(result_eq_eq);

}

void TestQTimeSpan::operator_not_eq_data()
{
    operator_eq_eq_data();
}

void TestQTimeSpan::operator_not_eq()
{
    QFETCH(QTimeSpan, span1);
    QFETCH(QTimeSpan, span2);
    QFETCH(bool, result_eq_eq);
    QFETCH(bool, result_matchesLength);

    QCOMPARE(span1 != span2, !result_eq_eq);
    Q_UNUSED(result_matchesLength)
}

void TestQTimeSpan::operator_lt_data()
{
    QTest::addColumn<QTimeSpan>("span1");
    QTest::addColumn<QTimeSpan>("span2");
    QTest::addColumn<bool>("lt");
    QTest::addColumn<bool>("lt_eq");
    QTest::addColumn<bool>("gt");
    QTest::addColumn<bool>("gt_eq");

    QTimeSpan s0();
    QTimeSpan s1(-1);
    QTimeSpan s2(0);
    QTimeSpan s3(1);
    QTimeSpan s4(3000);
    QTimeSpan s5 = 24 * QTimeSpan::hour();

    QList<QTimeSpan> spans;
    spans << s1 << s1 << s2 << s3 << s4 << s5;
    int i = 0;
    foreach (QTimeSpan s, spans) {
        QTest::newRow("Self comparison") << s << s << false << true << false << true;
        if (i>1) {
            QTest::newRow("Compare left < right") << spans[i-1] << s << true << true << false << false;
            QTest::newRow("Compare right > left") << s << spans[i-1] << false << false << true << true;
        }
    }
}

void TestQTimeSpan::operator_lt_eq_data()
{
    operator_lt_data();
}

void TestQTimeSpan::operator_gt_data()
{
    operator_lt_data();
}

void TestQTimeSpan::operator_gt_eq_data()
{
    operator_lt_data();
}


void TestQTimeSpan::operator_lt()
{
    QFETCH(QTimeSpan, span1);
    QFETCH(QTimeSpan, span2);
    QFETCH(bool, lt);
    QFETCH(bool, lt_eq);
    QFETCH(bool, gt);
    QFETCH(bool, gt_eq);

    Q_UNUSED(lt)
    Q_UNUSED(lt_eq)
    Q_UNUSED(gt)
    Q_UNUSED(gt_eq)

    QCOMPARE(span1 < span2, lt);
}

void TestQTimeSpan::operator_lt_eq()
{
    QFETCH(QTimeSpan, span1);
    QFETCH(QTimeSpan, span2);
    QFETCH(bool, lt);
    QFETCH(bool, lt_eq);
    QFETCH(bool, gt);
    QFETCH(bool, gt_eq);

    Q_UNUSED(lt)
    Q_UNUSED(lt_eq)
    Q_UNUSED(gt)
    Q_UNUSED(gt_eq)

    QCOMPARE(span1 <= span2, lt_eq);
}

void TestQTimeSpan::operator_gt()
{
    QFETCH(QTimeSpan, span1);
    QFETCH(QTimeSpan, span2);
    QFETCH(bool, lt);
    QFETCH(bool, lt_eq);
    QFETCH(bool, gt);
    QFETCH(bool, gt_eq);

    Q_UNUSED(lt)
    Q_UNUSED(lt_eq)
    Q_UNUSED(gt)
    Q_UNUSED(gt_eq)

    QCOMPARE(span1 > span2, gt);
}

void TestQTimeSpan::operator_gt_eq()
{
    QFETCH(QTimeSpan, span1);
    QFETCH(QTimeSpan, span2);
    QFETCH(bool, lt);
    QFETCH(bool, lt_eq);
    QFETCH(bool, gt);
    QFETCH(bool, gt_eq);

    Q_UNUSED(lt)
    Q_UNUSED(lt_eq)
    Q_UNUSED(gt)
    Q_UNUSED(gt_eq)

    QCOMPARE(span1 >= span2, gt_eq);
}

void TestQTimeSpan::operator_plus_eq()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_minus_eq()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_mult_eq_real()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_mult_eq_int()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_div_eq_real()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_div_eq_int()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_or_eq()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_and_eq()
{
    DEFAULTFALSE
}

void TestQTimeSpan::operator_mult()
{
    QDateTime dt = QDateTime::currentDateTime();
    QTimeSpan s1(dt, QTimeSpan::hour());
    QTimeSpan s2(dt, QTimeSpan::day());
    QTimeSpan s3(QTimeSpan::hour());

    QCOMPARE(24 * s1, s2);
    QCOMPARE(s1 * 24, s2);
    QCOMPARE(12 * s1, s2 * 0.5);
    QCOMPARE(s1 * 12.0, 0.5 * s2);


    QCOMPARE((s3 * 2.0).matchesLength( 2.0 * s1), true);
    QCOMPARE((s1 * 5) > (s1 * 4), true);
    QCOMPARE(s3 * 23.9 < s2, true);
}

void TestQTimeSpan::operator_minus_data()
{
    //data also used for testing matchesLength()
    QDateTime dt1;
    QDateTime dt2(QDateTime::currentDateTime());
    QDateTime dt3(QDate(1975,10,10));
    QDateTime dt4(QDate(2210, 9, 9));
    QDateTime dt5(QDate::currentDate(), QTime(2,0,0,0));
    QDateTime dt6(QDate::currentDate(), QTime(2,0,0,1));
    QDateTime dt7(QDate::currentDate(), QTime(1,59,59,999));
    QDateTime dt8(QDate::currentDate(), QTime(2,0,0,0), Qt::UTC);

    QTest::addColumn<QDateTime>("date1");
    QTest::addColumn<QDateTime>("date2");

    QTest::newRow("default construcuted") << dt1 << dt1;
    QTest::newRow("default construcuted minus current") << dt1 << dt2;
    QTest::newRow("current minus default construcuted") << dt2 << dt1;
    QTest::newRow("date in the past minus current") << dt3 << dt2;
    QTest::newRow("current minus date in the past") << dt2 << dt3;
    QTest::newRow("same date current") << dt2 << dt2;
    QTest::newRow("same date past") << dt3 << dt3;
    QTest::newRow("same date future") << dt4 << dt4;
    QTest::newRow("date in the past minus date in the future") << dt3 << dt4;
    QTest::newRow("date in the future minus date in the past") << dt4 << dt3;
    QTest::newRow("current date, precisely 2 minus almost two") << dt5 << dt7;
    QTest::newRow("current date, almost 2 minus precisely two") << dt7 << dt5;
    QTest::newRow("current date, precisely 2 minus just past two") << dt5 << dt6;
    QTest::newRow("current date, just past 2 minus precisely two") << dt6 << dt5;
    QTest::newRow("current date, almost 2 minus just past two") << dt7 << dt6;
    QTest::newRow("current date, just past 2 minus almost two") << dt6 << dt7;
    QTest::newRow("current date, just past 2 minus just past two") << dt6 << dt6;
    QTest::newRow("current date, extactly 2 minus extactly two") << dt5 << dt5;
    QTest::newRow("current date, almost 2 minus almost two") << dt7 << dt7;
    QTest::newRow("current date, same time, different time zones") << dt5 << dt8;

}

void TestQTimeSpan::operator_minus()
{
    QFETCH(QDateTime, date1); //date1 will normally be the earlier date
    QFETCH(QDateTime, date2); //date2 will normally be the later date

    QTimeSpan s = date2 - date1;

    //no matter what, the reference date should always be date right hand date, even if that date is invalid
    QCOMPARE(s.referenceDate(), date1);

    if (date1.isValid() && date2.isValid()) {
        QCOMPARE(s.referencedDate(), date2);
    } else {
        //substracting two QDateTimes where one or both is not valid results in a QTimeSpan with lenght 0
        QCOMPARE(s.toMSecs(), 0LL);
        if (date1.isValid()) {
            QCOMPARE(s.referencedDate(), date1);
        }
        if (date2.isValid() && !date1.isValid()) {
            QCOMPARE(s.hasValidReference(), false);
            QCOMPARE(s.referencedDate(), QDateTime());
        }
        if (date1.isValid() && !date2.isValid()) {
            QCOMPARE(s.hasValidReference(), true);
            QCOMPARE(s.referencedDate(), date1);
        }
    }

}

void TestQTimeSpan::operator_plus_and_minus()
{
    //test dates
    QDate currentDate = QDate::currentDate();
    QDate nextYear = currentDate.addYears(1);

    QTimeSpan s1 = nextYear - currentDate;
    QTimeSpan s2 = currentDate - nextYear; //negative time span!

    QCOMPARE(currentDate + s1, nextYear);
    QCOMPARE(nextYear - s1, currentDate);
    QCOMPARE(currentDate - s2, nextYear);
    QCOMPARE(nextYear + s2, currentDate);

    //test times
    QTime now = QTime::currentTime();
    QTime later = now.addSecs(60 * 60); //add one hour

    QTimeSpan s3 = later - now;
    QTimeSpan s4 = now - later; //negative time span!

    QCOMPARE(now + s3, later);
    QCOMPARE(later - s3, now);
    QCOMPARE(now - s4, later);
    QCOMPARE(later + s4, now);

    //test datetimes
    QDateTime rightNow = QDateTime::currentDateTime();
    QDateTime nextYear2 = QDateTime(rightNow.addYears(1).date(), rightNow.time());

    QTimeSpan s5 = nextYear2 - rightNow;
    QTimeSpan s6 = rightNow - nextYear2;

    QCOMPARE(rightNow + s5, nextYear2);
    QCOMPARE(nextYear2 - s5, rightNow);
    QCOMPARE(rightNow - s6, nextYear2);
    QCOMPARE(nextYear2 + s6, rightNow);
}

void TestQTimeSpan::normalized()
{
    QTimeSpan s1(QDate::currentDate(), QTimeSpan::day());
    QTimeSpan s2(QDate::currentDate(), -QTimeSpan::day());
    QTimeSpan s3(5000);
    QTimeSpan s4 = -QTimeSpan::day();

    QTimeSpan s5 = s1.normalized();
    QTimeSpan s6 = s2.normalized();
    QTimeSpan s7 = s3.normalized();
    QTimeSpan s8 = s4.normalized();

    //s1 should not be affected
    QCOMPARE(s1.referenceDate().date(), QDate::currentDate());
    QCOMPARE(s1.matchesLength(QTimeSpan::day()), true);

    //s2 should not be affected
    QCOMPARE(s2.referenceDate().date(), QDate::currentDate());
    QCOMPARE(s2.matchesLength(-QTimeSpan::day()), true);

    //s3 should not be affected
    QCOMPARE(s3.hasValidReference(), false);
    QCOMPARE(s3.toMSecs(), 5000LL);

    //s4 should not be affected
    QCOMPARE(s4.hasValidReference(), false);
    QCOMPARE(s4.matchesLength(-QTimeSpan::day()), true);

    //s5 should not be affected
    QCOMPARE(s5.referenceDate().date(), QDate::currentDate());
    QCOMPARE(s5.matchesLength(QTimeSpan::day()), true);

    //s6 should have it's reference date set to yesterday, and the length should be one day (positive)
    QDate d6=QDate::currentDate();
    d6 = d6.addDays(-1);
    QCOMPARE(s6.referenceDate().date(), d6);
    QCOMPARE(s6.matchesLength(QTimeSpan::day()), true);

    //s7 should be unaffected
    QCOMPARE(s7, QTimeSpan(5000));
    QCOMPARE(s7.hasValidReference(), false); //did a valid reference appear out of nowhere?

    //s8 should be positive now
    QCOMPARE(s8, QTimeSpan::day());
    QCOMPARE(s8.hasValidReference(), false); //did a valid reference appear out of nowhere?
}

void TestQTimeSpan::normalize()
{
    QTimeSpan s1(QDate::currentDate(), QTimeSpan::day());
    QTimeSpan s2(QDate::currentDate(), -QTimeSpan::day());
    QTimeSpan s3(5000);
    QTimeSpan s4 = -QTimeSpan::day();

    s1.normalize();
    s2.normalize();
    s3.normalize();
    s4.normalize();

    //s1 should not be affected
    QCOMPARE(s1.referenceDate().date(), QDate::currentDate());
    QCOMPARE(s1.matchesLength(QTimeSpan::day()), true);

    //s2 should have it's reference date set to yesterday, and the length should be one day (positive)
    QDate d2=QDate::currentDate();
    d2 = d2.addDays(-1);
    QCOMPARE(s2.referenceDate().date(), d2);
    QCOMPARE(s2.matchesLength(QTimeSpan::day()), true);

    //s3 should be unaffected
    QCOMPARE(s3, QTimeSpan(5000));
    QCOMPARE(s3.hasValidReference(), false); //did a valid reference appear out of nowhere?

    //s4 should be positive now
    QCOMPARE(s4, QTimeSpan::day());
    QCOMPARE(s4.hasValidReference(), false); //did a valid reference appear out of nowhere?

}

void TestQTimeSpan::isNormal()
{
    QTimeSpan s1(-1000);
    QTimeSpan s2(1000);
    QCOMPARE(s1.isNormal(), false);
    QCOMPARE(s2.isNormal(), true);
}

void TestQTimeSpan::isNegative()
{
    QTimeSpan s1(-1000);
    QTimeSpan s2(1000);
    QCOMPARE(s1.isNegative(), true);
    QCOMPARE(s2.isNegative(), false);
}

void TestQTimeSpan::abs()
{
    QTimeSpan s1(-45000);
    QTimeSpan s2(45000);
    QTimeSpan s3(QDate::currentDate(), -QTimeSpan::day());
    QTimeSpan s4(QDate::currentDate(), QTimeSpan::day());

    QTimeSpan abs_s1 = s1.abs();
    QTimeSpan abs_s2 = s2.abs();
    QTimeSpan abs_s3 = s3.abs();
    QTimeSpan abs_s4 = s4.abs();

    QCOMPARE(abs_s1.toMSecs(), 45000LL);
    QCOMPARE(abs_s1.hasValidReference(), false);

    QCOMPARE(abs_s2, s2);

    QCOMPARE(abs_s3.toMSecs(), QTimeSpan::day().toMSecs());
    QCOMPARE(abs_s3.referenceDate(), s3.referenceDate()); //ref date should not change

    QCOMPARE(abs_s4, s4);

}

void TestQTimeSpan::duration()
{
    //andre: I am not sure what this test is supposed to test exactly?
    DEFAULTFALSE
}

void TestQTimeSpan::overlaps()
{
    QTime t1(2,0,0);
    QTime t2(2,30,0);
    QTime t3(3,15,0);

    QTimeSpan s1(t1, QTimeSpan::hour());
    QTimeSpan s2(t2, QTimeSpan::hour());
    QTimeSpan s3(t3, QTimeSpan::hour());
    QTimeSpan s4(t1, QTimeSpan::minute() * 30);

    QCOMPARE(s1.overlaps(s2), true);
    QCOMPARE(s2.overlaps(s3), true);
    QCOMPARE(s1.overlaps(s3), false);
    QCOMPARE(s2.overlaps(s4), false); // these time spans should not be overlapping
}

void TestQTimeSpan::contains()
{
    //test the different overloads of bool QTimeSpan::contains()
    //note: not finished, see below

    QTimeSpan s1(QDate(1975, 10, 10), QTimeSpan::day());
    QTimeSpan s2(QTimeSpan::hour());

    QDate d1(1975, 10, 10);
    QDate d2(1975, 10, 11);
    QDate d3(1975, 10, 12);
    QTime t1;
    QTime t2(2,0,0);
    QTime t3(23, 59, 59, 999);
    QTime t4(0,0,0,001);

    QDateTime dt1(d1, t1); //1975/10/10, midnight
    QDateTime dt2(d2, t1); //1975/10/11, midnight
    QDateTime dt3(d1, t2); //1975/10/10, 2 AM
    QDateTime dt4(d2, t4); //1975/10/11, 1 millisecond past midnight

    //first, test false response for time spans with invalid references
    QCOMPARE(s2.contains(d1), false);
    QCOMPARE(s2.contains(t1), false);
    QCOMPARE(s2.contains(dt1), false);
    QCOMPARE(s1.contains(s2), false);
    QCOMPARE(s2.contains(s1), false);
    QCOMPARE(s2.contains(s2), false); //even a self-reference must return false if there is no reference date

    //a time span should contain itself, if it has a valid reference date
    QCOMPARE(s1.contains(s1), true);

    //test matching date times
    QCOMPARE(s1.contains(dt1), true);
    QCOMPARE(s1.contains(dt2), true);
    QCOMPARE(s1.contains(dt3), true);
    QCOMPARE(s1.contains(dt4), false);

    //test dates

    //test times

    //test time spans
    QTimeSpan s3(dt3, QTimeSpan::hour());
    QCOMPARE(s1.contains(s3), true);
    QCOMPARE(s3.contains(s1), false);

    DEFAULTFALSE

}

void TestQTimeSpan::overlapped()
{
    QTimeSpan s1(QDate(1975, 10, 10), QTimeSpan::day());
    QTimeSpan su1 = s1.overlapped(s1); //should be the same

    QCOMPARE(s1, su1);

    QTimeSpan s2(QDate(1975, 10, 11), QTimeSpan::day());
    QTimeSpan s3(QDate(1975, 10, 10), QTimeSpan::hour());
    QTimeSpan s4(QDateTime(QDate(1975, 10, 11), QTime(0,0,0,001)), QTimeSpan::day());

    QTimeSpan so2 = s1.overlapped(s2);
    QTimeSpan so3 = s2.overlapped(s1);
    QTimeSpan so4 = s1.overlapped(s3);
    QTimeSpan so5 = s3.overlapped(s1);
    QTimeSpan so6 = s2.overlapped(s3);
    QTimeSpan so7 = s3.overlapped(s2);
    QTimeSpan so8 = s1.overlapped(s4);
    QTimeSpan so9 = s2.overlapped(s4);

    QCOMPARE(so2.startDate(), s2.startDate());
    QCOMPARE(so2.endDate(), s2.startDate());
    QCOMPARE(so2.isEmpty(), true);
    QCOMPARE(so3, so2); //should be the same
    QCOMPARE(so4, s3);
    QCOMPARE(so5, s3);
    QCOMPARE(so6.hasValidReference(), false);
    QCOMPARE(so6.isEmpty(), true);
    QCOMPARE(so6.isNull(), true);
    QCOMPARE(so7, so6);
    QCOMPARE(so8, so6); //should, just like so6, yield a null QTimeSpan.
    QCOMPARE(so9.startDate().time().msec(), 1);
    QCOMPARE(so9.endDate(), s2.endDate());

    //compare when time zones are different
    DEFAULTFALSE

}

void TestQTimeSpan::united()
{
    QTimeSpan s1(QDate(1975, 10, 10), QTimeSpan::day());
    QTimeSpan su1 = s1.united(s1); //should be the same

    QCOMPARE(s1, su1);

    QTimeSpan s2(QDate(1975, 10, 11), QTimeSpan::day());
    QTimeSpan s3(QDate(1975, 10, 10), QTimeSpan::hour());

    QTimeSpan su2 = s1.united(s2);
    QTimeSpan su3 = s2.united(s1);
    QTimeSpan su4 = s1.united(s3);
    QTimeSpan su5 = s3.united(s1);
    QTimeSpan su6 = s2.united(s3);
    QTimeSpan su7 = s3.united(s2);

    QCOMPARE(su2.startDate(), s1.startDate());
    QCOMPARE(su2.endDate(), s2.endDate());
    QCOMPARE(su3, su2);
    QCOMPARE(su4, s1);
    QCOMPARE(su5, s1);
    QCOMPARE(su6.startDate(), s3.startDate());
    QCOMPARE(su6.endDate(), s2.endDate());
    QCOMPARE(su7, su6);

    //compare when time zones are different
    DEFAULTFALSE
}

QTEST_MAIN(TestQTimeSpan)
