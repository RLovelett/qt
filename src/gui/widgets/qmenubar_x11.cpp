/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#include "qmenubar_x11_p.h"

#ifndef QT_NO_MENUBAR

#include "qapplication.h"
#include "qdebug.h"
#include "qevent.h"
#include "qmenu.h"
#include "qmenubar.h"

QT_BEGIN_NAMESPACE

QX11MenuBar::~QX11MenuBar()
{
}

void QX11MenuBar::init(QMenuBar *_menuBar)
{
    nativeMenuBar = -1;
    menuBar = _menuBar;
}

void QX11MenuBar::setVisible(bool visible)
{
    menuBar->QWidget::setVisible(visible);
}

void QX11MenuBar::actionEvent(QActionEvent *e)
{
    Q_UNUSED(e);
}

void QX11MenuBar::handleReparent(QWidget *oldParent, QWidget *newParent, QWidget *oldWindow, QWidget *newWindow)
{
    Q_UNUSED(oldParent)
    Q_UNUSED(newParent)
    Q_UNUSED(oldWindow)
    Q_UNUSED(newWindow)
}

bool QX11MenuBar::allowCornerWidgets() const
{
    return true;
}

void QX11MenuBar::popupAction(QAction *)
{
}

void QX11MenuBar::setNativeMenuBar(bool value)
{
    if (nativeMenuBar == -1 || (value != bool(nativeMenuBar))) {
        nativeMenuBar = value;
    }
}

bool QX11MenuBar::isNativeMenuBar() const
{
    return false;
}

bool QX11MenuBar::shortcutsHandledByNativeMenuBar() const
{
    return false;
}

bool QX11MenuBar::menuBarEventFilter(QObject *, QEvent *)
{
    return false;
}

QT_END_NAMESPACE

#endif // QT_NO_MENUBAR
