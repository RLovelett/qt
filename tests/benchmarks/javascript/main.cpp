/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>
#include <QtScript>
#include <QString>

#include "json.h"
#include "sunspider.h"

class tst_javascript : public QObject
{
    Q_OBJECT

public:
    tst_javascript();
    virtual ~tst_javascript();

private slots:
    void simplecalls_data();
    void simplecalls();

    // JSON tests
    void parseJson();
    void evalJson();
    void nativeJson();

    // SunSpider tests
    void sunSpiderTests_data();
    void sunSpiderTests();

private:
    Json*   m_json;
};


tst_javascript::tst_javascript()
{
    m_json = (Json*)new Json();
}

tst_javascript::~tst_javascript()
{
    delete m_json;
}

void tst_javascript::simplecalls_data()
{
    QTest::addColumn<QString>("code");
    QTest::newRow("empty function") << QString::fromLatin1("(function(){})");
    QTest::newRow("function returning number") << QString::fromLatin1("(function(){ return 123; })");
    QTest::newRow("closure") << QString::fromLatin1("(function(a, b){ return function() { return a + b; }; })(1, 2)");
}

void tst_javascript::simplecalls()
{
    QScriptEngine engine;

    QFETCH(QString, code);
    QScriptValue func = engine.evaluate(code);
    QVERIFY(func.isFunction());
    QBENCHMARK {
        (void)func.call();
    }
}

void tst_javascript::parseJson()
{
    QScriptEngine engine;

    QBENCHMARK {
        QScriptValue parsedjson = engine.evaluate(m_json->jsonString());
    }
}

void tst_javascript::evalJson()
{
    QScriptEngine engine;

    QString jsonEval = "eval(";
    jsonEval += m_json->jsonString();
    jsonEval += ")";

    QBENCHMARK {
        QScriptValue parsedjson = engine.evaluate(jsonEval);
    }
}

void tst_javascript::nativeJson()
{
    QScriptEngine engine;

    QString jsonParser = "JSON.parse(";
    jsonParser += m_json->jsonString();
    jsonParser += ")";

    QBENCHMARK {
        QScriptValue parsedjson = engine.evaluate(jsonParser);
    }
}

void tst_javascript::sunSpiderTests_data()
{
    Sunspider* sunSpider = new Sunspider();
    QStringList testFiles = sunSpider->testFiles();

    QTest::addColumn<QString>("test");
    for (int i=0; i<testFiles.count(); i++) {
        QTest::newRow(testFiles.at(i).toLatin1()) << testFiles.at(i);
        }
}

void tst_javascript::sunSpiderTests()
{
    QScriptEngine engine;

    // Get the resource filename and open
    QFETCH(QString, test);
    QFile file(":/sunspider/" + test);
    if (!file.open(QFile::ReadOnly)) {
        qFatal("Cannot open input file");
        return;
    }

    QByteArray data = file.readAll();
    const char *d = data.constData();
    int size = data.size();
    QString testcode = QString::fromUtf8(d, size);

    QBENCHMARK {
        QScriptValue ss = engine.evaluate(testcode);
    }
}


QTEST_MAIN(tst_javascript)

#include "main.moc"
