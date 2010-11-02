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

const static QString BENCHMARK_OPT_WIN_TITLE = "Benchmark Settings";
const static QString BTN_SAVE = "Save";
const static QString BTN_CANCEL = "Cancel";
const static QString BTN_HELP = "Help";

BenchmarkOptDlg::BenchmarkOptDlg(GlobalConfig *config, QWidget *parent):
        QDialog(parent),
        initCfg(config),
        savedCfg(NULL)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(BENCHMARK_OPT_WIN_TITLE);
    load();
    setLayout();
    if (initCfg != NULL) {
        savedCfg = new GlobalConfig(*initCfg);
        savedCfg->setParent(this);
    }
    else {
        savedCfg = new GlobalConfig(this);
        initCfg = new GlobalConfig(*savedCfg);
        initCfg->setParent(this);
    }
    this->setAttribute(Qt::WA_DeleteOnClose);
}

BenchmarkOptDlg::~BenchmarkOptDlg()
{
}

void BenchmarkOptDlg::createWidgets()
{
    chkbVb = new QCheckBox(VERBOSEBENCHMARK, this);
    chkbVb->setChecked(initCfg != NULL && initCfg->verboseBenchMarking());

    chkbTickCounter = new QCheckBox(TICKCOUNTER, this);
    chkbTickCounter->setChecked(initCfg != NULL && initCfg->tickCounter());

    chkbEventCounter = new QCheckBox(EVENTCOUNTER, this);
    chkbEventCounter->setChecked(initCfg != NULL && initCfg->eventCounter());

    chkbMinVal = new QCheckBox(MINIMUMVALUE + ": ", this);
    chkbMinVal->setChecked(initCfg != NULL && initCfg->minimumValue() != -1);
    leMinVal = new QLineEdit(this);
    leMinVal->setEnabled(chkbMinVal->isChecked());
    if (leMinVal->isEnabled()) {
        leMinVal->setText(tr("%1").arg(initCfg->minimumValue()));
    }

    chkbIters = new QCheckBox(ITERATIONS, this);
    chkbIters->setChecked(initCfg != NULL && initCfg->minimumValue() != -1);
    leIters = new QLineEdit(this);
    leIters->setEnabled(chkbIters->isChecked());
    if (leIters->isEnabled()) {
        leIters->setText(tr("%1").arg(initCfg->iterations()));
    }

    chkbMedian = new QCheckBox(MEDIAN, this);
    chkbMedian->setChecked(initCfg != NULL && initCfg->median() != -1);
    leMedian = new QLineEdit(this);
    leMedian->setEnabled(chkbMedian->isChecked());
    if (leMedian->isEnabled()) {
        leMedian->setText(tr("%1").arg(initCfg->median()));
    }

#if defined (Q_OS_LINUX)
    chkbCallGrind = new QCheckBox(CALLGRIND, this);
    chkbCallGrind->setChecked(initCfg != NULL && initCfg->callGrind());
#endif

    btnSave = new QPushButton(BTN_SAVE, this);
    btnSave->setEnabled(false);
    btnCancel = new QPushButton(BTN_CANCEL, this);
    btnHelp = new QPushButton(BTN_HELP, this);
}

void BenchmarkOptDlg::setupWidgetsEventHandlers()
{
    connect(chkbVb, SIGNAL(stateChanged(int)), this, SLOT(changeVbBenchmarkState(int)));
    connect(chkbTickCounter, SIGNAL(stateChanged(int)), this, SLOT(changeTickCounterState(int)));
    connect(chkbEventCounter, SIGNAL(stateChanged(int)), this, SLOT(changeEventCounterState(int)));
    connect(chkbMinVal, SIGNAL(stateChanged(int)), this, SLOT(changeMinValState(int)));
    connect(chkbIters, SIGNAL(stateChanged(int)), this, SLOT(changeIterState(int)));
    connect(chkbMedian, SIGNAL(stateChanged(int)), this, SLOT(changeMedianState(int)));
#if defined (Q_OS_LINUX)
    connect(chkbCallGrind, SIGNAL(stateChanged(int)), this, SLOT(changeCallGrindState(int)));
#endif
    connect(leMinVal, SIGNAL(textChanged(QString)), this, SLOT(changeMinVal(QString)));
    connect(leIters, SIGNAL(textChanged(QString)), this, SLOT(changeIter(QString)));
    connect(leMedian, SIGNAL(textChanged(QString)), this, SLOT(changeMedian(QString)));
    connect(btnSave, SIGNAL(clicked()), this, SLOT(saveOpt()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(closeDlg()));
    connect(btnHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void BenchmarkOptDlg::load()
{
    createWidgets();
    setupWidgetsEventHandlers();
}

void BenchmarkOptDlg::setLayout()
{
    QWidget* hbWidget1 = new QWidget(this);
    QHBoxLayout* layout1 = new QHBoxLayout(hbWidget1);
    layout1->addWidget(chkbTickCounter);
    layout1->addWidget(chkbEventCounter);
    hbWidget1->setLayout(layout1);

    QWidget* hbWidget2 = new QWidget(this);
    QHBoxLayout* layout2 = new QHBoxLayout(hbWidget2);
    layout2->addWidget(chkbVb);
    layout2->addWidget(chkbIters);
    layout2->addWidget(leIters);
    hbWidget2->setLayout(layout2);

    QWidget* hbWidget3 = new QWidget(this);
    QHBoxLayout* layout3 = new QHBoxLayout(hbWidget3);
    layout3->addWidget(chkbMedian);
    layout3->addWidget(leMedian);
#if defined (Q_OS_LINUX)
    layout3->addWidget(chkbCallGrind);
#endif
    hbWidget3->setLayout(layout3);

    QWidget* hbWidget4 = new QWidget(this);
    QHBoxLayout* layout4 = new QHBoxLayout(hbWidget4);
    layout4->addWidget(chkbMinVal);
    layout4->addWidget(leMinVal);
    hbWidget4->setLayout(layout4);

    QWidget* btnLayoutWidget = new QWidget(this);
    QHBoxLayout* btnLayout = new QHBoxLayout(btnLayoutWidget);
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnHelp);
    btnLayoutWidget->setLayout(btnLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(hbWidget1);
    mainLayout->addWidget(hbWidget2);
    mainLayout->addWidget(hbWidget3);
    mainLayout->addWidget(hbWidget4);
    mainLayout->addWidget(btnLayoutWidget);
}

void BenchmarkOptDlg::changeBtnSaveEnableState()
{
    btnSave->setEnabled(!initCfg->equalsTo(savedCfg));
}

void BenchmarkOptDlg::changeVbBenchmarkState(int state)
{
    savedCfg->setVerboseBenchMarking(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeTickCounterState(int state)
{
    savedCfg->setTickCounter(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeEventCounterState(int state)
{
    savedCfg->setEventCounter(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMinValState(int state)
{
    leMinVal->setEnabled(state == Qt::Checked);
    if (!leMinVal->isEnabled()) {
        leMinVal->clear();
        savedCfg->setMinimumValue(-1);
    } else {
        if (initCfg->minimumValue() != -1) {
            leMinVal->setText(tr("%1").arg(initCfg->minimumValue()));
            savedCfg->setMinimumValue(initCfg->minimumValue());
        }
    }
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeIterState(int state)
{
    leIters->setEnabled(state == Qt::Checked);
    if (!leIters->isEnabled()) {
        leIters->clear();
        savedCfg->setIterations(-1);
    } else {
        if (initCfg->iterations() != -1) {
            leIters->setText(tr("%1").arg(initCfg->iterations()));
            savedCfg->setIterations(initCfg->iterations());
        }
    }
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMedianState(int state)
{
    leMedian->setEnabled(state == Qt::Checked);
    if (!leMedian->isEnabled()) {
        leMedian->clear();
        savedCfg->setMedian(-1);
    } else {
        if (initCfg->median() != -1) {
            leMedian->setText(tr("%1").arg(initCfg->median()));
            savedCfg->setMedian(initCfg->median());
        }
    }
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMinVal(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedCfg->setMinimumValue(val);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeIter(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedCfg->setIterations(val);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeMedian(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedCfg->setMedian(val);
    changeBtnSaveEnableState();
}

void BenchmarkOptDlg::changeCallGrindState(int state)
{
#if defined (Q_OS_LINUX)
    savedCfg->setCallGrind(state == Qt::Checked);
    changeBtnSaveEnableState();
#endif
}

void BenchmarkOptDlg::saveOpt()
{
    *initCfg = *savedCfg;
    changeBtnSaveEnableState();
    emit bmOptSaved(*savedCfg);
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
