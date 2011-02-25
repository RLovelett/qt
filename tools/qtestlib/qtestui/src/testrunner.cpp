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

#include "inc/testrunner.h"
#include "inc/testcase.h"

#include <QFile>
#include <QTextStream>

#if defined(Q_OS_SYMBIAN)
#include <unistd.h>
#endif

QT_BEGIN_NAMESPACE

TestRunner::TestRunner(QObject *parent):
        QObject(parent),
        config(NULL),
        result(NULL),
        caseIndex(0),
        process(NULL),
        stopFlag(false),
        pauseFlag(false)
{
}

TestRunner::~TestRunner()
{
    for (int i = (caseList.size()-1); i >= 0 ; i--) {
        delete caseList[i];
        caseList.removeAt(i);
    }
}

TestRunner::CaseRunningState TestRunner::testState()
{
    return state;
}

TestConfig *TestRunner::testConfig()
{
    return config;
}

TestResult *TestRunner::testResult()
{
    return result;
}

void TestRunner::addTestCase(TestCase *testCase)
{
    connect(testCase, SIGNAL(steps(int)), this, SLOT(progressSteps(int)));
    this->caseList.append(testCase);
}

void TestRunner::setTestConfig(TestConfig *testConfig)
{
    config = testConfig;
}

void TestRunner::setTestResult(TestResult *testResult)
{
    if (result != NULL)
        delete result;
    result = testResult;
    connect(this, SIGNAL(steps(int)), result, SLOT(progressSteps(int)));
}

QString TestRunner::createCommand(TestCase *testCase)
{
    QString command = testCase->command();
    command += config->configStr();
    if (config->useOutputFile())
        command += " -o " + config->outputFilePath() + testCase->logFileName();
    return command;
}

void TestRunner::runCommand(QString command)
{
    process = new PipsProcess(this);
    #if defined(Q_OS_SYMBIAN)
    process->setUsePips(!config->useOutputFile());
    #endif

    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(readyReadStandardError()),
            this, SLOT(processReadyStdErr()));
    connect(process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(processReadyStdOut()));

    process->start(command);

}

void TestRunner::start()
{

    if (state == ERunning)
        return;

    if (caseIndex >= caseList.size())
        return;

    if (process != NULL) {
        delete process;
        process = NULL;
    }
    stopFlag = false;
    pauseFlag = false;
    if (caseIndex == 0) {
        int range = 0;
        for (int i=0; i < caseList.size(); i++) {
            caseList[i]->clearResult();
            caseList[i]->setOutputType(config->outputType());
            range += caseList[i]->functionList().size();
        }
        result->textClear();
        result->setProgressRange(0, range);
        result->progressClear();
        result->clearReport();
    }

    QString command = createCommand(caseList.at(caseIndex));
    state = ERunning;
    emit started();
    result->textOut(command + "\n");
    runCommand(command);

}

void TestRunner::stop()
{
    stopFlag = true;
    if (process != NULL) {
        process->kill();
    } else {
        state = EStopped;
        caseIndex = 0;
        result->textOut(tr("stopped\n"));
        emit stoped();
    }
}

void TestRunner::pause()
{
    pauseFlag = true;
    result->textOut(tr("Waiting for pause...\n"));
}

void TestRunner::processReadyStdErr()
{
    result->textOut(process->readAllStandardError());
}

void TestRunner::processReadyStdOut()
{
    QString output = process->readAllStandardOutput();
    caseList[caseIndex]->outputAppend(output);
    result->textOut(output);
}

void TestRunner::progressSteps(int stepNum)
{
    int pass = 0;
    int fail = 0;
    int skip = 0;
    for (int i=0; i < caseList.size(); i++) {
        pass += caseList[i]->passNum();
        fail += caseList[i]->failNum();
        skip += caseList[i]->skipNum();
    }
    result->setProgressText(pass, fail, skip);
    emit steps(stepNum);
}

void TestRunner::processError(QProcess::ProcessError error)
{
    switch (error) {
    case QProcess::FailedToStart:
        result->textOut(tr("Process error: FailedToStart.\n"));
        processFinished(error, QProcess::CrashExit);
        break;
    case QProcess::Crashed:
        result->textOut(tr("Process error: Crashed.\n"));
        break;
    case QProcess::Timedout:
        result->textOut(tr("Process error: Timedout.\n"));
        break;
    case QProcess::WriteError:
        result->textOut(tr("Process error: WriteError.\n"));
        break;
    case QProcess::ReadError:
        result->textOut(tr("Process error: ReadError.\n"));
        break;
    default:
        result->textOut(tr("Process error: UnknownError.\n"));
    }
}

void TestRunner::processFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/ )
{
    if (process) {
        process->disconnect();
        process = NULL;
    }

    if (config->useOutputFile())
        showOutputFile();
    result->textOut("\n");
    emit steps(caseList[caseIndex]->remainingSteps());
    caseIndex += 1;

    if (stopFlag == true) {
        stop();
        return;
    }

    if ((caseIndex) >= caseList.size()) {
        state = EFinished;
        caseIndex = 0;
        result->textOut(tr("All finished.\n"));
        result->createReport(&caseList);
        emit finished();
        return;
    }

    if (pauseFlag == true) {
        state = EPaused;
        result->textOut(tr("paused\n"));
        emit paused();
        return;
    }

    QString command = createCommand(caseList[caseIndex]);
    result->textOut(command + "\n");
    runCommand(command);

}

void TestRunner::showOutputFile()
{
    // get output text from log file
    QString outputFile = config->outputFilePath() + caseList[caseIndex]->logFileName();
    QFile file(outputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result->textOut(tr("\nCould not open the output file: ")+ outputFile +"\n");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        caseList[caseIndex]->outputAppend(line + "\n");
        result->textOut(line + "\n");
    }

    file.close();

}
QT_END_NAMESPACE

