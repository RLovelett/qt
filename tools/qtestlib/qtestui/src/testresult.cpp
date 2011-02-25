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

#include "inc/testresult.h"

QT_BEGIN_NAMESPACE

TestResult::TestResult(QObject *parent):
        QObject(parent),
        textWidget(NULL),
        progressBar(NULL),
        passWidget(NULL),
        failWidget(NULL),
        skipWidget(NULL),
        reportWidget(NULL),
        caseList(NULL)
{
}

TestResult::~TestResult()
{
}

void TestResult::setTextWidget(QTextEdit *widget)
{
    this->textWidget = widget;
}

void TestResult::setProgressWidget(QProgressBar *widget)
{
    this->progressBar = widget;
}

void TestResult::setProgressRange(int minimum, int maximum)
{
    if (progressBar)
        progressBar->setRange(minimum, maximum);
}

void TestResult::progressClear()
{
    if (progressBar)
        progressBar->setValue(0);
    if (passWidget)
        passWidget->clear();
    if (failWidget)
        failWidget->clear();
    if (skipWidget)
        skipWidget->clear();
}

void TestResult::setProgressTextWidget(QLabel *pass, QLabel *fail, QLabel *skip)
{
    passWidget = pass;
    failWidget = fail;
    skipWidget = skip;
}

void TestResult::setProgressText(int pass, int fail, int skip)
{
    passWidget->setNum(pass);
    failWidget->setNum(fail);
    skipWidget->setNum(skip);
}

void TestResult::progressSteps(int steps)
{
    if (progressBar)
        progressBar->setValue(progressBar->value()+steps);
}

void TestResult::setReportWidget(QTableWidget *widget)
{
    this->reportWidget = widget;
    widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    widget->setSelectionMode(QAbstractItemView::NoSelection);
    widget->setColumnCount(4);
    widget->setColumnWidth(0, 225);
    widget->setColumnWidth(1, 35);
    widget->setColumnWidth(2, 35);
    widget->setColumnWidth(3, 35);
    QStringList headers;
    headers << "case" << "pass" << "fail" << "skip";
    widget->setHorizontalHeaderLabels(headers);
    widget->setRowCount(0);
    if (widget->isSortingEnabled())
        widget->setSortingEnabled(false);
    connect(widget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(showCaseLog(int, int)));
}

void TestResult::setReportSize(int size)
{
    if (reportWidget)
        reportWidget->setRowCount(size);
}

void TestResult::setReportItem(int index,
                               QTableWidgetItem *name,
                               QTableWidgetItem *pass,
                               QTableWidgetItem *fail,
                               QTableWidgetItem *skip)
{
    if (reportWidget)
    {
        reportWidget->setItem(index, 0, name);
        reportWidget->setItem(index, 1, pass);
        reportWidget->setItem(index, 2, fail);
        reportWidget->setItem(index, 3, skip);
    }
}

void TestResult::textOut(QString text)
{
    if (textWidget)
    {
        textWidget->moveCursor(QTextCursor::End);
        textWidget->insertPlainText(text);
        textWidget->moveCursor(QTextCursor::End);
    }
}

void TestResult::textClear()
{
    if (textWidget)
        textWidget->clear();
}

void TestResult::createReport(QList<TestCase *> *caseList)
{
    this->caseList = caseList;

    setReportSize(caseList->size() + 1);

    int pass = 0;
    int fail = 0;
    int skip = 0;
    foreach(TestCase *testCase, *caseList) {
        pass += testCase->passNum();
        fail += testCase->failNum();
        skip += testCase->skipNum();
    }
    QTableWidgetItem *summaryItem = new QTableWidgetItem(tr("Summary:"));
    QTableWidgetItem *passCountItem = new QTableWidgetItem(QString::number(pass));
    QTableWidgetItem *failCountItem = new QTableWidgetItem(QString::number(fail));
    QTableWidgetItem *skipCountItem = new QTableWidgetItem(QString::number(skip));
    summaryItem->setTextAlignment(Qt::AlignRight);
    passCountItem->setTextAlignment(Qt::AlignRight);
    failCountItem->setTextAlignment(Qt::AlignRight);
    skipCountItem->setTextAlignment(Qt::AlignRight);
    setReportItem(0, summaryItem, passCountItem, failCountItem, skipCountItem);

    for (int index = 0; index < caseList->size(); index++) {
        QTableWidgetItem *caseItem = new QTableWidgetItem(caseList->at(index)->caseName());
        QTableWidgetItem *passItem = new QTableWidgetItem(QString::number(caseList->at(index)->passNum()));
        QTableWidgetItem *failItem = new QTableWidgetItem(QString::number(caseList->at(index)->failNum()));
        QTableWidgetItem *skipItem = new QTableWidgetItem(QString::number(caseList->at(index)->skipNum()));
        caseItem->setTextAlignment(Qt::AlignRight);
        passItem->setTextAlignment(Qt::AlignRight);
        failItem->setTextAlignment(Qt::AlignRight);
        skipItem->setTextAlignment(Qt::AlignRight);
        setReportItem(index+1, caseItem, passItem, failItem, skipItem);
    }
}

void TestResult::clearReport()
{
    setReportSize(0);
}

void TestResult::showCaseLog(int row, int /*column*/)
{
    if (row > 0) {
        DialogLog dlgLog(reportWidget);
        dlgLog.showLog(caseList->at(row-1)->testOutput());
    }
}

DialogLog::DialogLog(QWidget *parent):
        QDialog(parent)
{
    setModal(true);
}

DialogLog::~DialogLog()
{
}

void DialogLog::showLog(QString text)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    textWidget = new QTextEdit(this);
    textWidget->setReadOnly(true);
    textWidget->setTextInteractionFlags(Qt::NoTextInteraction);
    textWidget->setContextMenuPolicy(Qt::NoContextMenu);
    layout->addWidget(textWidget, 0, 0, 1, -1);

#if defined(Q_OS_SYMBIAN)
    menuBar = new QMenuBar(this);
    menuBar->setContextMenuPolicy(Qt::NoContextMenu);
    closeAction = new QAction(tr("Close"), this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    menuBar->addAction(closeAction);
    layout->addWidget(menuBar, 1, 0);
#endif

    buttonClose = new QPushButton(tr("Close"), this);
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
    layout->addWidget(buttonClose, 1, 2);

    textWidget->insertPlainText(text);

    this->showMaximized();
    this->exec();
}

QT_END_NAMESPACE

