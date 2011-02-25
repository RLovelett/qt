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

#ifndef BENCHMARKOPTDLG_H
#define BENCHMARKOPTDLG_H

#include <QDialog>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QCheckBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class GlobalConfig;

class BenchmarkOptDlg : public QDialog
{
    Q_OBJECT

public:
    BenchmarkOptDlg(GlobalConfig *config = 0, QWidget *parent = 0);
    virtual ~BenchmarkOptDlg();

private:
    void createWidgets();
    void setupWidgetsEventHandlers();
    void load();
    void setLayout();
    void changeBtnSaveEnableState();

private slots:
    void changeVbBenchmarkState(int state);
    void changeTickCounterState(int state);
    void changeEventCounterState(int state);
    void changeMinValState(int state);
    void changeIterState(int state);
    void changeMedianState(int state);
    void changeMinVal(QString text);
    void changeIter(QString text);
    void changeMedian(QString text);
    void changeCallGrindState(int state);
    void saveOpt();
    void closeDlg();
    void showHelp();

signals:
    void bmOptSaved(const GlobalConfig &);

private:
    GlobalConfig    *initConfig;
    GlobalConfig    *savedConfig;
    QCheckBox       *checkboxVerbose;
    QCheckBox       *checkboxTickCounter;
    QCheckBox       *checkboxEventCounter;
    QCheckBox       *checkboxMinValue;
    QLineEdit       *lineEditMinVal;
    QCheckBox       *checkboxIterations;
    QLineEdit       *lineEditIterations;
    QCheckBox       *checkboxMedian;
    QLineEdit       *lineEditMedian;
#if defined (Q_OS_LINUX)
    QCheckBox       *checkboxCallGrind;
#endif
    QPushButton     *buttonSave;
    QPushButton     *buttonCancel;
    QPushButton     *buttonHelp;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // BENCHMARKOPTDLG_H
