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

#include "inc/testconfig.h"
#include <QSettings>

const static QString ORG = "Nokia";
const static QString APP = "QTestUI";

const static QString DELIMETER = "/";

const static QString EXEPATH = "path";
const static QString SELECTEDCASES = "selectedcases";

const static QString GLOBALCFG = "global";
const static QString OPTCHAR = "-";
const static QString INTARG = " %1";

#if defined(Q_OS_LINUX)
const static QString PATHDELI = "/";
#else
const static QString PATHDELI = "\\";
#endif

TestConfig::TestConfig(QObject* parent):
        QObject(parent),
        config(NULL),
        globalCfg(NULL)
{
    loadConfig();
    loadGlobalConfig();
}

TestConfig::TestConfig(const QString& path, QObject* parent):
        QObject(parent),
        config(NULL)
{
    loadAlternativeConfig(path);
    loadGlobalConfig();
}

TestConfig::~TestConfig()
{
    if (config != NULL)
        delete config;
}

const GlobalConfig* TestConfig::globalConfig()
{
    return globalCfg;
}

QString TestConfig::configStr()
{
    QString str;
    if (globalCfg != NULL) {
        str += globalCfg->configStr();
    }
    return str;
}

TestConfig::OutputType TestConfig::outputType()
{
    if ((globalCfg->outputFormat() == "xml")
        || (globalCfg->outputFormat() == "lightxml")) {
        return EXml;
    } else if (globalCfg->outputFormat() == "xunitxml") {
        return EXunit;
    } else {
        return ETxt;
    }
}

bool TestConfig::useOutputFile()
{
    return outputFilePath() != "";
}

QString TestConfig::outputFilePath()
{
    if (globalCfg != NULL)
        return globalCfg->outputPath();
    else
        return "";
}

QString TestConfig::selectedCases(const QString& testName)
{
    return config->value(testName + DELIMETER + SELECTEDCASES).toString();
}

QString TestConfig::executableFile(const QString& testName)
{
    return config->value(testName + DELIMETER + EXEPATH).toString();
}

QStringList TestConfig::testNames()
{
    QStringList names = config->childGroups();
    names.removeAll(GLOBALCFG);
    return names;
}

void TestConfig::loadConfig()
{
    config = new QSettings(QSettings::IniFormat, QSettings::UserScope, ORG, APP, this);
}

void TestConfig::loadAlternativeConfig(const QString& path)
{
    config = new QSettings(path, QSettings::IniFormat, this);
}

void TestConfig::loadGlobalConfig()
{
    globalCfg = new GlobalConfig(this);
    QStringList sectionList = config->childGroups();
    if (sectionList.contains(GLOBALCFG)) {
        config->beginGroup(GLOBALCFG);
#if defined(Q_OS_LINUX)
        globalCfg->setCallGrind(config->value(/*GLOBALCFG + DELIMETER + */CALLGRIND, false).toBool());
#endif
        globalCfg->setEventCounter(config->value(/*GLOBALCFG + DELIMETER + */EVENTCOUNTER, false).toBool());
        globalCfg->setOutputFormat(config->value(/*GLOBALCFG + DELIMETER + */OUTPUTFORMAT).toString());
        globalCfg->setOutputPath(config->value(/*GLOBALCFG + DELIMETER + */OUTPUTPATH).toString());
        globalCfg->setFlush(config->value(/*GLOBALCFG + DELIMETER + */FLUSH, false).toBool());
        globalCfg->setSilent(config->value(/*GLOBALCFG + DELIMETER + */SILENT, false).toBool());
        globalCfg->setVerbose(config->value(/*GLOBALCFG + DELIMETER + */VERBOSE).toString());
        globalCfg->setEventDelay(config->value(/*GLOBALCFG + DELIMETER + */EVENTDELAY, -1).toInt());
        globalCfg->setKeyDelay(config->value(/*GLOBALCFG + DELIMETER + */KEYDELAY, -1).toInt());
        globalCfg->setMouseDelay(config->value(/*GLOBALCFG + DELIMETER + */MOUSEDELAY, -1).toInt());
        globalCfg->setKeyEventVerbose(config->value(/*GLOBALCFG + DELIMETER + */KEYEVENTVERBOSE, false).toBool());
        globalCfg->setMaxWarnings(config->value(/*GLOBALCFG + DELIMETER + */MAXWARNINGS, -1).toInt());
        globalCfg->setNoCrashHandler(config->value(/*GLOBALCFG + DELIMETER + */NOCRASHHANDLER, false).toBool());
        globalCfg->setTickCounter(config->value(/*GLOBALCFG + DELIMETER + */TICKCOUNTER, false).toBool());
        globalCfg->setMinimumValue(config->value(/*GLOBALCFG + DELIMETER + */MINIMUMVALUE, -1).toInt());
        globalCfg->setIterations(config->value(/*GLOBALCFG + DELIMETER + */ITERATIONS, -1).toInt());
        globalCfg->setMedian(config->value(/*GLOBALCFG + DELIMETER + */MEDIAN, -1).toInt());
        globalCfg->setVerboseBenchMarking(config->value(/*GLOBALCFG + DELIMETER + */VERBOSEBENCHMARK, false).toBool());
        config->endGroup();
    }
}

void TestConfig::saveGlobalConfig(const GlobalConfig& cfg)
{
    config->beginGroup(GLOBALCFG);
#if defined(Q_OS_LINUX)
    config->setValue(CALLGRIND, cfg.callGrind());
#endif
    config->setValue(EVENTCOUNTER, cfg.eventCounter());
    config->setValue(OUTPUTFORMAT, cfg.outputFormat());
    QString path = cfg.outputPath();
    if (path != "" && path.endsWith('/')) path.remove(path.length() - 1, 1);
    config->setValue(OUTPUTPATH, path);
    config->setValue(FLUSH, cfg.flush());
    config->setValue(SILENT, cfg.silent());
    config->setValue(VERBOSE, cfg.verbose());
    config->setValue(EVENTDELAY, cfg.eventDelay());
    config->setValue(KEYDELAY, cfg.keyDelay());
    config->setValue(MOUSEDELAY, cfg.mouseDelay());
    config->setValue(KEYEVENTVERBOSE, cfg.keyEventVerbose());
    config->setValue(MAXWARNINGS, cfg.maxWarnings());
    config->setValue(NOCRASHHANDLER, cfg.noCrashHandler());
    config->setValue(TICKCOUNTER, cfg.tickCounter());
    config->setValue(MINIMUMVALUE, cfg.minimumValue());
    config->setValue(ITERATIONS, cfg.iterations());
    config->setValue(MEDIAN, cfg.median());
    config->setValue(VERBOSEBENCHMARK, cfg.verboseBenchMarking());
    config->endGroup();
}

GlobalConfig::GlobalConfig(QObject *parent):
        QObject(parent),
        m_flush(false),
        m_silent(false),
        m_eventDelay(-1),
        m_keyDelay(-1),
        m_mouseDelay(-1),
        m_keyEventVerbose(false),
        m_maxWarnings(-1),
        m_noCrashHandler(false),
#if defined(Q_OS_LINUX)
        m_callGrind(false),
#endif
        m_tickCounter(false),
        m_eventCounter(false),
        m_minimumValue(-1),
        m_iterations(-1),
        m_median(-1),
        m_verboseBenchMarking(false)
{
}

GlobalConfig::GlobalConfig(const GlobalConfig& cfg):
        QObject(cfg.parent()),
        m_outputFormat(cfg.m_outputFormat),
        m_outputPath(cfg.m_outputPath),
        m_verbose(cfg.m_verbose),
        m_flush(cfg.m_flush),
        m_silent(cfg.m_silent),
        m_eventDelay(cfg.m_eventDelay),
        m_keyDelay(cfg.m_keyDelay),
        m_mouseDelay(cfg.m_mouseDelay),
        m_keyEventVerbose(cfg.m_keyEventVerbose),
        m_maxWarnings(cfg.m_maxWarnings),
        m_noCrashHandler(cfg.m_noCrashHandler),
#if defined(Q_OS_LINUX)
        m_callGrind(cfg.m_callGrind),
#endif
        m_tickCounter(cfg.m_tickCounter),
        m_eventCounter(cfg.m_eventCounter),
        m_minimumValue(cfg.m_minimumValue),
        m_iterations(cfg.m_iterations),
        m_median(cfg.m_median),
        m_verboseBenchMarking(cfg.m_verboseBenchMarking)
{
}

GlobalConfig::~GlobalConfig()
{
}

QString GlobalConfig::configStr()
{
    QString str;
#if defined(Q_OS_LINUX)
    if (this->m_callGrind) str += " " + OPTCHAR + CALLGRIND;
#endif
    if (this->m_eventCounter) str += " " + OPTCHAR + EVENTCOUNTER;
    if (this->m_outputFormat != "")  str += " " + OPTCHAR + this->m_outputFormat;
    if (this->m_flush) str += " " + OPTCHAR + FLUSH;
    if (this->m_silent) str += " " + OPTCHAR + SILENT;
    if (this->m_verbose != "") str += " " + OPTCHAR + this->m_verbose;
    if (this->m_eventDelay != -1) str += " " + OPTCHAR + EVENTDELAY + INTARG.arg(this->m_eventDelay);
    if (this->m_keyDelay != -1)  str += " " + OPTCHAR + KEYDELAY + INTARG.arg(this->m_keyDelay);
    if (this->m_mouseDelay != -1) str += " " + OPTCHAR + MOUSEDELAY + INTARG.arg(this->m_mouseDelay);
    if (this->m_keyEventVerbose) str += " " + OPTCHAR + KEYEVENTVERBOSE;
    if (this->m_maxWarnings != -1) str += " " + OPTCHAR + MAXWARNINGS + INTARG.arg(this->m_maxWarnings);
    if (this->m_noCrashHandler) str += " " + OPTCHAR + NOCRASHHANDLER;
    if (this->m_tickCounter) str += " " + OPTCHAR + TICKCOUNTER;
    if (this->m_minimumValue != -1) str += " " + OPTCHAR + MINIMUMVALUE + INTARG.arg(this->m_minimumValue);
    if (this->m_iterations != -1) str += " " + OPTCHAR + ITERATIONS + INTARG.arg(this->m_iterations);
    if (this->m_median != -1) str += " " + OPTCHAR + MEDIAN + INTARG.arg(this->m_median);
    if (this->m_verboseBenchMarking) str += " " + OPTCHAR + VERBOSEBENCHMARK;
    return str;
}

QString GlobalConfig::outputPath() const
{
    QString str;
    if (this->m_outputPath != "") str += /*" " + OPTCHAR + "o" + " " + */this->m_outputPath + PATHDELI;
    return str;
}

bool GlobalConfig::equalsTo(const GlobalConfig* cfg)
{
    if (cfg == NULL)
        return false;

    if (this->m_outputPath != cfg->m_outputPath) return false;
    if (this->m_outputFormat != cfg->m_outputFormat) return false;
    if (!this->m_flush != !cfg->m_flush) return false;
    if (!this->m_silent != !cfg->m_silent) return false;
    if (this->m_verbose != cfg->m_verbose) return false;

    if (this->m_eventDelay != cfg->m_eventDelay) return false;
    if (this->m_keyDelay != cfg->m_keyDelay) return false;
    if (this->m_mouseDelay != cfg->m_mouseDelay) return false;
    if (!this->m_keyEventVerbose != !cfg->m_keyEventVerbose) return false;
    if (this->m_maxWarnings != cfg->m_maxWarnings) return false;
    if (!this->m_noCrashHandler != !cfg->m_noCrashHandler) return false;

#if defined(Q_OS_LINUX)
    if (!this->m_callGrind != !cfg->m_callGrind) return false;
#endif

    if (!this->m_tickCounter != !cfg->m_tickCounter) return false;
    if (this->m_minimumValue != cfg->m_minimumValue) return false;
    if (this->m_iterations != cfg->m_iterations) return false;
    if (this->m_median != cfg->m_median) return false;
    if (!this->m_eventCounter != !cfg->m_eventCounter) return false;
    if (!this->m_verboseBenchMarking != !cfg->m_verboseBenchMarking) return false;

    return true;
}

GlobalConfig& GlobalConfig::operator=(const GlobalConfig& other)
{
    this->m_outputPath = other.m_outputPath;
    this->m_outputFormat = other.m_outputFormat;
    this->m_flush = other.m_flush;
    this->m_silent = other.m_silent;
    this->m_verbose = other.m_verbose;
    this->m_eventDelay = other.m_eventDelay;
    this->m_keyDelay = other.m_keyDelay;
    this->m_mouseDelay = other.m_mouseDelay;
    this->m_keyEventVerbose = other.m_keyEventVerbose;
    this->m_maxWarnings = other.m_maxWarnings;
    this->m_noCrashHandler = other.m_noCrashHandler;

#if defined(Q_OS_LINUX)
    this->m_callGrind = other.m_callGrind;
#endif

    this->m_tickCounter = other.m_tickCounter;
    this->m_minimumValue = other.m_minimumValue;
    this->m_iterations = other.m_iterations;
    this->m_median = other.m_median;
    this->m_eventCounter = other.m_eventCounter;
    this->m_verboseBenchMarking = other.m_verboseBenchMarking;
    return *this;
}

void GlobalConfig::setOutputFormat(QString outputFormat)
{
    m_outputFormat = outputFormat;
}

void GlobalConfig::setOutputPath(QString outputPath)
{
    m_outputPath = outputPath;
}

void GlobalConfig::setFlush(bool flush)
{
    m_flush = flush;
}

void GlobalConfig::setSilent(bool silent)
{
    m_silent = silent;
}

void GlobalConfig::setVerbose(QString verbose)
{
    m_verbose = verbose;
}


void GlobalConfig::setEventDelay(int eventDelay)
{
    m_eventDelay = eventDelay;
}

void GlobalConfig::setKeyDelay(int keyDelay)
{
    m_keyDelay = keyDelay;
}

void GlobalConfig::setMouseDelay(int mouseDelay)
{
    m_mouseDelay = mouseDelay;
}

void GlobalConfig::setKeyEventVerbose(bool keyEventVerbose)
{
    m_keyEventVerbose = keyEventVerbose;
}

void GlobalConfig::setMaxWarnings(int maxWarnings)
{
    m_maxWarnings = maxWarnings;
}

void GlobalConfig::setNoCrashHandler(bool noCrashHandler)
{
    m_noCrashHandler = noCrashHandler;
}

#if defined(Q_OS_LINUX)
void GlobalConfig::setCallGrind(bool callGrind)
{
    m_callGrind = callGrind;
}
#endif

void GlobalConfig::setTickCounter(bool tickCounter)
{
    m_tickCounter = tickCounter;
}

void GlobalConfig::setEventCounter(bool eventCounter)
{
    m_eventCounter = eventCounter;
}

void GlobalConfig::setMinimumValue(int minValue)
{
    m_minimumValue = minValue;
}

void GlobalConfig::setIterations(int iters)
{
    m_iterations = iters;
}

void GlobalConfig::setMedian(int median)
{
    m_median = median;
}

void GlobalConfig::setVerboseBenchMarking(bool verboseBenchMarking)
{
    m_verboseBenchMarking = verboseBenchMarking;
}

QString GlobalConfig::outputFormat() const
{
    return m_outputFormat;
}

bool GlobalConfig::flush() const
{
    return m_flush;
}

bool GlobalConfig::silent() const
{
    return m_silent;
}

QString GlobalConfig::verbose() const
{
    return m_verbose;
}

int GlobalConfig::eventDelay() const
{
    return m_eventDelay;
}

int GlobalConfig::keyDelay() const
{
    return m_keyDelay;
}

int GlobalConfig::mouseDelay() const
{
    return m_mouseDelay;
}

bool GlobalConfig::keyEventVerbose() const
{
    return m_keyEventVerbose;
}

int GlobalConfig::maxWarnings() const
{
    return m_maxWarnings;
}

bool GlobalConfig::noCrashHandler() const
{
    return m_noCrashHandler;
}

#if defined(Q_OS_LINUX)
bool GlobalConfig::callGrind() const
{
    return m_callGrind;
}
#endif

bool GlobalConfig::tickCounter() const
{
    return m_tickCounter;
}

bool GlobalConfig::eventCounter() const
{
    return m_eventCounter;
}

int GlobalConfig::minimumValue() const
{
    return m_minimumValue;
}

int GlobalConfig::iterations() const
{
    return m_iterations;
}

int GlobalConfig::median() const
{
    return m_median;
}

bool GlobalConfig::verboseBenchMarking() const
{
    return m_verboseBenchMarking;
}

