/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QStatusBar;
QT_END_NAMESPACE
class TabletCanvas;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(TabletCanvas *canvas);

private slots:
    void brushColorAct();
    void alphaActionTriggered(QAction *action);
    void lineWidthActionTriggered(QAction *action);
    void saturationActionTriggered(QAction *action);
    void saveAct();
    void loadAct();
    void aboutAct();

private:
    void createActions();
    void createMenus();

    TabletCanvas *myCanvas;

    QAction *brushColorAction;
    QActionGroup *brushActionGroup;

    QActionGroup *alphaChannelGroup;
    QAction *alphaChannelPressureAction;
    QAction *alphaChannelTiltAction;
    QAction *noAlphaChannelAction;

    QActionGroup *colorSaturationGroup;
    QAction *colorSaturationVTiltAction;
    QAction *colorSaturationHTiltAction;
    QAction *colorSaturationPressureAction;
    QAction *noColorSaturationAction;

    QActionGroup *lineWidthGroup;
    QAction *lineWidthPressureAction;
    QAction *lineWidthTiltAction;
    QAction *lineWidthFixedAction;

    QAction *exitAction;
    QAction *saveAction;
    QAction *loadAction;

    QAction *aboutAction;
    QAction *aboutQtAction;

    QMenu *fileMenu;
    QMenu *brushMenu;
    QMenu *tabletMenu;
    QMenu *helpMenu;
    QMenu *colorSaturationMenu;
    QMenu *lineWidthMenu;
    QMenu *alphaChannelMenu;
};
//! [0]

#endif
