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
#include "qhildonappmenu.h"
#include "qhildonappmenu_p.h"//TODO Add private and copy it in include directory

QT_BEGIN_NAMESPACE


void QHildonAppMenuPrivate::createButtons(QList<QAction*> actions){
    
}

void QHildonAppMenuPrivate::addButton(QAction* action){
    
}

/*!
    \class QHildonAppMenu
    \brief The QHildonAppMenu class is the default Maemo5 Application Menu widget.

    \ingroup menu

    \mainclass
    Add a description here.
*/

QHildonAppMenu::QHildonAppMenu(QWidget *parent)
    : QWidget(*new QHildonAppMenuPrivate, parent, QFlag(Qt::Dialog))
{
    
}

QHildonAppMenu::~QHildonAppMenu()
{
    
}
