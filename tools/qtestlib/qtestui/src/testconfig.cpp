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
#include <QDir>

QT_BEGIN_NAMESPACE

// Constants
const QString TestConfig::ORG = "Nokia";
const QString TestConfig::APP = "QTestUI";
const QString TestConfig::DELIMETER = "/";
const QString TestConfig::EXEPATH = "path";
const QString TestConfig::SELECTEDCASES = "selectedcases";
const QString TestConfig::GLOBALCFG = "global";
const QString TestConfig::OPTCHAR = "-";
const QString TestConfig::INTARG = " %1";
const QString TestConfig::PATHDELI = "/";

TestConfig::TestConfig(QObject *parent):
        QObject(parent),
        config(NULL),
        globalCfg(NULL)
{
    loadConfig();
    loadGlobalConfig();
}

TestConfig::TestConfig(const QString &path, QObject *parent):
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

const GlobalConfig *TestConfig::globalConfig()
{
    return globalCfg;
}

QString TestConfig::configStr()
{
    QString str;
    if (globalCfg != NULL)
        str += globalCfg->configStr();
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
        return QString();
}

QString TestConfig::selectedCases(const QString &testName)
{
    return config->value(testName + TestConfig::DELIMETER + TestConfig::SELECTEDCASES).toString();
}

void TestConfig::saveSelectedCases(const QString &testName, const QString &path, const QString &testCases)
{
    config->setValue(testName + TestConfig::DELIMETER + TestConfig::EXEPATH, path);
    config->setValue(testName + TestConfig::DELIMETER + TestConfig::SELECTEDCASES, testCases);
}

void TestConfig::removeUnselectedCases(const QString &testName)
{
    config->remove(testName);
    config->remove(testName + TestConfig::DELIMETER + TestConfig::EXEPATH);
    config->remove(testName + TestConfig::DELIMETER + TestConfig::SELECTEDCASES);
}

QString TestConfig::executableFile(const QString &testName)
{
    return config->value(testName + TestConfig::DELIMETER + TestConfig::EXEPATH).toString();
}

QStringList TestConfig::testNames()
{
    QStringList names = config->childGroups();
    names.removeAll(TestConfig::GLOBALCFG);
    return names;
}

void TestConfig::loadConfig()
{
    config = new QSettings(QSettings::IniFormat, QSettings::UserScope, TestConfig::ORG, TestConfig::APP, this);
}

void TestConfig::loadAlternativeConfig(const QString &path)
{
    config = new QSettings(path, QSettings::IniFormat, this);
}

void TestConfig::loadGlobalConfig()
{
    globalCfg = new GlobalConfig(this);
    QStringList sectionList = config->childGroups();
    if (sectionList.contains(TestConfig::GLOBALCFG)) {
        config->beginGroup(TestConfig::GLOBALCFG);
#if defined(Q_OS_LINUX)
        globalCfg->setCallGrind(config->value(GlobalConfig::CALLGRIND, false).toBool());
#endif
        globalCfg->setEventCounter(config->value(GlobalConfig::EVENTCOUNTER, false).toBool());
        globalCfg->setOutputFormat(config->value(GlobalConfig::OUTPUTFORMAT).toString());
        globalCfg->setOutputPath(config->value(GlobalConfig::OUTPUTPATH).toString());
        globalCfg->setFlush(config->value(GlobalConfig::FLUSH, false).toBool());
        globalCfg->setSilent(config->value(GlobalConfig::SILENT, false).toBool());
        globalCfg->setVerbose(config->value(GlobalConfig::VERBOSE).toString());
        globalCfg->setEventDelay(config->value(GlobalConfig::EVENTDELAY, -1).toInt());
        globalCfg->setKeyDelay(config->value(GlobalConfig::KEYDELAY, -1).toInt());
        globalCfg->setMouseDelay(config->value(GlobalConfig::MOUSEDELAY, -1).toInt());
        globalCfg->setKeyEventVerbose(config->value(GlobalConfig::KEYEVENTVERBOSE, false).toBool());
        globalCfg->setMaxWarnings(config->value(GlobalConfig::MAXWARNINGS, -1).toInt());
        globalCfg->setNoCrashHandler(config->value(GlobalConfig::NOCRASHHANDLER, false).toBool());
        globalCfg->setTickCounter(config->value(GlobalConfig::TICKCOUNTER, false).toBool());
        globalCfg->setMinimumValue(config->value(GlobalConfig::MINIMUMVALUE, -1).toInt());
        globalCfg->setIterations(config->value(GlobalConfig::ITERATIONS, -1).toInt());
        globalCfg->setMedian(config->value(GlobalConfig::MEDIAN, -1).toInt());
        globalCfg->setVerboseBenchMarking(config->value(GlobalConfig::VERBOSEBENCHMARK, false).toBool());
        config->endGroup();
    }
}

void TestConfig::saveGlobalConfig(const GlobalConfig &cfg)
{
    config->beginGroup(TestConfig::GLOBALCFG);
#if defined(Q_OS_LINUX)
    config->setValue(GlobalConfig::CALLGRIND, cfg.callGrind());
#endif
    config->setValue(GlobalConfig::EVENTCOUNTER, cfg.eventCounter());
    config->setValue(GlobalConfig::OUTPUTFORMAT, cfg.outputFormat());
    QString path = cfg.outputPath();
    if (path != "" && path.endsWith('/')) path.remove(path.length() - 1, 1);
    config->setValue(GlobalConfig::OUTPUTPATH, path);
    config->setValue(GlobalConfig::FLUSH, cfg.flush());
    config->setValue(GlobalConfig::SILENT, cfg.silent());
    config->setValue(GlobalConfig::VERBOSE, cfg.verbose());
    config->setValue(GlobalConfig::EVENTDELAY, cfg.eventDelay());
    config->setValue(GlobalConfig::KEYDELAY, cfg.keyDelay());
    config->setValue(GlobalConfig::MOUSEDELAY, cfg.mouseDelay());
    config->setValue(GlobalConfig::KEYEVENTVERBOSE, cfg.keyEventVerbose());
    config->setValue(GlobalConfig::MAXWARNINGS, cfg.maxWarnings());
    config->setValue(GlobalConfig::NOCRASHHANDLER, cfg.noCrashHandler());
    config->setValue(GlobalConfig::TICKCOUNTER, cfg.tickCounter());
    config->setValue(GlobalConfig::MINIMUMVALUE, cfg.minimumValue());
    config->setValue(GlobalConfig::ITERATIONS, cfg.iterations());
    config->setValue(GlobalConfig::MEDIAN, cfg.median());
    config->setValue(GlobalConfig::VERBOSEBENCHMARK, cfg.verboseBenchMarking());
    config->endGroup();
}

// Constants
const QString GlobalConfig::OUTPUTFORMAT = tr("outputformat");
const QString GlobalConfig::OUTPUTPATH = tr("outputpath");
const QString GlobalConfig::FLUSH = tr("flush");
const QString GlobalConfig::SILENT = tr("silent");
const QString GlobalConfig::VERBOSE = tr("verbose");
const QString GlobalConfig::EVENTDELAY = tr("eventdelay");
const QString GlobalConfig::KEYDELAY = tr("keydelay");
const QString GlobalConfig::MOUSEDELAY = tr("mousedelay");
const QString GlobalConfig::KEYEVENTVERBOSE = tr("keyevent-verbose");
const QString GlobalConfig::MAXWARNINGS = tr("maxwarnings");
const QString GlobalConfig::NOCRASHHANDLER = tr("nocrashhandler");
const QString GlobalConfig::CALLGRIND = tr("callgrind");
const QString GlobalConfig::TICKCOUNTER = tr("tickcounter");
const QString GlobalConfig::EVENTCOUNTER = tr("eventcounter");
const QString GlobalConfig::MINIMUMVALUE = tr("minimumvalue");
const QString GlobalConfig::ITERATIONS = tr("iterations");
const QString GlobalConfig::MEDIAN = tr("median");
const QString GlobalConfig::VERBOSEBENCHMARK = tr("vb");

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

GlobalConfig::GlobalConfig(const GlobalConfig &cfg):
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
    if (this->m_callGrind) str += " " + TestConfig::OPTCHAR + GlobalConfig::CALLGRIND;
#endif
    if (this->m_eventCounter) str += " " + TestConfig::OPTCHAR + GlobalConfig::EVENTCOUNTER;
    if (this->m_outputFormat != "")  str += " " + TestConfig::OPTCHAR + this->m_outputFormat;
    if (this->m_flush) str += " " + TestConfig::OPTCHAR + GlobalConfig::FLUSH;
    if (this->m_silent) str += " " + TestConfig::OPTCHAR + GlobalConfig::SILENT;
    if (this->m_verbose != "") str += " " + TestConfig::OPTCHAR + this->m_verbose;
    if (this->m_eventDelay != -1) str += " "
                                         + TestConfig::OPTCHAR
                                         + GlobalConfig::EVENTDELAY
                                         + TestConfig::INTARG.arg(this->m_eventDelay);
    if (this->m_keyDelay != -1)  str += " "
                                        + TestConfig::OPTCHAR
                                        + GlobalConfig::KEYDELAY
                                        + TestConfig::INTARG.arg(this->m_keyDelay);
    if (this->m_mouseDelay != -1) str += " "
                                         + TestConfig::OPTCHAR
                                         + GlobalConfig::MOUSEDELAY
                                         + TestConfig::INTARG.arg(this->m_mouseDelay);
    if (this->m_keyEventVerbose) str += " " + TestConfig::OPTCHAR + GlobalConfig::KEYEVENTVERBOSE;
    if (this->m_maxWarnings != -1) str += " "
                                          + TestConfig::OPTCHAR
                                          + GlobalConfig::MAXWARNINGS
                                          + TestConfig::INTARG.arg(this->m_maxWarnings);
    if (this->m_noCrashHandler) str += " " + TestConfig::OPTCHAR + GlobalConfig::NOCRASHHANDLER;
    if (this->m_tickCounter) str += " " + TestConfig::OPTCHAR + GlobalConfig::TICKCOUNTER;
    if (this->m_minimumValue != -1) str += " "
                                           + TestConfig::OPTCHAR
                                           + GlobalConfig::MINIMUMVALUE
                                           + TestConfig::INTARG.arg(this->m_minimumValue);
    if (this->m_iterations != -1) str += " "
                                         + TestConfig::OPTCHAR
                                         + GlobalConfig::ITERATIONS
                                         + TestConfig::INTARG.arg(this->m_iterations);
    if (this->m_median != -1) str += " "
                                     + TestConfig::OPTCHAR
                                     + GlobalConfig::MEDIAN
                                     + TestConfig::INTARG.arg(this->m_median);
    if (this->m_verboseBenchMarking) str += " " + TestConfig::OPTCHAR + GlobalConfig::VERBOSEBENCHMARK;
    return str;
}

QString GlobalConfig::outputPath() const
{
    QString str;
    if (this->m_outputPath != "") {
        QDir path(this->m_outputPath);
        if (!path.exists()) {
            if (!path.mkpath(this->m_outputPath))
                return "";
        }
        str += this->m_outputPath + TestConfig::PATHDELI;
    }
    return str;
}

bool GlobalConfig::equalsTo(const GlobalConfig *cfg)
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

GlobalConfig &GlobalConfig::operator=(const GlobalConfig &other)
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

QT_END_NAMESPACE

