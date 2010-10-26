#ifndef TST_QTIMESPAN_H
#define TST_QTIMESPAN_H

#include <QtTest/QtTest>
#include "qtimespan.h"

class TestQTimeSpan : public QObject
{
    Q_OBJECT
public:
    TestQTimeSpan();
    virtual ~TestQTimeSpan();

private slots:
    // Run before any tests
    void initTestCase();

    // Run after all tests have finished
    void cleanupTestCase();

    // Run before every test function
    void init();

    // Run after every test
    void cleanup();

    // The tests
    void constructionEmpty();
    void constructionNonEmpty();

    void msecsPart();
    void secsPart();
    void minutesPart();
    void hoursPart();
    void daysPart();
    void weeksPart();
    void monthsPart();
    void yearsPart();
    void part();
    void parts();
    void magnitude_data();
    void magnitude();

    void setMSecsPart();
    void setSecsPart();
    void setMinutesPart();
    void setHoursPart();
    void setDaysPart();
    void setWeeksPart();
    void setMonthsPart();
    void setYearsPart();
    void setPart();

    void toMSecs();
    void toSecs();
    void toMinutes();
    void toHours();
    void toDays();
    void toWeeks();
    void toMonths();
    void toYears();
    void toTimeUnit();

    void setFromMSecs();
    void setFromSecs();
    void setFromMinutes();
    void setFromHours();
    void setFromDays();
    void setFromWeeks();
    void setFromMonths();
    void setFromYears();
    void setFromTimeUnit();

    void hasValidReferenceDate();
    void referenceDate();
    void setReferenceDate();
    void moveReferenceDate();
    void setReferencedDate();
    void moveReferencedDate();
    void startAndEndDate();
    void referencedDate();

    void toString();
    void fromString();
    void toApproximateString();

    /*
    void fromMSecs();
    void fromSecs();
    void fromMinutes();
    void fromHours();
    void fromDays();
    void fromWeeks();
    void fromMonths();
    void fromYears();
    */
    void fromTimeUnit();

    void operator_eq();
    void operator_eq_eq();
    void operator_eq_eq_data();
    void operator_not_eq();
    void operator_not_eq_data();

    void operator_lt_data();
    void operator_lt_eq_data();
    void operator_gt_data();
    void operator_gt_eq_data();

    void operator_lt();
    void operator_lt_eq();
    void operator_gt();
    void operator_gt_eq();

    void matchesLength();
    void matchesLength_data();

    void operator_plus_eq();
    void operator_minus_eq();
    void operator_mult_eq_real();
    void operator_mult_eq_int();
    void operator_div_eq_real();
    void operator_div_eq_int();
    void operator_or_eq();
    void operator_and_eq();

    void operator_mult();
    void operator_plus_and_minus();

    void operator_minus_data();
    void operator_minus();

    void normalized();
    void normalize();
    void isNormal();
    void isNegative();
    void abs();
    void duration();

    void overlaps();
    void contains();
    void overlapped();
    void united();
};

#endif // TST_QTIMESPAN_H
