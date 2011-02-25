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

QT_BEGIN_NAMESPACE

OutputOptDlg::OutputOptDlg(GlobalConfig *config, QWidget *parent):
        QDialog(parent),
        initConfig(config),
        savedConfig(NULL)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(tr("Output Settings"));
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
    checkboxUseOutputFile = new QCheckBox(GlobalConfig::OUTPUTPATH + ": ", this);
    checkboxUseOutputFile->setChecked(initConfig != NULL && !(initConfig->outputPath().isEmpty()));

    labelOutputFormat = new QLabel(tr("output format:"), this);

    comboBoxOutputFormat = new QComboBox(this);
    listFormats << "" << "xml" << "lightxml" << "xunitxml";
    comboBoxOutputFormat->addItems(listFormats);
    if (initConfig != NULL && !initConfig->outputFormat().isEmpty())
        comboBoxOutputFormat->setCurrentIndex(listFormats.indexOf(initConfig->outputFormat()));

    lineEditOutputName = new QLineEdit(this);
    lineEditOutputName->setEnabled(initConfig != NULL && !(initConfig->outputPath().isEmpty()));
    if (lineEditOutputName->isEnabled()) {
        QString path = initConfig->outputPath();
        // Get rid of extra "/" at the end
        if (path != "" && path.endsWith('/')) path.remove(path.length() - 1, 1);
        lineEditOutputName->setText(path);
    }

    checkboxSilent = new QCheckBox(GlobalConfig::SILENT, this);
    checkboxSilent->setChecked(initConfig != NULL && initConfig->silent());

    checkboxVerbose = new QCheckBox(GlobalConfig::VERBOSE, this);
    checkboxVerbose->setChecked(initConfig != NULL && !(initConfig->verbose().isEmpty()));

    listVbLevel << "" << "v1" << "v2" << "vs";
    comboBoxVerbose = new QComboBox(this);
    comboBoxVerbose->addItems(listVbLevel);
    comboBoxVerbose->setEnabled(checkboxVerbose->isChecked());
    if (comboBoxVerbose->isEnabled())
        comboBoxVerbose->setCurrentIndex(listVbLevel.indexOf(initConfig->verbose()));

    checkboxMaxWarnings = new QCheckBox(GlobalConfig::MAXWARNINGS + ": ", this);
    checkboxMaxWarnings->setChecked(initConfig != NULL && initConfig->maxWarnings() != -1);
    lineEditMaxWarnings = new QLineEdit(this);
    lineEditMaxWarnings->setEnabled(checkboxMaxWarnings->isChecked());
    if (lineEditMaxWarnings->isEnabled())
        lineEditMaxWarnings->setText(tr("%1").arg(initConfig->maxWarnings()));

    checkboxFlush = new QCheckBox(GlobalConfig::FLUSH, this);
    checkboxFlush->setChecked(initConfig != NULL && initConfig->flush());

    buttonSave = new QPushButton(tr("Save"), this);
    buttonSave->setEnabled(false);
    buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonHelp = new QPushButton(tr("Help"), this);
}

void OutputOptDlg::setLayout()
{
    QWidget *outputLayoutWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(outputLayoutWidget);
    layout->addWidget(checkboxUseOutputFile);
    layout->addWidget(lineEditOutputName);
    outputLayoutWidget->setLayout(layout);

    QWidget *fmtLayoutWidget = new QWidget(this);
    QHBoxLayout *fmtLayout = new QHBoxLayout(fmtLayoutWidget);
    fmtLayout->addWidget(labelOutputFormat);
    fmtLayout->addWidget(comboBoxOutputFormat);
    fmtLayoutWidget->setLayout(fmtLayout);

    QWidget *vbLayoutWidget = new QWidget(this);
    QHBoxLayout *layoutVb = new QHBoxLayout(vbLayoutWidget);
    layoutVb->addWidget(checkboxSilent);
    layoutVb->addWidget(checkboxVerbose);
    layoutVb->addWidget(comboBoxVerbose);
    vbLayoutWidget->setLayout(layoutVb);

    QWidget *mLayoutWidget = new QWidget(this);
    QHBoxLayout *mLayout = new QHBoxLayout(mLayoutWidget);
    mLayout->addWidget(checkboxFlush);
    mLayout->addWidget(checkboxMaxWarnings);
    mLayout->addWidget(lineEditMaxWarnings);
    mLayoutWidget->setLayout(mLayout);

    QWidget *btnLayoutWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnLayoutWidget);
    btnLayout->addWidget(buttonSave);
    btnLayout->addWidget(buttonCancel);
    btnLayout->addWidget(buttonHelp);
    btnLayoutWidget->setLayout(btnLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(outputLayoutWidget);
    mainLayout->addWidget(fmtLayoutWidget);
    mainLayout->addWidget(vbLayoutWidget);
    mainLayout->addWidget(mLayoutWidget);
    mainLayout->addWidget(btnLayoutWidget);
}

void OutputOptDlg::setupWidgetsEventHandlers()
{
    connect(checkboxUseOutputFile, SIGNAL(stateChanged(int)), this, SLOT(changeOutputFile(int)));
    connect(checkboxSilent, SIGNAL(stateChanged(int)), this, SLOT(changeSilent(int)));
    connect(lineEditOutputName, SIGNAL(textChanged(QString)), this, SLOT(changeOutputFilePath(QString)));
    connect(comboBoxOutputFormat, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(changeOutputFormat(QString)));
    connect(checkboxVerbose, SIGNAL(stateChanged(int)), this, SLOT(changeVerboseState(int)));
    connect(comboBoxVerbose, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeVerboseLevel(QString)));
    connect(checkboxFlush, SIGNAL(stateChanged(int)), this, SLOT(changeFlush(int)));
    connect(checkboxMaxWarnings, SIGNAL(stateChanged(int)), this, SLOT(changeMaxWarningsState(int)));
    connect(lineEditMaxWarnings, SIGNAL(textChanged(QString)), this, SLOT(changeMaxWarnings(QString)));
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(saveOutputOpt()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(closeDlg()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void OutputOptDlg::changeBtnSaveEnableState()
{
    buttonSave->setEnabled(!initConfig->equalsTo(savedConfig));
}

void OutputOptDlg::changeOutputFile(int state)
{
    lineEditOutputName->setEnabled(state == Qt::Checked);
    if (!lineEditOutputName->isEnabled()) {
        lineEditOutputName->clear();
        savedConfig->setOutputPath("");
    } else {
        QString path = initConfig->outputPath();
        // Get rid of extra "/" at the end
        if (path != "" && path.endsWith('/')) path.remove(path.length() - 1, 1);
        lineEditOutputName->setText(path);
        savedConfig->setOutputPath(path);
    }
    comboBoxOutputFormat->setCurrentIndex(initConfig != NULL && !initConfig->outputFormat().isEmpty()
                                          ? listFormats.indexOf(initConfig->outputFormat()) : 0);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeSilent(int state)
{
    savedConfig->setSilent(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeOutputFilePath(QString text)
{
    savedConfig->setOutputPath(text);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeOutputFormat(QString text)
{
    savedConfig->setOutputFormat(text);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeVerboseState(int state)
{
    comboBoxVerbose->setEnabled(state == Qt::Checked);
    comboBoxVerbose->setCurrentIndex(comboBoxVerbose->isEnabled()
                                     ? listVbLevel.indexOf(initConfig->verbose()) : 0);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeVerboseLevel(QString text)
{
    savedConfig->setVerbose(text);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeFlush(int state)
{
    savedConfig->setFlush(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void OutputOptDlg::changeMaxWarningsState(int state)
{
    lineEditMaxWarnings->setEnabled(state == Qt::Checked);
    if (!lineEditMaxWarnings->isEnabled()) {
        lineEditMaxWarnings->clear();
        savedConfig->setMaxWarnings(-1);
    } else {
        if (initConfig->maxWarnings() != -1) {
            lineEditMaxWarnings->setText(tr("%1").arg(initConfig->maxWarnings()));
            savedConfig->setMaxWarnings(initConfig->maxWarnings());
        }
    }
}

void OutputOptDlg::changeMaxWarnings(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedConfig->setMaxWarnings(val);
    changeBtnSaveEnableState();
}

void OutputOptDlg::saveOutputOpt()
{
    QString savedPath = savedConfig->outputPath();
    QDir path(savedPath);
    if (!path.exists()) {
        if (!path.mkpath(savedPath)) {
            QMessageBox::information(this, tr("ERROR"), tr("Failed to create dir: ") + savedPath);
            return;
        }
    }
    *initConfig = *savedConfig;
    changeBtnSaveEnableState();
    emit outputOptSaved(*savedConfig);
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
                               + tr(" v1: Print enter msg for each test function.\n")
                               + tr(" v2: each QVERIFY/QCOMPARE/QTEST and v1.\n")
                               + tr(" vs: Print every signal emitted.\n")
                               + tr("flush: Flushes the results.\n")
                               + tr("maxwarnings: Max amount of msg to output.(0: unlimited, default: 2000)");
    QMessageBox::information(this, tr("Output Options"), msg);
}

QT_END_NAMESPACE

