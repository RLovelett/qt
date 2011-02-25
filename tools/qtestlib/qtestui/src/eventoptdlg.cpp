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
#include "inc/eventoptdlg.h"
#include "inc/testconfig.h"

QT_BEGIN_NAMESPACE

EventOptDlg::EventOptDlg(GlobalConfig *config, QWidget *parent):
        QDialog(parent),
        initConfig(config),
        savedConfig(NULL)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(tr("Event Settings"));
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

EventOptDlg::~EventOptDlg()
{
}

void EventOptDlg::createWidgets()
{
    checkboxEventDelay = new QCheckBox(GlobalConfig::EVENTDELAY + ": ", this);
    checkboxEventDelay->setChecked(initConfig != NULL && initConfig->eventDelay() != -1);
    lineEditEventDelay = new QLineEdit(this);
    lineEditEventDelay->setEnabled(checkboxEventDelay->isChecked());
    if (lineEditEventDelay->isEnabled())
        lineEditEventDelay->setText(tr("%1").arg(initConfig->eventDelay()));

    checkboxKeyDelay = new QCheckBox(GlobalConfig::KEYDELAY + ": ", this);
    checkboxKeyDelay->setChecked(initConfig != NULL && initConfig->keyDelay() != -1);
    lineEditKeyDelay = new QLineEdit(this);
    lineEditKeyDelay->setEnabled(checkboxKeyDelay->isChecked());
    if (lineEditKeyDelay->isEnabled())
        lineEditKeyDelay->setText(tr("%1").arg(initConfig->keyDelay()));

    checkboxMouseDelay = new QCheckBox(GlobalConfig::MOUSEDELAY + ": ", this);
    checkboxMouseDelay->setChecked(initConfig != NULL && initConfig->mouseDelay() != -1);
    lineEditMouseDelay = new QLineEdit(this);
    lineEditMouseDelay->setEnabled(checkboxMouseDelay->isChecked());
    if (lineEditMouseDelay->isEnabled())
        lineEditMouseDelay->setText(tr("%1").arg(initConfig->mouseDelay()));

    checkboxKeyEventVerbose = new QCheckBox(GlobalConfig::KEYEVENTVERBOSE, this);
    checkboxKeyEventVerbose->setChecked(initConfig != NULL && initConfig->keyEventVerbose());

    checkboxCrashHandler = new QCheckBox(GlobalConfig::NOCRASHHANDLER, this);
    checkboxCrashHandler->setChecked(initConfig != NULL && initConfig->noCrashHandler());

    buttonSave = new QPushButton(tr("Save"), this);
    buttonSave->setEnabled(false);
    buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonHelp = new QPushButton(tr("Help"), this);
}

void EventOptDlg::setupWidgetsEventHandlers()
{
    connect(checkboxEventDelay, SIGNAL(stateChanged(int)), this, SLOT(changeEventDelayState(int)));
    connect(checkboxKeyDelay, SIGNAL(stateChanged(int)), this, SLOT(changeKeyDelayState(int)));
    connect(checkboxMouseDelay, SIGNAL(stateChanged(int)), this, SLOT(changeMouseDelayState(int)));
    connect(checkboxKeyEventVerbose, SIGNAL(stateChanged(int)), this, SLOT(changeKeyEventVB(int)));
    connect(checkboxCrashHandler, SIGNAL(stateChanged(int)), this, SLOT(changeNoCrashHandler(int)));
    connect(lineEditEventDelay, SIGNAL(textChanged(QString)), this, SLOT(changeEventDelay(QString)));
    connect(lineEditKeyDelay, SIGNAL(textChanged(QString)), this, SLOT(changeKeyDelay(QString)));
    connect(lineEditMouseDelay, SIGNAL(textChanged(QString)), this, SLOT(changeMouseDelay(QString)));
    connect(buttonSave, SIGNAL(clicked()), this, SLOT(saveOpt()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(closeDlg()));
    connect(buttonHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void EventOptDlg::load()
{
    createWidgets();
    setupWidgetsEventHandlers();
}

void EventOptDlg::setLayout()
{
    QWidget *hbWidget1 = new QWidget(this);
    QHBoxLayout *layout1 = new QHBoxLayout(hbWidget1);
    layout1->addWidget(checkboxEventDelay);
    layout1->addWidget(lineEditEventDelay);
    hbWidget1->setLayout(layout1);

    QWidget *hbWidget2 = new QWidget(this);
    QHBoxLayout *layout2 = new QHBoxLayout(hbWidget2);
    layout2->addWidget(checkboxKeyDelay);
    layout2->addWidget(lineEditKeyDelay);
    hbWidget2->setLayout(layout2);

    QWidget *hbWidget3 = new QWidget(this);
    QHBoxLayout *layout3 = new QHBoxLayout(hbWidget3);
    layout3->addWidget(checkboxMouseDelay);
    layout3->addWidget(lineEditMouseDelay);
    hbWidget3->setLayout(layout3);

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
    mainLayout->addWidget(checkboxKeyEventVerbose);
    mainLayout->addWidget(checkboxCrashHandler);
    mainLayout->addWidget(btnLayoutWidget);
}

void EventOptDlg::changeBtnSaveEnableState()
{
    buttonSave->setEnabled(!initConfig->equalsTo(savedConfig));
}

void EventOptDlg::changeEventDelayState(int state)
{
    lineEditEventDelay->setEnabled(state == Qt::Checked);
    if (!lineEditEventDelay->isEnabled()) {
        lineEditEventDelay->clear();
        savedConfig->setEventDelay(-1);
    } else {
        if (initConfig->eventDelay() != -1) {
            lineEditEventDelay->setText(tr("%1").arg(initConfig->eventDelay()));
            savedConfig->setEventDelay(initConfig->eventDelay());
        }
    }
    changeBtnSaveEnableState();
}

void EventOptDlg::changeKeyDelayState(int state)
{
    lineEditKeyDelay->setEnabled(state == Qt::Checked);
    if (!lineEditKeyDelay->isEnabled()) {
        lineEditKeyDelay->clear();
        savedConfig->setKeyDelay(-1);
    } else {
        if (initConfig->keyDelay() != -1) {
            lineEditKeyDelay->setText(tr("%1").arg(initConfig->keyDelay()));
            savedConfig->setKeyDelay(initConfig->keyDelay());
        }
    }
    changeBtnSaveEnableState();
}

void EventOptDlg::changeMouseDelayState(int state)
{
    lineEditMouseDelay->setEnabled(state == Qt::Checked);
    if (!lineEditMouseDelay->isEnabled()) {
        lineEditMouseDelay->clear();
        savedConfig->setMouseDelay(-1);
    } else {
        if (initConfig->mouseDelay() != -1) {
            lineEditMouseDelay->setText(tr("%1").arg(initConfig->mouseDelay()));
            savedConfig->setMouseDelay(initConfig->mouseDelay());
        }
    }
    changeBtnSaveEnableState();
}

void EventOptDlg::changeKeyEventVB(int state)
{
    savedConfig->setKeyEventVerbose(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeNoCrashHandler(int state)
{
    savedConfig->setNoCrashHandler(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeEventDelay(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedConfig->setEventDelay(val);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeKeyDelay(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedConfig->setKeyDelay(val);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeMouseDelay(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedConfig->setMouseDelay(val);
    changeBtnSaveEnableState();
}

void EventOptDlg::saveOpt()
{
    *initConfig = *savedConfig;
    changeBtnSaveEnableState();
    emit eventOptSaved(*savedConfig);
    closeDlg();
}

void EventOptDlg::closeDlg()
{
    this->close();
}

void EventOptDlg::showHelp()
{
    const static QString msg = tr("eventdelay: Default delay(milliseconds) for mouse and keyboard simulation.\n")
                               + tr("keydelay: Default delay(milliseconds) for keyboard simulation.\n")
                               + tr("mousedelay: Default delay(milliseconds) for mouse simulation.\n")
                               + tr("keyevent-verbose: Turn on verbose msg for keyboard simulation.\n")
                               + tr("nocrashhandler: Disables the crash handler.");
    QMessageBox::information(this, tr("Event Options"), msg);
}

QT_END_NAMESPACE

