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
#include <qlayout.h>
#include <qdesktopwidget.h>
#include <qapplication.h>

QT_BEGIN_NAMESPACE

void QHildonAppMenuPrivate::init(){
    Q_Q(QHildonAppMenu);
    q->setAttribute(Qt::WA_DeleteOnClose);
    action = 0;

    desktop = QApplication::desktop();
    QObject::connect(desktop, SIGNAL(resized(int)), q, SLOT(_q_screenResized(int)));
    updateSize();

    gridLayout = new QGridLayout(q);
    gridLayout->setSpacing(0);
}

void QHildonAppMenuPrivate::createButtons(QList<QAction*> actions){
    QAction *action;
    foreach(action, actions){
        if (action->isVisible() && 
            action->isEnabled() &&
            !action->isSeparator() &&
            !action->menu())
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
    
    switch(menuMode){
    case Landscape:
        culumn = ((buttonList.count() % 2) == 0) ? 1 : 0;
        row = (buttonList.count() - culumn)/ 2;
        break;
    case Portrait:
        culumn = 0;
        row = gridLayout->rowCount();
    }

    gridLayout->addWidget(pushButton, row, culumn);

    QObject::connect(pushButton, SIGNAL(clicked()), q, SLOT(_q_activateAction()));
}

void QHildonAppMenuPrivate::updateSize(){
    Q_Q(QHildonAppMenu);
    int menuWidth = desktop->screenGeometry(q).width() -100;
    q->resize(menuWidth, 100);
    menuMode = (menuWidth > 480) ? Landscape : Portrait; 
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
    QHildoinAppMenu shows QActions in a widget like the HildonAppMenu.
    This class should not be used by developer. It should be just for internal use.
    
    QHildonAppMenu shows actions that are enabled and visible. 
    Separator or menu QActions won't be shown.

    The widget is destroyed as soon as a button is clicked or if the user click outside the 
    menu.
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
}

void QHildonAppMenu::hideEvent(QHideEvent * event)
{
    Q_D(QHildonAppMenu);
    Q_UNUSED(event);
    QAction *action = d->action;
    if (action)
        action->activate(QAction::Trigger);
    done(0);
}

void QHildonAppMenu::_q_activateAction()
{
    Q_D(QHildonAppMenu);

    QAction *action = d->buttonList[sender()];
    if (action){
        d->action = action;
        hide();
    }else{
        qWarning("Impossible to activate the action");
    }
}

void QHildonAppMenu::_q_screenResized(int screen)
{
    Q_UNUSED(screen);
    //Close the menu so that we don't care about
    //sorting the buttons again.
    done(1);    
}