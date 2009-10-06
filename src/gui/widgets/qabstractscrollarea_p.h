/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QABSTRACTSCROLLAREA_P_H
#define QABSTRACTSCROLLAREA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qframe_p.h"
#include "qabstractscrollarea.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SCROLLAREA

class QScrollBar;
class QAbstractScrollAreaScrollBarContainer;
class QAbstractScrollAreaScroller;
class Q_GUI_EXPORT QAbstractScrollAreaPrivate: public QFramePrivate
{
    Q_DECLARE_PUBLIC(QAbstractScrollArea)

public:
    QAbstractScrollAreaPrivate();

    void replaceScrollBar(QScrollBar *scrollBar, Qt::Orientation orientation);

    QAbstractScrollAreaScrollBarContainer *scrollBarContainers[Qt::Vertical + 1];
    QScrollBar *hbar, *vbar;
    Qt::ScrollBarPolicy vbarpolicy, hbarpolicy;

    QWidget *viewport;
    QWidget *cornerWidget;
    QRect cornerPaintingRect;
#ifdef Q_WS_MAC
    QRect reverseCornerPaintingRect;
#endif
    int left, top, right, bottom; // viewport margin

    int xoffset, yoffset;

    void init();
    void layoutChildren();
    // ### Fix for 4.4, talk to Bjoern E or Girish.
    virtual void scrollBarPolicyChanged(Qt::Orientation, Qt::ScrollBarPolicy);

    void _q_hslide(int);
    void _q_vslide(int);
    void _q_showOrHideScrollBars();

    virtual QPoint contentsOffset() const;
#ifdef Q_WS_HILDON
    QAbstractScrollAreaScroller *fingerScroller;
#endif
    inline bool viewportEvent(QEvent *event)
    { return q_func()->viewportEvent(event); }
    QObject *viewportFilter;
};

class QAbstractScrollAreaFilter : public QObject
{
    Q_OBJECT
    
public:
    QAbstractScrollAreaFilter(QAbstractScrollAreaPrivate *p) : d(p)
    { setObjectName(QLatin1String("qt_abstractscrollarea_filter")); }
    bool eventFilter(QObject *o, QEvent *e)
    { return (o == d->viewport ? d->viewportEvent(e) : false); }
private:
    QAbstractScrollAreaPrivate *d;
};

class QBoxLayout;
class QAbstractScrollAreaScrollBarContainer : public QWidget
{
public:
    enum LogicalPosition { LogicalLeft = 1, LogicalRight = 2 };

    QAbstractScrollAreaScrollBarContainer(Qt::Orientation orientation, QWidget *parent);
    void addWidget(QWidget *widget, LogicalPosition position);
    QWidgetList widgets(LogicalPosition position);
    void removeWidget(QWidget *widget);

    QScrollBar *scrollBar;
    QBoxLayout *layout;
private:
    int scrollBarLayoutIndex() const;

    Qt::Orientation orientation;
};

#ifdef Q_WS_HILDON
// This is the Kinetic fingerscroll implementation for Hildon
#include <qqueue.h>
#include <qdatetime.h>


class QAbstractScrollAreaScroller : public QObject
{
    Q_OBJECT
public:
    QAbstractScrollAreaScroller(QAbstractScrollArea *parent,
                                QAbstractScrollAreaPrivate *parentPriv);
    ~QAbstractScrollAreaScroller();
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void replayEvents();

private:
    void drawOvershoot(QPoint overshoot);
    void setScrollbarsStyle(int fremantleStyle); //TODO Use QHildon style instead of stylesheets
    void handleMoveEvent ( QMouseEvent * event );
    void timerEvent(QTimerEvent *event);
    void registerChildrenForFingerScrolling(QObject *top);

    static const int SENSITIVITY, 
                     KINETIC_REFRESH,
                     VSCALE,
                     DECEL_DURATION,
                     DEBOUNCE,
                     DECEL_PC,
                     START_WITHIN,
                     VMAX,
                     MAX_OVERSHOOT,
                     OVERSHOOT_DECEL_PC,
                     REBOUND_ACCEL;
    
    QAbstractScrollArea* scrollArea;  // the area we're scrolling
    QAbstractScrollAreaPrivate* scrollAreaPriv; //Ugly: Fast way to get Margins

    typedef enum { NotScrolling = 0,
                   Maybe,
                   ManualScroll,
                   AutoScroll,
                   OverShootDecel,
                   OverShootPause,
                   OverShootStabilise,
                   ReissuingEvents } ScrollState;

    ScrollState  scrollState,
                 xScrollState,
                 yScrollState;

    QPointer<QWidget> eventSourceWidget; // who do we play events back to
    QQueue<QEvent*>   storedEvents;      // events to play back

    QPoint scrollBase;           // scrollbar value at fingerdown to establish base position
    QPoint start;                // global event x,y at start
    QPoint curr;                 // current x,y (kept to allow velocity calc)
    QPoint rest;                 // rest x,y (resting place after overshoot

    QPointF scrollFactor;        // scroll range factor - converts pixel delta to sb value delta

    QTime event_time;
    QPoint last;                 // global event x,y at start
    QPoint vel;                  // Velocity
    QPoint vel1;                 // Velocity before
    int last_ev_time, curr_time; // timer for velocity
        
    int scrollingNotStartedTimer;
    int x_braking_cycles;          // how long do we slow down for
    int y_braking_cycles;          // how long do we slow down for
    QPoint overshoot;            // overshoot x,y (distance past scrollarea limits)
};
#endif // Q_WS_HILDON

#endif // QT_NO_SCROLLAREA

QT_END_NAMESPACE

#endif // QABSTRACTSCROLLAREA_P_H
