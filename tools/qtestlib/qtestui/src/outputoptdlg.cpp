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
#include "inc/outputoptdlg.h"
#include "inc/testconfig.h"

const static QString OUTPUT_OPT_WIN_TITLE = "Output Settings";
const static QString FMT_XUNITXML = "xunitxml";
const static QString FMT_XML = "xml";
const static QString FMT_LIGHTXML = "lightxml";
const static QString VERBOSE_LEVEL1 = "v1";
const static QString VERBOSE_LEVEL2 = "v2";
const static QString VERBOSE_LEVEL_S = "vs";
const static QString OUTPUT_FMT = "output format:";
const static QString BTN_SAVE = "Save";
const static QString BTN_CANCEL = "Cancel";
const static QString BTN_HELP = "Help";

OutputOptDlg::OutputOptDlg(GlobalConfig *config, QWidget *parent):
        QDialog(parent),
        initCfg(config),
        savedCfg(NULL)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(OUTPUT_OPT_WIN_TITLE);
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

OutputOptDlg::~OutputOptDlg()
{
}

void OutputOptDlg::load()
{
    createWidgets();
    setupWidgetsEventHandlers();
}

void OutputOptDlg::createWidgets()
{
    chkbUseOutputFile = new QCheckBox(OUTPUTPATH + ": ", this);
    chkbUseOutputFile->setChecked(initCfg != NULL && !(initCfg->outputPath().isEmpty()));

    lblOutputFmt = new QLabel(OUTPUT_FMT, this);

    cbOutputFormat = new QComboBox(this);
    listFormats << "" << FMT_XML << FMT_LIGHTXML << FMT_XUNITXML;
    cbOutputFormat->addItems(listFormats);
    if (initCfg != NULL && !initCfg->outputFormat().isEmpty())
        cbOutputFormat->setCurrentIndex(listFormats.indexOf(initCfg->outputFormat()));

    leOutputName = new QLineEdit(this);
    leOutputName->setEnabled(initCfg != NULL && !(initCfg->outputPath().isEmpty()));
    if (leOutputName->isEnabled()) {
        QString path = initCfg->outputPath();
        // Get rid of extra "/" at the end
        if (path != "" && path.endsWith('/')) path.remove(path.length() - 1, 1);
        leOutputName->setText(path);
    }

    chkbSilent = new QCheckBox(SILENT, this);
    chkbSilent->setChecked(initCfg != NULL && initCfg->silent());

    chkbVerbose = new QCheckBox(VERBOSE, this);
    chkbVerbose->setChecked(initCfg != NULL && !(initCfg->verbose().isEmpty()));

    listVbLevel << "" << VERBOSE_LEVEL1 << VERBOSE_LEVEL2 << VERBOSE_LEVEL_S;
    cbVerbose = new QComboBox(this);
    cbVerbose->addItems(listVbLevel);
    cbVerbose->setEnabled(chkbVerbose->isChecked());
    if (cbVerbose->isEnabled())
        cbVerbose->setCurrentIndex(listVbLevel.indexOf(initCfg->verbose()));

    chkbMaxWarnings = new QCheckBox(MAXWARNINGS + ": ", this);
    chkbMaxWarnings->setChecked(initCfg != NULL && initCfg->maxWarnings() != -1);
    leMaxWarnings = new QLineEdit(this);
    leMaxWarnings->setEnabled(chkbMaxWarnings->isChecked());
    if (leMaxWarnings->isEnabled())
        leMaxWarnings->setText(tr("%1").arg(initCfg->maxWarnings()));

    chkbFlush = new QCheckBox(FLUSH, this);
    chkbFlush->setChecked(initCfg != NULL && initCfg->flush());

    btnSave = new QPushButton(BTN_SAVE, this);
    btnSave->setEnabled(false);
    btnCancel = new QPushButton(BTN_CANCEL, this);
    btnHelp = new QPushButton(BTN_HELP, this);
}

void OutputOptDlg::setLayout()
{
    QWidget* outputLayoutWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(outputLayoutWidget);
    layout->addWidget(chkbUseOutputFile);
    layout->addWidget(leOutputName);
    outputLayoutWidget->setLayout(layout);

    QWidget* fmtLayoutWidget = new QWidget(this);
    QHBoxLayout* fmtLayout = new QHBoxLayout(fmtLayoutWidget);
    fmtLayout->addWidget(lblOutputFmt);
    fmtLayout->addWidget(cbOutputFormat);
    fmtLayoutWidget->setLayout(fmtLayout);

    QWidget* vbLayoutWidget = new QWidget(this);
    QHBoxLayout* layoutVb = new QHBoxLayout(vbLayoutWidget);
    layoutVb->addWidget(chkbSilent);
    layoutVb->addWidget(chkbVerbose);
    layoutVb->addWidget(cbVerbose);
    vbLayoutWidget->setLayout(layoutVb);

    QWidget* mLayoutWidget = new QWidget(this);
    QHBoxLayout* mLayout = new QHBoxLayout(mLayoutWidget);
    mLayout->addWidget(chkbFlush);
    mLayout->addWidget(chkbMaxWarnings);
    mLayout->addWidget(leMaxWarnings);
    mLayoutWidget->setLayout(mLayout);

    QWidget* btnLayoutWidget = new QWidget(this);
    QHBoxLayout* btnLayout = new QHBoxLayout(btnLayoutWidget);
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnHelp);
    btnLayoutWidget->setLayout(btnLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(outputLayoutWidget);
    mainLayout->addWidget(fmtLayoutWidget);
    mainLayout->addWidget(vbLayoutWidget);
    mainLayout->addWidget(mLayoutWidget);
    mainLayout->addWidget(btnLayoutWidget);
}

void OutputOptDlg::setupWidgetsEventHandlers()
{
    connect(chkbUseOutputFile, SIGNAL(stateChanged(int)), this, SLOT(changeOutputFile(int)));
    connect(chkbSilent, SIGNAL(stateChanged(int)), this, SLOT(changeSilent(int)));
    connect(leOutputName, SIGNAL(textChanged(QString)), this, SLOT(changeOutputFilePath(QString)));
    connect(cbOutputFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeOutputFormat(QString)));
    connect(chkbVerbose, SIGNAL(stateChanged(int)), this, SLOT(changeVerboseState(int)));
    connect(cbVerbose, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeVerboseLevel(QString)));
    connect(chkbFlush, SIGNAL(stateChanged(int)), this, SLOT(changeFlush(int)));
    connect(chkbMaxWarnings, SIGNAL(stateChanged(int)), this, SLOT(changeMaxWarningsState(int)));
    connect(leMaxWarnings, SIGNAL(textChanged(QString)), this, SLOT(changeMaxWarnings(QString)));
    connect(btnSave, SIGNAL(clicked()), this, SLOT(saveOutputOpt()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(closeDlg()));
    connect(btnHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void OutputOptDlg::changeBtnSaveEnableState()
{
    btnSave->setEnabled(!initCfg->equalsTo(savedCfg));
}

void OutputOptDlg::changeOutputFile(int state)
{
    leOutputName->setEnabled(state == Qt::Checked);
    if (!leOutputName->isEnabled()) {
        leOutputName->clear();
        savedCfg->setOutputPath(tr(""));
    } else {
        QString path = initCfg->outputPath();
        // Get rid of extra "/" at the end
        if (path != "" && path.endsWith('/')) path.remove(path.length() - 1, 1);
        leOutputName->setText(path);
        savedCfg->setOutputPath(path);
    }
    cbOutputFormat->setCurrentIndex(initCfg != NULL && !initCfg->outputFormat().isEmpty() ? listFormats.indexOf(initCfg->outputFormat()) : 0);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeSilent(int state)
{
    savedCfg->setSilent(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeOutputFilePath(QString text)
{
    savedCfg->setOutputPath(text);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeOutputFormat(QString text)
{
    savedCfg->setOutputFormat(text);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeVerboseState(int state)
{
    cbVerbose->setEnabled(state == Qt::Checked);
    cbVerbose->setCurrentIndex(cbVerbose->isEnabled() ? listVbLevel.indexOf(initCfg->verbose()) : 0);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeVerboseLevel(QString text)
{
    savedCfg->setVerbose(text);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeFlush(int state)
{
    savedCfg->setFlush(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeMaxWarningsState(int state)
{
    leMaxWarnings->setEnabled(state == Qt::Checked);
    if (!leMaxWarnings->isEnabled()) {
        leMaxWarnings->clear();
        savedCfg->setMaxWarnings(-1);
    } else {
        if (initCfg->maxWarnings() != -1) {
            leMaxWarnings->setText(tr("%1").arg(initCfg->maxWarnings()));
            savedCfg->setMaxWarnings(initCfg->maxWarnings());
        }
    }
}

void OutputOptDlg::changeMaxWarnings(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedCfg->setMaxWarnings(val);
    changeBtnSaveEnableState();
}

void OutputOptDlg::saveOutputOpt()
{
    QString savedPath = savedCfg->outputPath();
    QDir path(savedPath);
    if (!path.exists()) {
        if (!path.mkdir(savedPath)) {
            QMessageBox::information(this, tr("ERROR"), tr("Invalid file/directory name: ") + savedPath);
            return;
        }
    }
    *initCfg = *savedCfg;
    changeBtnSaveEnableState();
    emit outputOptSaved(*savedCfg);
    closeDlg();
}

void OutputOptDlg::closeDlg()
{
    this->close();
}

void OutputOptDlg::showHelp()
{
    const static QString msg = tr("silent: Only outputs warnings and failures.\n")
                               + tr("verbose:\n")
                               + tr(" v1: Print enter msg for each test func.\n")
                               + tr(" v2: each QVERIFY/QCOMPARE/QTEST and v1.\n")
                               + tr(" vs: Print every signal emitted.\n")
                               + tr("flush: Flushes the results.\n")
                               + tr("maxwarnings: Max amount of msg to output.(0: unlimited, default: 2000)");
    QMessageBox::information(this, tr("Output Options"), msg);
}
