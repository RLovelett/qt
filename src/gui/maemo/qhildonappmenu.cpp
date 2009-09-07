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
#include "private/qhildonappmenu_p.h"//TODO Add private and copy it in include directory

#include <qlayout.h>
#include <qpushbutton.h>
#include <qaction.h>
#include <qdebug.h>
#include <qlayout.h>

QT_BEGIN_NAMESPACE

void QHildonAppMenuPrivate::init(){
    Q_Q(QHildonAppMenu);
    q->resize(700, 100);
    gridLayout = new QGridLayout(q);
    gridLayout->setSpacing(0);
}

void QHildonAppMenuPrivate::createButtons(QList<QAction*> actions){
    QAction *action;
    foreach(action, actions){
        addButton(action);
    }
}

void QHildonAppMenuPrivate::addButton(QAction* action){
    Q_Q(QHildonAppMenu);

    int row, culumn;
    QPushButton *pushButton = new QPushButton(q);
    pushButton->setMinimumSize(QSize(0, 65));
    pushButton->setText(action->text());
    buttonList.insert(pushButton, action);
    
    culumn = ((buttonList.count() % 2) == 0) ? 1 : 0;
    row = (buttonList.count() - culumn)/ 2;
    
    gridLayout->addWidget(pushButton, row, culumn);

    QObject::connect(pushButton, SIGNAL(clicked()), q, SLOT(_q_activateAction()));
}

#if 0
void QHildonAppMenuPrivate::_q_activateAction()
{
    Q_Q(QHildonAppMenu);

    QAction *action = buttonList[sender()];
    //qDebug() << "activate QAction sender=" << sender();
    if (action)
        action->activate(QAction::Trigger);
    else
        qWarning("Impossible to activate the action");

}
#endif 

/*!
    \class QHildonAppMenu
    \brief The QHildonAppMenu class is the default Maemo5 Application Menu widget.

    \ingroup menu

    \mainclass
    Add a description here.
*/

QHildonAppMenu::QHildonAppMenu(QList<QAction*> actions, QWidget *parent)
    : QDialog(*new QHildonAppMenuPrivate, parent, QFlag(Qt::HildonAppMenu))
{
    Q_D(QHildonAppMenu);
    d->init();
    d->createButtons(actions);
}

QHildonAppMenu::~QHildonAppMenu()
{
    qDebug() << "Destroyed";
}

void QHildonAppMenu::_q_activateAction()
{
    Q_D(QHildonAppMenu);

    QAction *action = d->buttonList[sender()];
    //qDebug() << "activate QAction sender=" << sender();
    if (action){
        action->activate(QAction::Trigger);
        close();
    }else{
        qWarning("Impossible to activate the action");
    }
}
