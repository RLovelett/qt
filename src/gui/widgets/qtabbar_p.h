/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QTABBAR_P_H
#define QTABBAR_P_H

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

#include "qtabbar.h"
#include "private/qwidget_p.h"

#include <qicon.h>
#include <qtoolbutton.h>
#include <qtimeline.h>
#include <qhash.h>
#include <qdebug.h>

#ifndef QT_NO_TABBAR

#define ANIMATION_DURATION 250

#include <qstyleoption.h>

QT_BEGIN_NAMESPACE

class QTabBarPrivate  : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QTabBar)
public:
    QTabBarPrivate()
        :currentIndex(-1), pressedIndex(-1),
         shape(QTabBar::RoundedNorth),
         layoutDirty(false), drawBase(true), scrollOffset(0), expanding(true), closeButtonOnTabs(false), selectionBehaviorOnRemove(QTabBar::SelectRightTab), paintWithOffsets(true), movable(false), dragInProgress(false), documentMode(false), movingTab(0) {}

    int currentIndex;
    int pressedIndex;
    QTabBar::Shape shape;
    bool layoutDirty;
    bool drawBase;
    int scrollOffset;

    struct Tab {
        inline Tab(const QIcon &ico, const QString &txt)
            : enabled(true)
            , shortcutId(0)
            , text(txt)
            , icon(ico)
            , leftWidget(0)
            , rightWidget(0)
            , lastTab(-1)
            , timeLine(0)
            , dragOffset(0)
        {}
        bool enabled;
        int shortcutId;
        QString text;
#ifndef QT_NO_TOOLTIP
        QString toolTip;
#endif
#ifndef QT_NO_WHATSTHIS
        QString whatsThis;
#endif
        QIcon icon;
        QRect rect;
        QRect minRect;
        QRect maxRect;

        QColor textColor;
        QVariant data;
        QWidget *leftWidget;
        QWidget *rightWidget;
        int lastTab;

        QTimeLine *timeLine;
        int dragOffset;

        void makeTimeLine(QWidget *q) {
            if (timeLine)
                return;
            timeLine = new QTimeLine(ANIMATION_DURATION, q);
            q->connect(timeLine, SIGNAL(frameChanged(int)), q, SLOT(_q_moveTab(int)));
            q->connect(timeLine, SIGNAL(finished()), q, SLOT(_q_moveTabFinished()));
        }

    };
    QList<Tab> tabList;
    QHash<QTimeLine*, int> animations;

    int calculateNewPosition(int from, int to, int index) const;
    void slide(int from, int to);
    void init();
    int extraWidth() const;

    Tab *at(int index);
    const Tab *at(int index) const;

    int indexAtPos(const QPoint &p) const;

    inline bool validIndex(int index) const { return index >= 0 && index < tabList.count(); }
    void setCurrentNextEnabledIndex(int offset);

    QSize minimumTabSizeHint(int index);

    QToolButton* rightB; // right or bottom
    QToolButton* leftB; // left or top

    void _q_scrollTabs();
    void _q_closeTab();
    void _q_moveTab(int);
    void _q_moveTabFinished();
    void _q_moveTabFinished(int offset);
    QRect hoverRect;

    void refresh();
    void layoutTabs();
    void layoutWidgets(int index = -1);
    void layoutTab(int index);
    void updateMacBorderMetrics();
    void setupMovableTab();

    void makeVisible(int index);
    QSize iconSize;
    Qt::TextElideMode elideMode;
    bool useScrollButtons;

    bool expanding;
    bool closeButtonOnTabs;
    QTabBar::SelectionBehavior selectionBehaviorOnRemove;

    QPoint dragStartPosition;
    bool paintWithOffsets;
    bool movable;
    bool dragInProgress;
    bool documentMode;

    QWidget *movingTab;

    // shared by tabwidget and qtabbar
    static void initStyleBaseOption(QStyleOptionTabBarBaseV2 *optTabBase, QTabBar *tabbar, QSize size)
    {
        QStyleOptionTab tabOverlap;
        tabOverlap.shape = tabbar->shape();
        int overlap = tabbar->style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &tabOverlap, tabbar);
        QWidget *theParent = tabbar->parentWidget();
        optTabBase->init(tabbar);
        optTabBase->shape = tabbar->shape();
        optTabBase->documentMode = tabbar->documentMode();
        if (theParent && overlap > 0) {
            QRect rect;
            switch (tabOverlap.shape) {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
                rect.setRect(0, size.height()-overlap, size.width(), overlap);
                break;
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
                rect.setRect(0, 0, size.width(), overlap);
                break;
            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
                rect.setRect(0, 0, overlap, size.height());
                break;
            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
                rect.setRect(size.width() - overlap, 0, overlap, size.height());
                break;
            }
            optTabBase->rect = rect;
        }
    }

};

class CloseButton : public QAbstractButton
{
    Q_OBJECT

public:
    CloseButton(QWidget *parent = 0);

    QSize sizeHint() const;
    inline QSize minimumSizeHint() const
        { return sizeHint(); }
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
};


QT_END_NAMESPACE

#endif

#endif
