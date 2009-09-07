/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: antonio.aloisio@nokia.com
**
**
** GNU General Public License Usage
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
****************************************************************************/
#ifndef QHILDONAPPMENU_P_H
#define QHILDONAPPMENU_P_H

#include "private/qdialog_p.h"
#include "QtGui/qhildonappmenu.h" 

QT_BEGIN_NAMESPACE

class QGridLayout;
class QAction;
class QDesktopWidget;

class QHildonAppMenuPrivate : public QDialogPrivate
{
    Q_DECLARE_PUBLIC(QHildonAppMenu)
public:
    enum orientation { Landscape, Portrait };
    inline QHildonAppMenuPrivate(){};
    void init();
    void createButtons(QList<QAction*> actions);
    void addButton(QAction* action);
    void updateSize();

    QGridLayout *gridLayout;
    QHash<QObject*, QAction*> buttonList;
    QDesktopWidget * desktop; 
    orientation menuMode; 
    //void _q_activateAction();
};

QT_END_NAMESPACE    

#endif
