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

#include "inc/tabwidget.h"
#include "inc/version.h"
#include <QFileInfo>

#if defined(Q_OS_SYMBIAN)
#include <unistd.h>
#endif

QT_BEGIN_NAMESPACE

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    tabCase(NULL),
    treeCase(NULL),
    tabRun(NULL),
    textRun(NULL),
    passRun(NULL),
    failRun(NULL),
    skipRun(NULL),
    progressRun(NULL),
    tabReport(NULL),
    tableReport(NULL),
    runner(NULL),
    tests(NULL),
    testCfg(NULL)
{
    setContextMenuPolicy(Qt::NoContextMenu);

    this->tabBar()->setExpanding(true);
    loadWidgets();

    loadPopulatedTests();
}

TabWidget::~TabWidget()
{
}

void TabWidget::loadWidgets()
{
    loadTabCase();
    loadTabRun();
    loadTabReport();
}

void TabWidget::loadTabCase()
{
    tabCase = new QWidget();
    this->addTab(tabCase, tr("  Case  "));

    loadTreeCase();
}

void TabWidget::loadTabRun()
{
    tabRun = new QWidget();
    this->addTab(tabRun, tr("  Run   "));

    loadTextRun();
}

void TabWidget::loadTabReport()
{
    tabReport = new QWidget();
    this->addTab(tabReport, tr(" Report "));

    loadTableReport();
}

void TabWidget::loadTreeCase()
{

    treeCase = new QTreeWidget(tabCase);
    treeCase->setObjectName(QString::fromUtf8("treeCase"));
    treeCase->headerItem()->setText(0, "");

    QGridLayout *treeLayout = new QGridLayout(tabCase);
    treeLayout->setContentsMargins(0, 0, 0, 0);
    treeLayout->setObjectName(QString::fromUtf8("treeLayout"));

    treeLayout->addWidget(treeCase, 0, 0, 1, 1);
    tabCase->setLayout(treeLayout);
}

void TabWidget::loadTextRun()
{

    QGridLayout *textLayout = new QGridLayout(tabRun);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setObjectName(QString::fromUtf8("textLayout"));

    textRun = new QTextEdit(tabRun);
    textRun->setReadOnly(true);
    textRun->setTextInteractionFlags(Qt::NoTextInteraction);
    textRun->setObjectName(QString::fromUtf8("textRun"));
    textLayout->addWidget(textRun, 0, 0, 1, -1);

    textLayout->addWidget(new QLabel(tr("passed:"), this), 1, 0, 1, 1);
    passRun = new QLabel(this);
    textLayout->addWidget(passRun, 1, 1, 1, 1);
    textLayout->addWidget(new QLabel(tr("failed:"), this), 1, 2, 1, 1);
    failRun = new QLabel(this);
    textLayout->addWidget(failRun, 1, 3, 1, 1);
    textLayout->addWidget(new QLabel(tr("skipped:"), this), 1, 4, 1, 1);
    skipRun = new QLabel(this);
    textLayout->addWidget(skipRun, 1, 5, 1, 1);

    progressRun = new QProgressBar(tabRun);
    progressRun->setFixedHeight(20);
    progressRun->setTextVisible(true);
    progressRun->setRange(0, 1);
    progressRun->setValue(0);
    textLayout->addWidget(progressRun, 2, 0, 1, -1);
}

void TabWidget::loadTableReport()
{
    tableReport = new QTableWidget(tabReport);
    tableReport->setObjectName(QString::fromUtf8("tableReport"));

    QGridLayout *tableLayout = new QGridLayout(tabReport);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setObjectName(QString::fromUtf8("tableLayout"));

    tableLayout->addWidget(tableReport, 0, 0, 1, 1);
    tabReport->setLayout(tableLayout);
}

void TabWidget::loadPopulatedTests()
{
    QString cfgFile = findConfigFile();
    if (cfgFile == "")
        testCfg = new TestConfig(this);
    else
        testCfg = new TestConfig(cfgFile, this);

    tests = new TestFileCaseList(this);
    foreach (QString name, testCfg->testNames()) {
        QString execFileName = testCfg->executableFile(name);
        if (execFileName == "") {
            addToMissingMandatoryParamList(name + "/path");
        } else {
            tests->addItem(execFileName, testCfg->selectedCases(name));
            // Populated test executable not found.
            if (!checkFileExistence(execFileName))
                addToNonExistentFileList(execFileName);
        }
    }

    showTests();
    fetchAllCases();
    showNonExistentErrDlg(nonExistentFiles, tr("File(s) not found:"));
    showNonExistentErrDlg(nonExistentCases, tr("Case(s) not found:"));
    showNonExistentErrDlg(missingMandatoryParams, tr("Mandatory param(s) not found:"));
}

void TabWidget::showTests()
{
    foreach (QString path, tests->getPaths()) {
        QString baseName = tests->getBaseName(path);
        if (treeCase->findItems(baseName, Qt::MatchExactly).isEmpty()
            && !nonExistentFiles.contains(path)) {
            QTreeWidgetItem *testItem = new QTreeWidgetItem(treeCase);
            testItem->setFlags(testItem->flags()
                               | Qt::ItemIsEnabled
                               | Qt::ItemIsSelectable
                               | Qt::ItemIsUserCheckable
                               | Qt::ItemIsTristate);
            testItem->setText(0, baseName);

            QString functions = tests->getFunctions(path).trimmed();
            QStringList funcList = functions.split(QRegExp("\\s+"));
            funcList.removeAll("");
            QStringList caseList = fetchCases(path).split(QRegExp("\\s+"));

            foreach (QString name, caseList) {
                if (!name.isEmpty()) {
                    QTreeWidgetItem *functionItem = new QTreeWidgetItem(testItem);
                    functionItem->setFlags(functionItem->flags()
                                           | Qt::ItemIsEnabled
                                           | Qt::ItemIsSelectable
                                           | Qt::ItemIsUserCheckable
                                           | Qt::ItemIsTristate);
                    QString caseName = name.section('(', 0, 0);
                    functionItem->setText(0, caseName);
                    if (functions.isEmpty() || funcList.contains(caseName) || funcList.contains(name)) {
                        functionItem->setCheckState(0, Qt::Checked);
                        // Keep non-existent cases in the list.
                        if (!funcList.removeOne(name))
                            funcList.removeOne(caseName);
                    } else
                        functionItem->setCheckState(0, Qt::Unchecked);
                }
            }

            foreach (QString nonExistentCase, funcList) {
                addToNonExistentCaseList(baseName + "::" + nonExistentCase.section('(', 0, 0));
            }
        }
    }
}

void TabWidget::fetchAllCases()
{
    foreach (QString path, tests->getPaths()) {
        if (tests->getFunctions(path).isEmpty())
            tests->addFunc(path, fetchCases(path));
    }
}

QString TabWidget::fetchCases(QString path)
{
    if (path.isEmpty())
        return QString();

#if defined(Q_OS_SYMBIAN)
    QString filePath = tests->getBaseName(path);
#else
    QString filePath = path;
#endif
    filePath += " -functions";

    PipsProcess *cmdProcess = new PipsProcess();
#if defined(Q_OS_SYMBIAN)
    cmdProcess->setUsePips(true);
#endif
    cmdProcess->start(filePath);
    cmdProcess->waitForFinished();
    QString functions = cmdProcess->readAllStandardOutput();
    delete cmdProcess;

    stripAdditionalInfo(functions);
    return functions;
}

void TabWidget::addTest()
{
    if (currentPath.isEmpty())
        currentPath = QDir::currentPath();

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Select test"),
        currentPath,
#if defined(Q_OS_LINUX)
        tr("All files (*);;Test (*.exe)") );
#else
        tr("Test (*.exe);;All files (*.*);;All (*)") );
#endif
    if (filePath.isNull())
        return;

    currentPath = QDir(filePath).absolutePath();

    tests->addItem(filePath, fetchCases(filePath));

    showTests();
    this->setCurrentWidget(tabCase);
    emit menuStateChanged(MainWindow::EReady);
}

void TabWidget::runTest()
{
    this->setCurrentWidget(tabRun);

    if (runner != NULL) {
        if ((runner->testState() == TestRunner::EPaused)
            || (runner->testState() == TestRunner::ERunning)) {
            runner->start();
            return;
        } else {
            delete runner;
            runner = NULL;
        }
    }

    runner = new TestRunner(this);

    if (addSelectedTestCases() == 0) {
        delete runner;
        runner = NULL;
        QMessageBox::warning(this, tr("Attention"), tr("No case selected"));
        this->setCurrentWidget(tabCase);
        return;
    }

    runner->setTestConfig(testCfg);

    TestResult *result = new TestResult(runner);
    result->setTextWidget(textRun);
    result->setProgressWidget(progressRun);
    result->setProgressTextWidget(passRun, failRun, skipRun);
    result->setReportWidget(tableReport);
    runner->setTestResult(result);

    connect(runner, SIGNAL(started()), this, SLOT(runnerStarted()));
    connect(runner, SIGNAL(finished()), this, SLOT(runnerStoped()));
    connect(runner, SIGNAL(stoped()), this, SLOT(runnerStoped()));
    connect(runner, SIGNAL(paused()), this, SLOT(runnerPaused()));

    runner->start();
}

void TabWidget::stopTest()
{
    runner->stop();
}

void TabWidget::pauseTest()
{
    runner->pause();
}

void TabWidget::about()
{
    QMessageBox::information(this,
                             tr("QTestUI"),
                             tr("QTestUI %1.%2.%3")
                                .arg(MAJOR_VERSION)
                                .arg(MINOR_VERSION)
                                .arg(BUILD_VERSION));
}

void TabWidget::runnerStarted(){
    emit menuStateChanged(MainWindow::ERunning);
}

void TabWidget::runnerStoped(){
    emit menuStateChanged(MainWindow::EReady);
}

void TabWidget::runnerPaused(){
    emit menuStateChanged(MainWindow::EPaused);
}

void TabWidget::loadOutputSettings()
{
    emit outputSettingsDlgDataLoaded(const_cast<GlobalConfig *>(testCfg->globalConfig()));
}

void TabWidget::loadEventSettings()
{
    emit eventSettingsDlgDataLoaded(const_cast<GlobalConfig *>(testCfg->globalConfig()));
}

void TabWidget::loadBMSettings()
{
    emit bmSettingsDlgDataLoaded(const_cast<GlobalConfig *>(testCfg->globalConfig()));
}

void TabWidget::saveCases()
{
    for (int i = 0; i < treeCase->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = treeCase->topLevelItem(i);
        Qt::CheckState checkState = item->checkState(0);
        if (checkState == Qt::Checked || checkState == Qt::PartiallyChecked) {
            QString cases;
            for (int j = 0; j < item->childCount(); ++j) {
                QTreeWidgetItem *child = item->child(j);
                if (child->checkState(0) == Qt::Checked) {
                    cases += child->text(0) + " ";
                }
            }
            cases = cases.trimmed();
            testCfg->saveSelectedCases(item->text(0), tests->getPath(item->text(0)), cases);
        } else {
            // Remove unchecked cases if saved previously
            testCfg->removeUnselectedCases(item->text(0));
        }
    }
}

void TabWidget::saveSettings(const GlobalConfig &config)
{
    testCfg->saveGlobalConfig(config);
}

void TabWidget::stripAdditionalInfo(QString &
#if defined(Q_OS_LINUX)
        testCases
#endif
        )
{
#if defined(Q_OS_LINUX)
    const QString tag = "Available testfunctions:\n";
    int pos = testCases.lastIndexOf(tag);
    testCases.remove(0, pos + tag.length());
#endif
}

int TabWidget::addSelectedTestCases()
{
    if (runner == NULL)
        return 0;

    int count = 0;
    for (int i = 0; i < treeCase->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = treeCase->topLevelItem(i);
        Qt::CheckState chkState = item->checkState(0);
        if (chkState == Qt::Checked || chkState == Qt::PartiallyChecked) {
            ++count;
            TestCase *testCase = new TestCase();
            testCase->setFullPath(tests->getPath(item->text(0)));
            testCase->setName(item->text(0));
            for (int j = 0; j < item->childCount(); ++j) {
                QTreeWidgetItem *child = item->child(j);
                Qt::CheckState childChkState = child->checkState(0);
                if (childChkState == Qt::Checked || childChkState == Qt::PartiallyChecked)
                    testCase->addFunction(child->text(0));
            }
            runner->addTestCase(testCase);
        }
    }
    return count;
}

QString TabWidget::findConfigFile()
{
#if defined (Q_OS_SYMBIAN)
    QStringList listOfLocations;
    listOfLocations << "E:/QTestUI.ini"
            << "F:/QTestUI.ini"
            << "C:/QTestUI.ini";
    foreach (QString temp, listOfLocations) {
        if (QFile::exists(temp))
            return temp;
    }
    return QString();
#endif

    const static QString LOC_CURRENT_DIR = "./QTestUI.ini";
    if (QFile::exists(LOC_CURRENT_DIR))
        return LOC_CURRENT_DIR;
    else
        return QString();
}

bool TabWidget::checkFileExistence(const QString &fileName)
{
    return QFile::exists(fileName);
}

void TabWidget::addToNonExistentFileList(QString fileName)
{
    nonExistentFiles << fileName;
}

void TabWidget::addToNonExistentCaseList(QString qualifiedCaseName)
{
    nonExistentCases << qualifiedCaseName;
}

void TabWidget::addToMissingMandatoryParamList(QString paramName)
{
    missingMandatoryParams << paramName;
}

void TabWidget::showNonExistentErrDlg(QStringList nonExistentNames, QString errMsg)
{
    if (!nonExistentNames.isEmpty()) {
        QString msg = errMsg;
        foreach (QString name, nonExistentNames)
            msg += tr("\n") + tr("\"") + name + tr("\"");
        QMessageBox::critical(this, tr("ERROR"), msg);
    }
}

TestFileCaseList::TestFileCaseList(QObject *parent):
        QObject(parent)
{
}

void TestFileCaseList::addItem(QString path, QString functions)
{
    QMap<QString, QString>::iterator iter = fileCaseList.find(path);
    if (iter != fileCaseList.end())
        iter.value() += " " + functions;
    else
        fileCaseList[path] = functions;
}

void TestFileCaseList::addFunc(QString path, QString function)
{
    QMap<QString, QString>::iterator iter = fileCaseList.find(path);
    if (iter != fileCaseList.end())
        iter.value() = function;
}

void TestFileCaseList::removeItem(QString path)
{
    fileCaseList.remove(path);
}

void TestFileCaseList::removeFunc(QString path, QString function)
{
    QMap<QString, QString>::iterator iter = fileCaseList.find(path);
    if (iter != fileCaseList.end())
        iter.value().remove(function);
}

QString TestFileCaseList::getFunctions(QString path)
{
    QMap<QString, QString>::iterator iter = fileCaseList.find(path);
    if (iter != fileCaseList.end())
        return iter.value();
    else
        return QString();
}

QStringList TestFileCaseList::getPaths()
{
    return fileCaseList.keys();
}

QString TestFileCaseList::getBaseName(QString path)
{
    QFileInfo fi(path);
    return fi.fileName();
}

QString TestFileCaseList::getPath(QString baseName)
{
    foreach (QString path, fileCaseList.keys()) {
        if (baseName == getBaseName(path))
            return path;
    }
    return QString();
}

QT_END_NAMESPACE
