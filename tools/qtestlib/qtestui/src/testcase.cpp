/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "inc/testcase.h"

QT_BEGIN_NAMESPACE

TestCase::TestCase(QObject *parent) :
        QObject(parent),
        pass(0),
        fail(0),
        skip(0),
        progressPos(0),
        progressIndex(0),
        type(TestConfig::ETxt)
{
}

TestCase::~TestCase()
{
}

QString TestCase::caseName()
{
    return name;
}

void TestCase::setName(QString name)
{
    this->name = name;
}

QString TestCase::fullPath()
{
    return path;
}

void TestCase::setFullPath(QString path)
{
    this->path = path;
}

QStringList TestCase::functionList()
{
    return functions;
}

void TestCase::addFunction(QString function)
{
    functions.append(function);
}

QString TestCase::logFileName()
{
    return name + "_output";
}

QString TestCase::command()
{
#if defined(Q_OS_LINUX)
    QString command = path;
#elif defined(Q_OS_SYMBIAN)
    QString command = name;
#else
    QString command = path;
#endif
    command += tr(" ");
    command += functions.join(tr(" "));
    return command;
}

int TestCase::passNum()
{
    return pass;
}

void TestCase::setPassNum(int passNum)
{
    pass = passNum;
}

int TestCase::failNum()
{
    return fail;
}

void TestCase::setFailNum(int failNum)
{
    fail = failNum;
}

int TestCase::skipNum()
{
    return skip;
}

void TestCase::setSkipNum(int skipNum)
{
    skip = skipNum;
}

TestConfig::OutputType TestCase::outputType()
{
    return type;
}

void TestCase::setOutputType(TestConfig::OutputType outputType)
{
    type = outputType;
}

QString TestCase::testOutput()
{
    return output;
}

void TestCase::outputAppend(QString str)
{
    output.append(str);
    switch (type) {
    case TestConfig::ETxt:
        txtParser();
        break;
    case TestConfig::EXml:
        xmlParser();
        break;
    case TestConfig::EXunit:
        xunitParser();
        break;
    default:
        ;
    }
}

void TestCase::clearResult()
{
    pass = 0;
    fail = 0;
    skip = 0;
    output.clear();
    type = TestConfig::ETxt;
    progressPos = 0;
    progressIndex = 0;
}

void TestCase::txtParser()
{
    while (progressIndex < functions.size()) {
        QRegExp rx(functions[progressIndex]);
        int matchPos = rx.indexIn(output, progressPos);
        if (matchPos == -1)
            break;
        progressPos = matchPos;
        progressIndex += 1;
        emit steps(1);
    }

    QRegExp rx("Totals: (\\d+) passed, (\\d+) failed, (\\d+) skipped");
    if (rx.indexIn(output, progressPos) != -1) {
        pass = rx.cap(1).toInt();
        fail = rx.cap(2).toInt();
        skip = rx.cap(3).toInt();
        emit steps(functions.size() - progressIndex);
    }

}

void TestCase::xmlParser()
{
    int step = 0;

    while (1) {
        //parse function name
        QRegExp rxName("<TestFunction name=\\\"([^\\\"]*)\\\">");
        int matchPosName = rxName.indexIn(output, progressPos);
        //parse test result
        QRegExp rxType("<Incident type=\\\"([^\\\"]*)\\\"|<Message type=\\\"([^\\\"]*)\\\"");
        int matchPosType = rxType.indexIn(output, progressPos);

        if ((matchPosName == -1) && (matchPosType == -1))
            break;

        if ((matchPosName != -1) && (matchPosType != -1)) {
            if (matchPosName < matchPosType)
                matchPosType = -1;
            else
                matchPosName = -1;
        }

        if (matchPosName != -1) {
            progressPos = matchPosName + rxName.matchedLength();
            if (progressIndex < functions.size()) {
                if (rxName.cap(1) == functions[progressIndex]) {
                    progressIndex += 1;
                    step += 1;
                }
            }
        }

        if (matchPosType != -1) {
            progressPos = matchPosType + rxType.matchedLength();
            if ((rxType.cap(1) == "pass")
                ||(rxType.cap(1) == "xfail")) {
                pass += 1;
            } else if ((rxType.cap(1) == "fail")
                ||(rxType.cap(1) == "xpass")) {
                fail += 1;
            } else if (rxType.cap(2) == "skip") {
                pass += 1;
                skip += 1;
            }
        }

    }
    emit steps(step);
}

void TestCase::xunitParser()
{
    int step = 0;
    QString name = "";
    QString type = "";

    while (1) {
        //parse "<testcase"
        QRegExp rxTestcase("<testcase result=\\\"([^\\\"]*)\\\" name=\\\"([^\\\"]*)\\\"");
        int matchPos = rxTestcase.indexIn(output, progressPos);
        if (matchPos == -1)
            break;

        type = rxTestcase.cap(1);
        name = rxTestcase.cap(2);

        if ( type == "pass") {
            //parse "/>" or "</testcase"
            QRegExp rxTestcaseEnd1("\\/>");
            QRegExp rxTestcaseEnd2("<\\/testcase");
            int matchPosEnd1 = rxTestcaseEnd1.indexIn(output, matchPos);
            if ((matchPosEnd1 == -1)
                ||(matchPosEnd1 > matchPos+rxTestcase.matchedLength())) {
                int matchPosEnd2 = rxTestcaseEnd2.indexIn(output, matchPos);
                if (matchPosEnd2 == -1) {
                    break;
                }
                QRegExp rxSkip("message=\\\"skip comments\\\" type=\\\"skip\\\"");
                int matchPosSkip = rxSkip.indexIn(output, matchPos);
                if (matchPosSkip != -1) {
                    type = "skip";
                }
            }
        }

        progressPos = matchPos + rxTestcase.matchedLength();

        if (progressIndex < functions.size()) {
            if (name == functions[progressIndex]) {
                progressIndex += 1;
                step += 1;
            }
        }

        if ((type == "pass")
            ||(type == "xfail"))
            pass += 1;
        else if ((type == "fail")
            ||(type == "xpass"))
            fail += 1;
        else if (type == "skip") {
            pass += 1;
            skip += 1;
        }

    }
    emit steps(step);
}

int TestCase::remainingSteps()
{
    return functions.size() - progressIndex;
}

QT_END_NAMESPACE

