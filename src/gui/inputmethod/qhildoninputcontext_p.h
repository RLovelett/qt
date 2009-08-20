/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** This file is part of the QtGui module of the Maemo Qt project.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QHILDONINPUTCONTEXT_P_H
#define QHILDONINPUTCONTEXT_P_H

#include "QtGui/qinputcontext.h"
#include "qhildoninputmethodprotocol_p.h"

#ifdef Q_WS_HILDON

QT_BEGIN_HEADER

QT_MODULE(Gui)

class QHildonInputContext : public QInputContext
{
    Q_OBJECT
public:
    explicit QHildonInputContext(QObject* parent = 0);
    ~QHildonInputContext();

    QString identifierName();
    QString language();
    void reset();
    bool isComposing() const;
    void setFocusWidget(QWidget *w);
    bool eventFilter(QObject *obj, QEvent *event);
    void update();

protected:
    void toggleHildonMainIMUi();
    void timerEvent(QTimerEvent *);

    //Filters
    bool filterKeyPress(QWidget *w,QKeyEvent *ev);
    bool x11FilterEvent(QWidget *keywidget, XEvent *event);

private:
    bool canUseIM(QWidget *w);
    void insertUtf8(int flag, const QString& text);
    void clearSelection();

    void sendHildonCommand(HildonIMCommand cmd, QWidget *widget=0);
    void sendX11Event(XEvent *event);
    void showHIMMainUI();

    //Context
    void checkSentenceStart();
    void commitPreeditData();
    void checkCommitMode(); //### REMOVE?
    void sendSurrounding(bool sendAllContents = false);
    void sendSurroundingHeader(int offset);
    void inputModeChanged();
    void sendInputMode();
    void setClientCursorLocation(int offsetIsRelative, int cursorOffset); //FIXME?? Move offsetIsRelative to Bool?

    //Vars
    int timerId;
    int mask;
    int options;
    HildonIMTrigger triggerMode;
    HildonIMCommitMode commitMode;
    int inputMode;
    QString preEditBuffer;
    QString surrounding;
    int textCursorPosOnPress; //position of the cursor in the surrounding text at the last TabletPress event
    bool autoUpper;
    bool lastInternalChange;
};

QT_END_HEADER

#endif // Q_WS_HILDON

#endif //QHILDONINPUTCONTEXT_P_H
