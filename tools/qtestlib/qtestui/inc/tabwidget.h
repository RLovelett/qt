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

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QtGui>
#include <QMap>
#include "mainwindow.h"
#include "testrunner.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class TestFileCaseList;
class TestConfig;
class GlobalConfig;

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent = 0);
    virtual ~TabWidget();

private:
    void loadWidgets();
    void loadTabCase();
    void loadTabRun();
    void loadTabReport();
    void loadTreeCase();
    void loadTextRun();
    void loadTableReport();
    void loadPopulatedTests();
    void showTests();
    void fetchAllCases();
    QString fetchCases(QString path);
    int addSelectedTestCases();

    // helpers.
    void stripAdditionalInfo(QString &testCases);
    QString findConfigFile();
    bool checkFileExistence(const QString &fileName);
    void addToNonExistentFileList(QString fileName);
    void addToNonExistentCaseList(QString qualifiedCaseName);
    void addToMissingMandatoryParamList(QString paramName);
    void showNonExistentErrDlg(QStringList nonExistentNames, QString errMsg);

private slots:
    void addTest();
    void runTest();
    void stopTest();
    void pauseTest();
    void about();

public slots:
    void runnerStarted();
    void runnerStoped();
    void runnerPaused();
    void loadOutputSettings();
    void saveSettings(const GlobalConfig &config);
    void loadEventSettings();
    void loadBMSettings();
    void saveCases();

signals:
    void menuStateChanged(MainWindow::MenuBarState state);
    void outputSettingsDlgDataLoaded(GlobalConfig *config);
    void eventSettingsDlgDataLoaded(GlobalConfig *config);
    void bmSettingsDlgDataLoaded(GlobalConfig *config);

private:
    //Tab Case
    QWidget     *tabCase;
    QTreeWidget *treeCase;

    //Tab Run
    QWidget             *tabRun;
    QTextEdit           *textRun;
    QLabel              *passRun;
    QLabel              *failRun;
    QLabel              *skipRun;
    QProgressBar        *progressRun;

    //Tab Report
    QWidget             *tabReport;
    QTableWidget        *tableReport;

    TestRunner          *runner;
    QString             testOutput;
    QStringList         nonExistentFiles;
    QStringList         nonExistentCases;
    QStringList         missingMandatoryParams;
    TestFileCaseList    *tests;
    TestConfig          *testCfg;
    QString             currentPath;

};

class TestFileCaseList : public QObject
{
    Q_OBJECT

public:
    TestFileCaseList(QObject *parent = 0);
    void addItem(QString path, QString functions);
    void addFunc(QString path, QString function);
    void removeItem(QString path);
    void removeFunc(QString path, QString function);
    QString getFunctions(QString path);
    QStringList getPaths();
    QString getBaseName(QString path);
    QString getPath(QString baseName);

private:
    QMap<QString, QString> fileCaseList;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // TABWIDGET_H
