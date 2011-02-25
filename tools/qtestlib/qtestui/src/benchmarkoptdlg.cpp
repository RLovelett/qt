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

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QDir>
#include "inc/benchmarkoptdlg.h"
#include "inc/testconfig.h"

QT_BEGIN_NAMESPACE

BenchmarkOptDlg::BenchmarkOptDlg(GlobalConfig *config, QWidget *parent) :
        QDialog(parent),
        initConfig(config),
        savedConfig(NULL)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(tr("Benchmark Settings"));
    load();
    setLayout();

    if (initConfig != NULL) {
        savedConfig = new GlobalConfig(*initConfig);
        savedConfig->setParent(this);
    } else {
        savedConfig = new GlobalConfig(this);
        initConfig = new GlobalConfig(*savedConfig);
        initConfig->setParent(this);
    }

    this->setAttribute(Qt::WA_DeleteOnClose);
}

BenchmarkOptDlg::~BenchmarkOptDlg()
{
}

void BenchmarkOptDlg::createWidgets()
{
    checkboxVerbose = new QCheckBox(GlobalConfig::VERBOSEBENCHMARK, this);
    checkboxVerbose->setChecked(initConfig != NULL && initConfig->verboseBenchMarking());

    checkboxTickCounter = new QCheckBox(GlobalConfig::TICKCOUNTER, this);
    checkboxTickCounter->setChecked(initConfig != NULL && initConfig->tickCounter());

    checkboxEventCounter = new QCheckBox(GlobalConfig::EVENTCOUNTER, this);
    checkboxEventCounter->setChecked(initConfig != NULL && initConfig->eventCounter());

    checkboxMinValue = new QCheckBox(GlobalConfig::MINIMUMVALUE + ": ", this);
    checkboxMinValue->setChecked(initConfig != NULL && initConfig->minimumValue() != -1);
    lineEditMinVal = new QLineEdit(this);
    lineEditMinVal->setEnabled(checkboxMinValue->isChecked());
    if (lineEditMinVal->isEnabled())
        lineEditMinVal->setText(tr("%1").arg(initConfig->minimumValue()));

    checkboxIterations = new QCheckBox(GlobalConfig::ITERATIONS, this);
    checkboxIterations->setChecked(initConfig != NULL && initConfig->iterations() != -1);
    lineEditIterations = new QLineEdit(this);
    lineEditIterations->setEnabled(checkboxIterations->isChecked());
    if (lineEditIterations->isEnabled())
        lineEditIterations->setText(tr("%1").arg(initConfig->iterations()));

    checkboxMedian = new QCheckBox(GlobalConfig::MEDIAN, this);
    checkboxMedian->setChecked(initConfig != NULL && initConfig->median() != -1);
    lineEditMedian = new QLineEdit(this);
    lineEditMedian->setEnabled(checkboxMedian->isChecked());
    if (lineEditMedian->isEnabled())
        lineEditMedian->setText(tr("%1").arg(initConfig->median()));

#if defined (Q_OS_LINUX)
    checkboxCallGrind = new QCheckBox(GlobalConfig::CALLGRIND, this);
    checkboxCallGrind->setChecked(initConfig != NULL && initConfig->callGrind());
#endif

    buttonSave = new QPushButton(tr("Save"), this);
    buttonSave->setEnabled(false);
    buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonHelp = new QPushButton(tr("Help"), this);
}

void BenchmarkOptDlg::setupWidgetsEventHandlers()
{
    connect(checkboxVerbose, SIGNAL(stateChanged(int)), this, SLOT(changeVbBenchmarkState(int)));
    connect(checkboxTickCounter, SIGNAL(stateChanged(int)), this, SLOT(changeTickCounterState(int)));
    connect(checkboxEventCounter, SIGNAL(stateChanged(int)), this, SLOT(changeEventCounterState(int)));
    connect(checkboxMinValue, SIGNAL(stateChanged(int)), this, SLOT(changeMinValState(int)));
    connect(checkboxIterations, SIGNAL(stateChanged(int)), this, SLOT(changeIterState(int)));
    connect(checkboxMedian, SIGNAL(stateChanged(int)), this, SLOT(changeMedianState(int)));
#if defined (Q_OS_LINUX)
    connect(checkboxCallGrind, SIGNAL(stateChanged(int)), this, SLOT(changeCallGrindState(int)));
#endif
    connect(lineEditMinVal, SIGNAL(textChanged(QString)), this, SLOT(changeMinVal(QString)));
    connect(lineEditIterations, SIGNAL(textChanged(QString)), this, SLOT(changeIter(QString)));
    connect(lineEditMedian, SIGNAL(textChanged(QString)), this, SLOT(changeMedian(QString)));
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(saveOpt()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(closeDlg()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void BenchmarkOptDlg::load()
{
    createWidgets();
    setupWidgetsEventHandlers();
}

void BenchmarkOptDlg::setLayout()
{
    QWidget *hbWidget1 = new QWidget(this);
    QHBoxLayout *layout1 = new QHBoxLayout(hbWidget1);
    layout1->addWidget(checkboxTickCounter);
    layout1->addWidget(checkboxEventCounter);
    hbWidget1->setLayout(layout1);

    QWidget *hbWidget2 = new QWidget(this);
    QHBoxLayout *layout2 = new QHBoxLayout(hbWidget2);
    layout2->addWidget(checkboxVerbose);
    layout2->addWidget(checkboxIterations);
    layout2->addWidget(lineEditIterations);
    hbWidget2->setLayout(layout2);

    QWidget *hbWidget3 = new QWidget(this);
    QHBoxLayout *layout3 = new QHBoxLayout(hbWidget3);
    layout3->addWidget(checkboxMedian);
    layout3->addWidget(lineEditMedian);
#if defined (Q_OS_LINUX)
    layout3->addWidget(checkboxCallGrind);
#endif
    hbWidget3->setLayout(layout3);

    QWidget *hbWidget4 = new QWidget(this);
    QHBoxLayout *layout4 = new QHBoxLayout(hbWidget4);
    layout4->addWidget(checkboxMinValue);
    layout4->addWidget(lineEditMinVal);
    hbWidget4->setLayout(layout4);

    QWidget *btnLayoutWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnLayoutWidget);
    btnLayout->addWidget(buttonSave);
    btnLayout->addWidget(buttonCancel);
    btnLayout->addWidget(buttonHelp);
    btnLayoutWidget->setLayout(btnLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(hbWidget1);
    mainLayout->addWidget(hbWidget2);
    mainLayout->addWidget(hbWidget3);
    mainLayout->addWidget(hbWidget4);
    mainLayout->addWidget(btnLayoutWidget);
}

void BenchmarkOptDlg::changeBtnSaveEnableState()
{
    buttonSave->setEnabled(!initConfig->equalsTo(savedConfig));
}

void BenchmarkOptDlg::changeVbBenchmarkState(int state)
{
    savedConfig->setVerboseBenchMarking(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeTickCounterState(int state)
{
    savedConfig->setTickCounter(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeEventCounterState(int state)
{
    savedConfig->setEventCounter(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMinValState(int state)
{
    lineEditMinVal->setEnabled(state == Qt::Checked);
    if (!lineEditMinVal->isEnabled()) {
        lineEditMinVal->clear();
        savedConfig->setMinimumValue(-1);
    } else {
        if (initConfig->minimumValue() != -1) {
            lineEditMinVal->setText(tr("%1").arg(initConfig->minimumValue()));
            savedConfig->setMinimumValue(initConfig->minimumValue());
        }
    }
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeIterState(int state)
{
    lineEditIterations->setEnabled(state == Qt::Checked);
    if (!lineEditIterations->isEnabled()) {
        lineEditIterations->clear();
        savedConfig->setIterations(-1);
    } else {
        if (initConfig->iterations() != -1) {
            lineEditIterations->setText(tr("%1").arg(initConfig->iterations()));
            savedConfig->setIterations(initConfig->iterations());
        }
    }
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMedianState(int state)
{
    lineEditMedian->setEnabled(state == Qt::Checked);
    if (!lineEditMedian->isEnabled()) {
        lineEditMedian->clear();
        savedConfig->setMedian(-1);
    } else {
        if (initConfig->median() != -1) {
            lineEditMedian->setText(tr("%1").arg(initConfig->median()));
            savedConfig->setMedian(initConfig->median());
        }
    }
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMinVal(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedConfig->setMinimumValue(val);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeIter(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedConfig->setIterations(val);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMedian(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedConfig->setMedian(val);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeCallGrindState(int
#if defined(Q_OS_LINUX)
        state
#endif
        )
{
#if defined (Q_OS_LINUX)
    savedConfig->setCallGrind(state == Qt::Checked);
    changeBtnSaveEnableState();
#endif
}

void BenchmarkOptDlg::saveOpt()
{
    *initConfig = *savedConfig;
    changeBtnSaveEnableState();
    emit bmOptSaved(*savedConfig);
    closeDlg();
}

void BenchmarkOptDlg::closeDlg()
{
    this->close();
}

void BenchmarkOptDlg::showHelp()
{
    const static QString msg = tr("tickcounter: Use CPU tick counters to time benchmarks.\n")
#if defined (Q_OS_LINUX)
                               + tr("callgrind: Use callgrind to time benchmarks.\n")
#endif
                               + tr("eventcounter: Counts events received during benchmarks.\n")
                               + tr("minimumvalue: Minimum acceptable measurement value.\n")
                               + tr("iterations: The number of accumulation iterations.\n")
                               + tr("median: The number of median iterations.\n")
                               + tr("vb: Print out verbose benchmarking information.");
    QMessageBox::information(this, tr("Output Options"), msg);
}

QT_END_NAMESPACE

