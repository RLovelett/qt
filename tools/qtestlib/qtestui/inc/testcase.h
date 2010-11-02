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

#ifndef TESTCASE_H
#define TESTCASE_H

#include <QString>
#include <QStringList>
#include "testconfig.h"

class TestCase : public QObject
{
    Q_OBJECT

public:

    TestCase(QObject* parent = 0);
    virtual ~TestCase();

    QString caseName();
    void setName(QString name);
    QString fullPath();
    void setFullPath(QString path);
    QStringList functionList();

    QString logFileName();
    QString command();

    void addFunction(QString function);
    int passNum();
    void setPassNum(int passNum);
    int failNum();
    void setFailNum(int failNum);
    int skipNum();
    void setSkipNum(int skipNum);
    TestConfig::OutputType outputType();
    void setOutputType(TestConfig::OutputType outputType);
    QString testOutput();
    void outputAppend(QString str);

    int remainingSteps();
    void clearResult();

private:
    void txtParser();
    void xmlParser();
    void xunitParser();

signals:
    void steps(int stepNum);

private:
    QString name;
    QString path;
    QStringList functions;

    int pass;
    int fail;
    int skip;
    QString output;
    TestConfig::OutputType type;
    int progressPos;
    int progressIndex;

};


#endif // TESTCASE_H
