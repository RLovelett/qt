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

#ifndef TESTCONFIG_H
#define TESTCONFIG_H

#include <QString>
#include <QObject>
#include <QStringList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSettings;
class GlobalConfig;

class TestConfig : public QObject
{
    Q_OBJECT

public:
    // Constants
    const static QString ORG;
    const static QString APP;
    const static QString DELIMETER;
    const static QString EXEPATH;
    const static QString SELECTEDCASES;
    const static QString GLOBALCFG;
    const static QString OPTCHAR;
    const static QString INTARG;
    const static QString PATHDELI;

public:
    TestConfig(QObject *parent = 0);
    TestConfig(const QString &path, QObject *parent = 0);
    enum OutputType
    {
        ETxt = 1,
        EXml = 2,
        EXunit = 3
    };

    virtual ~TestConfig();

    const GlobalConfig *globalConfig();

    bool useOutputFile();
    QString outputFilePath();
    OutputType outputType();
    QString configStr();
    QString selectedCases(const QString &testName);
    void saveSelectedCases(const QString &testName, const QString &path, const QString &testCases);
    void removeUnselectedCases(const QString &testName);
    QString executableFile(const QString &testName);
    QStringList testNames();

    void saveGlobalConfig(const GlobalConfig &cfg);

private:
    void loadConfig();
    void loadAlternativeConfig(const QString &path);
    void loadGlobalConfig();

private:
    QSettings       *config;
    GlobalConfig    *globalCfg;
};

// Internal data class
class GlobalConfig : public QObject
{
    Q_OBJECT

public:
    // Constants
    const static QString OUTPUTFORMAT;
    const static QString OUTPUTPATH;
    const static QString FLUSH;
    const static QString SILENT;
    const static QString VERBOSE;
    const static QString EVENTDELAY;
    const static QString KEYDELAY;
    const static QString MOUSEDELAY;
    const static QString KEYEVENTVERBOSE;
    const static QString MAXWARNINGS;
    const static QString NOCRASHHANDLER;
    const static QString CALLGRIND;
    const static QString TICKCOUNTER;
    const static QString EVENTCOUNTER;
    const static QString MINIMUMVALUE;
    const static QString ITERATIONS;
    const static QString MEDIAN;
    const static QString VERBOSEBENCHMARK;

public:
    GlobalConfig(QObject *parent =0);
    GlobalConfig(const GlobalConfig &cfg);
    virtual ~GlobalConfig();
    QString configStr();
    QString outputPath() const;
    bool equalsTo(const GlobalConfig *cfg);

    GlobalConfig &operator = (const GlobalConfig &other);

    void setOutputFormat(QString outputFormat);
    void setOutputPath(QString outputPath);
    void setFlush(bool flush);
    void setSilent(bool silent);
    void setVerbose(QString verbose);

    void setEventDelay(int eventDelay);
    void setKeyDelay(int keyDelay);
    void setMouseDelay(int mouseDelay);
    void setKeyEventVerbose(bool keyEventVerbose);
    void setMaxWarnings(int maxWarnings);
    void setNoCrashHandler(bool noCrashHandler);

#if defined(Q_OS_LINUX)
    void setCallGrind(bool callGrind);
#endif
    void setTickCounter(bool tickCounter);
    void setEventCounter(bool eventCounter);
    void setMinimumValue(int minValue);
    void setIterations(int iters);
    void setMedian(int median);
    void setVerboseBenchMarking(bool verboseBenchMarking);

    QString outputFormat() const;
    bool    flush() const;
    bool    silent() const;
    QString verbose() const;

    int     eventDelay() const;
    int     keyDelay() const;
    int     mouseDelay() const;
    bool    keyEventVerbose() const;
    int     maxWarnings() const;
    bool    noCrashHandler() const;

#if defined(Q_OS_LINUX)
    bool    callGrind() const;
#endif
    bool    tickCounter() const;
    bool    eventCounter() const;
    int     minimumValue() const;
    int     iterations() const;
    int     median() const;
    bool    verboseBenchMarking() const;

private:
    QString m_outputFormat;
    QString m_outputPath;
    QString m_verbose;
    bool    m_flush;
    bool    m_silent;

    int     m_eventDelay;
    int     m_keyDelay;
    int     m_mouseDelay;
    bool    m_keyEventVerbose;
    int     m_maxWarnings;
    bool    m_noCrashHandler;

#if defined(Q_OS_LINUX)
    bool    m_callGrind;
#endif
    bool    m_tickCounter;
    bool    m_eventCounter;
    int     m_minimumValue;
    int     m_iterations;
    int     m_median;
    bool    m_verboseBenchMarking;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // TESTCONFIG_H
