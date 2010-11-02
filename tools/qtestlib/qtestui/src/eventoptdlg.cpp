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

const static QString EVENT_OPT_WIN_TITLE = "Event Settings";
const static QString BTN_SAVE = "Save";
const static QString BTN_CANCEL = "Cancel";
const static QString BTN_HELP = "Help";

EventOptDlg::EventOptDlg(GlobalConfig *config, QWidget *parent):
        QDialog(parent),
        initCfg(config),
        savedCfg(NULL)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(EVENT_OPT_WIN_TITLE);
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

EventOptDlg::~EventOptDlg()
{
}

void EventOptDlg::createWidgets()
{
    chkbEventDelay = new QCheckBox(EVENTDELAY + ": ", this);
    chkbEventDelay->setChecked(initCfg != NULL && initCfg->eventDelay() != -1);
    leEventDelay = new QLineEdit(this);
    leEventDelay->setEnabled(chkbEventDelay->isChecked());
    if (leEventDelay->isEnabled())
        leEventDelay->setText(tr("%1").arg(initCfg->eventDelay()));

    chkbKeyDelay = new QCheckBox(KEYDELAY + ": ", this);
    chkbKeyDelay->setChecked(initCfg != NULL && initCfg->keyDelay() != -1);
    leKeyDelay = new QLineEdit(this);
    leKeyDelay->setEnabled(chkbKeyDelay->isChecked());
    if (leKeyDelay->isEnabled())
        leKeyDelay->setText(tr("%1").arg(initCfg->keyDelay()));

    chkbMouseDelay = new QCheckBox(MOUSEDELAY + ": ", this);
    chkbMouseDelay->setChecked(initCfg != NULL && initCfg->mouseDelay() != -1);
    leMouseDelay = new QLineEdit(this);
    leMouseDelay->setEnabled(chkbMouseDelay->isChecked());
    if (leMouseDelay->isEnabled())
        leMouseDelay->setText(tr("%1").arg(initCfg->mouseDelay()));

    chkbKeyEventVerbose = new QCheckBox(KEYEVENTVERBOSE, this);
    chkbKeyEventVerbose->setChecked(initCfg != NULL && initCfg->keyEventVerbose());

    chkbCrashHandler = new QCheckBox(NOCRASHHANDLER, this);
    chkbCrashHandler->setChecked(initCfg != NULL && initCfg->noCrashHandler());

    btnSave = new QPushButton(BTN_SAVE, this);
    btnSave->setEnabled(false);
    btnCancel = new QPushButton(BTN_CANCEL, this);
    btnHelp = new QPushButton(BTN_HELP, this);
}

void EventOptDlg::setupWidgetsEventHandlers()
{
    connect(chkbEventDelay, SIGNAL(stateChanged(int)), this, SLOT(changeEventDelayState(int)));
    connect(chkbKeyDelay, SIGNAL(stateChanged(int)), this, SLOT(changeKeyDelayState(int)));
    connect(chkbMouseDelay, SIGNAL(stateChanged(int)), this, SLOT(changeMouseDelayState(int)));
    connect(chkbKeyEventVerbose, SIGNAL(stateChanged(int)), this, SLOT(changeKeyEventVB(int)));
    connect(chkbCrashHandler, SIGNAL(stateChanged(int)), this, SLOT(changeNoCrashHandler(int)));
    connect(leEventDelay, SIGNAL(textChanged(QString)), this, SLOT(changeEventDelay(QString)));
    connect(leKeyDelay, SIGNAL(textChanged(QString)), this, SLOT(changeKeyDelay(QString)));
    connect(leMouseDelay, SIGNAL(textChanged(QString)), this, SLOT(changeMouseDelay(QString)));
    connect(btnSave, SIGNAL(clicked()), this, SLOT(saveOpt()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(closeDlg()));
    connect(btnHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
}

void EventOptDlg::load()
{
    createWidgets();
    setupWidgetsEventHandlers();
}

void EventOptDlg::setLayout()
{
    QWidget* hbWidget1 = new QWidget(this);
    QHBoxLayout* layout1 = new QHBoxLayout(hbWidget1);
    layout1->addWidget(chkbEventDelay);
    layout1->addWidget(leEventDelay);
    hbWidget1->setLayout(layout1);

    QWidget* hbWidget2 = new QWidget(this);
    QHBoxLayout* layout2 = new QHBoxLayout(hbWidget2);
    layout2->addWidget(chkbKeyDelay);
    layout2->addWidget(leKeyDelay);
    hbWidget2->setLayout(layout2);

    QWidget* hbWidget3 = new QWidget(this);
    QHBoxLayout* layout3 = new QHBoxLayout(hbWidget3);
    layout3->addWidget(chkbMouseDelay);
    layout3->addWidget(leMouseDelay);
    hbWidget3->setLayout(layout3);

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
    mainLayout->addWidget(chkbKeyEventVerbose);
    mainLayout->addWidget(chkbCrashHandler);
    mainLayout->addWidget(btnLayoutWidget);
}

void EventOptDlg::changeBtnSaveEnableState()
{
    btnSave->setEnabled(!initCfg->equalsTo(savedCfg));
}

void EventOptDlg::changeEventDelayState(int state)
{
    leEventDelay->setEnabled(state == Qt::Checked);
    if (!leEventDelay->isEnabled()) {
        leEventDelay->clear();
        savedCfg->setEventDelay(-1);
    } else {
        if (initCfg->eventDelay() != -1) {
            leEventDelay->setText(tr("%1").arg(initCfg->eventDelay()));
            savedCfg->setEventDelay(initCfg->eventDelay());
        }
    }
    changeBtnSaveEnableState();
}

void EventOptDlg::changeKeyDelayState(int state)
{
    leKeyDelay->setEnabled(state == Qt::Checked);
    if (!leKeyDelay->isEnabled()) {
        leKeyDelay->clear();
        savedCfg->setKeyDelay(-1);
    } else {
        if (initCfg->keyDelay() != -1) {
            leKeyDelay->setText(tr("%1").arg(initCfg->keyDelay()));
            savedCfg->setKeyDelay(initCfg->keyDelay());
        }
    }
    changeBtnSaveEnableState();
}

void EventOptDlg::changeMouseDelayState(int state)
{
    leMouseDelay->setEnabled(state == Qt::Checked);
    if (!leMouseDelay->isEnabled()) {
        leMouseDelay->clear();
        savedCfg->setMouseDelay(-1);
    } else {
        if (initCfg->mouseDelay() != -1) {
            leMouseDelay->setText(tr("%1").arg(initCfg->mouseDelay()));
            savedCfg->setMouseDelay(initCfg->mouseDelay());
        }
    }
    changeBtnSaveEnableState();
}

void EventOptDlg::changeKeyEventVB(int state)
{
    savedCfg->setKeyEventVerbose(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeNoCrashHandler(int state)
{
    savedCfg->setNoCrashHandler(state == Qt::Checked);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeEventDelay(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedCfg->setEventDelay(val);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeKeyDelay(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedCfg->setKeyDelay(val);
    changeBtnSaveEnableState();
}

void EventOptDlg::changeMouseDelay(QString text)
{
    bool ok = false;
    int val = text.toInt(&ok);
    if (!ok) val = -1;
    savedCfg->setMouseDelay(val);
    changeBtnSaveEnableState();
}

void EventOptDlg::saveOpt()
{
    *initCfg = *savedCfg;
    changeBtnSaveEnableState();
    emit eventOptSaved(*savedCfg);
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
