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

#include <QMenu>
#include "inc/mainwindow.h"
#include "inc/tabwidget.h"
#include "inc/outputoptdlg.h"
#include "inc/eventoptdlg.h"
#include "inc/benchmarkoptdlg.h"

QT_BEGIN_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    outputOptDlg(NULL),
    eventOptDlg(NULL),
    benchmarkOptDlg(NULL),
    saveCasesAction(NULL)
{
    setWindowTitle(tr("QTestUI"));

    loadWidgets();

    createMenus();
    loadMenus();
    setMenuBar(EInit);

    loadStyleSheet();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenus()
{
    connect(tabWidget, SIGNAL(menuStateChanged(MainWindow::MenuBarState)),
            this, SLOT(setMenuBar(MainWindow::MenuBarState)));

    addAction = new QAction(tr("Add Test"), this);
    connect(addAction, SIGNAL(triggered()), tabWidget, SLOT(addTest()));

    runAction = new QAction(tr("Run"), this);
    connect(runAction, SIGNAL(triggered()), tabWidget, SLOT(runTest()));

    settingsMenu = new QMenu(tr("Settings"), this);

    outputOptAction = new QAction(tr("Output Settings..."), settingsMenu);
    connect(outputOptAction, SIGNAL(triggered()), tabWidget, SLOT(loadOutputSettings()));
    connect(tabWidget, SIGNAL(outputSettingsDlgDataLoaded(GlobalConfig *)),
            this, SLOT(openOutputSettingsDlg(GlobalConfig *)));

    eventOptAction = new QAction(tr("Event Settings..."), settingsMenu);
    connect(eventOptAction, SIGNAL(triggered()), tabWidget, SLOT(loadEventSettings()));
    connect(tabWidget, SIGNAL(eventSettingsDlgDataLoaded(GlobalConfig *)),
            this, SLOT(openEventSettingsDlg(GlobalConfig *)));

    benchmarkOptActoin = new QAction(tr("Benchmark Settings..."), settingsMenu);
    connect(benchmarkOptActoin, SIGNAL(triggered()), tabWidget, SLOT(loadBMSettings()));
    connect(tabWidget, SIGNAL(bmSettingsDlgDataLoaded(GlobalConfig *)),
            this, SLOT(openBMSettingsDlg(GlobalConfig *)));

    stopAction = new QAction(tr("Stop"), this);
    connect(stopAction, SIGNAL(triggered()), tabWidget, SLOT(stopTest()));

    pauseAction = new QAction(tr("Pause"), this);
    connect(pauseAction, SIGNAL(triggered()), tabWidget, SLOT(pauseTest()));

    saveCasesAction = new QAction(tr("SaveCases"), this);
    connect(saveCasesAction, SIGNAL(triggered()), tabWidget, SLOT(saveCases()));

    aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, SIGNAL(triggered()), tabWidget, SLOT(about()));

    exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, SIGNAL(triggered()), tabWidget, SLOT(saveCases()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::loadMenus()
{
    menuBar()->clear();
    menuBar()->setContextMenuPolicy(Qt::NoContextMenu);
    menuBar()->addAction(addAction);
    menuBar()->addAction(runAction);
    settingsMenu->addAction(outputOptAction);
    settingsMenu->addAction(eventOptAction);
    settingsMenu->addAction(benchmarkOptActoin);
    menuBar()->addAction(settingsMenu->menuAction());
    menuBar()->addAction(stopAction);
    menuBar()->addAction(pauseAction);
    menuBar()->addAction(saveCasesAction);

    menuBar()->addAction(aboutAction);
    menuBar()->addAction(exitAction);
}

void MainWindow::setMenuBar(MainWindow::MenuBarState state)
{
    switch (state) {
    case EInit:
        addAction->setVisible(true);
        runAction->setVisible(true);
        saveCasesAction->setVisible(true);
        stopAction->setVisible(false);
        pauseAction->setVisible(false);
        break;
    case EReady:
        addAction->setVisible(true);
        runAction->setVisible(true);
        stopAction->setVisible(false);
        pauseAction->setVisible(false);
        break;
    case ERunning:
        addAction->setVisible(false);
        runAction->setVisible(false);
        stopAction->setVisible(true);
        pauseAction->setVisible(true);
        break;
    case EPaused:
        addAction->setVisible(false);
        runAction->setVisible(true);
        stopAction->setVisible(true);
        pauseAction->setVisible(false);
        break;
    }
}

void MainWindow::openOutputSettingsDlg(GlobalConfig *config)
{
    if (outputOptDlg == NULL) {
        outputOptDlg = new OutputOptDlg(config, this);
        connect(outputOptDlg, SIGNAL(outputOptSaved(const GlobalConfig &)),
                tabWidget, SLOT(saveSettings(const GlobalConfig &)));
    }
    outputOptDlg->exec();
    outputOptDlg = NULL; // The dialog is deleted upon close.
}

void MainWindow::openEventSettingsDlg(GlobalConfig *config)
{
    if (eventOptDlg == NULL) {
        eventOptDlg = new EventOptDlg(config, this);
        connect(eventOptDlg, SIGNAL(eventOptSaved(const GlobalConfig &)),
                tabWidget, SLOT(saveSettings(const GlobalConfig &)));
    }
    eventOptDlg->exec();
    eventOptDlg = NULL; // The dialog is deleted upon close.
}

void MainWindow::openBMSettingsDlg(GlobalConfig *config)
{
    if (benchmarkOptDlg == NULL) {
        benchmarkOptDlg = new BenchmarkOptDlg(config, this);
        connect(benchmarkOptDlg, SIGNAL(bmOptSaved(const GlobalConfig &)),
                tabWidget, SLOT(saveSettings(const GlobalConfig &)));
    }
    benchmarkOptDlg->exec();
    benchmarkOptDlg = NULL; // The dialog is deleted upon close.
}

void MainWindow::loadWidgets()
{
    QWidget *centralWidget;
    centralWidget = new QWidget(this);
    centralWidget->setContextMenuPolicy(Qt::NoContextMenu);
    setCentralWidget(centralWidget);

    QGridLayout *tabLayout = new QGridLayout(centralWidget);
    tabLayout->setContentsMargins(6, 6, 6, 6);
    tabLayout->setObjectName(QString::fromUtf8("mainLayout"));

    tabWidget = new TabWidget(centralWidget);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));

    tabLayout->addWidget(tabWidget, 0, 0, 1, 1);
    centralWidget->setLayout(tabLayout);
}

void MainWindow::loadStyleSheet()
{
    QFile file(":/qss/blue.qss");

    if (file.open(QFile::ReadOnly)) {
        QString style = file.readAll();
        qApp->setStyleSheet(style);
        file.close();
    }
}

QT_END_NAMESPACE

