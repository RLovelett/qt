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
#ifndef QHILDONAPPMENU_H
#define QHILDONAPPMENU_H

#include <QtGui/qdialog.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QHildonAppMenuPrivate;

class Q_GUI_EXPORT QHildonAppMenu : public QDialog
{
    Q_OBJECT

public:
    explicit QHildonAppMenu(QList<QAction*> actions, QWidget *parent = 0);
    virtual ~QHildonAppMenu();

private:
    Q_DECLARE_PRIVATE(QHildonAppMenu)
    Q_DISABLE_COPY(QHildonAppMenu)

    //FIXME Doesn't compile :( 
    //Q_PRIVATE_SLOT(d_func(), void _q_activateAction())
private Q_SLOTS:
    void _q_activateAction();
    void _q_screenResized();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDIALOG_H
