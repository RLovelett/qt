/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the unofficial Maemo Qt Toolkit.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include "qhildonstyle.h"

#include <QtGui/QStyleOption>
#include <QDebug>

#include "qgtkpainter_p.h"

#if !defined(QT_NO_STYLE_HILDON) && defined(Q_WS_HILDON)

QT_BEGIN_NAMESPACE

//Belongs to QGTKStyle
static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

/*!
    \class QHildonStyle
    \brief The QHildonStyle class provides a widget style rendered by GTK+

    The QHildonStyle style provides a look and feel that integrates well
    into Hildon-based desktop environments.

    It's based on QGTKStyle.

    Note: The style requires GTK+ version 2.10 or later.
          The Qt3-based "Qt" GTK+ theme engine will not work with QGtkStyle.
*/

/*!
    Constructs a QGtkStyle object.
*/
QHildonStyle::QHildonStyle()
{
}

/*!
    Destroys the QHildonStyle object.
*/
QHildonStyle::~QHildonStyle()
{
}

QPalette QHildonStyle::standardPalette() const
{
    return QGtkStyle::standardPalette();
}

void QHildonStyle::polish(QPalette &palette)
{
    QGtkStyle::polish(palette);
}

void QHildonStyle::polish(QApplication *app)
{
    QGtkStyle::polish(app);
}

void QHildonStyle::unpolish(QApplication *app)
{
    QGtkStyle::unpolish(app);
}

void QHildonStyle::polish(QWidget *widget)
{
    QGtkStyle::polish(widget);
}

void QHildonStyle::unpolish(QWidget *widget)
{
    QGtkStyle::unpolish(widget);
}

int QHildonStyle::pixelMetric(PixelMetric metric,
                           const QStyleOption *option,
                           const QWidget *widget) const
{
   if (!QGtk::isThemeAvailable())
        return QCleanlooksStyle::pixelMetric(metric, option, widget);
   
   switch (metric) {
    //coordinate of the Application Context Menu upper left corner.
    case PM_MenuOffsetHorizontal:
    {
        GtkWidget *gtkMenu = QGtk::gtkWidget("menu_force_with_corners");
        gint horizontal_offset;
        QGtk::gtk_widget_style_get(gtkMenu, "horizontal-offset", &horizontal_offset, NULL);
        return horizontal_offset;
    }
    case PM_MenuOffsetVertical:
    {
        GtkWidget *gtkMenu = QGtk::gtkWidget("menu_force_with_corners");
        gint vertical_offset;
        QGtk::gtk_widget_style_get(gtkMenu, "vertical-offset", &vertical_offset, NULL);
        return vertical_offset;
    }
    default:
        return  QGtkStyle::pixelMetric(metric, option, widget);
    }
}

int QHildonStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,
                         QStyleHintReturn *returnData = 0) const
{
    if (!QGtk::isThemeAvailable())
        return QCleanlooksStyle::styleHint(hint, option, widget, returnData);

    switch (hint) {
    case SH_Menu_Scrollable:
        return int(true);
    case SH_DialogButtonBox_ButtonsHaveIcons:
    case SH_ScrollBar_ContextMenu:
        return int(false);
    default:
        return QGtkStyle::styleHint(hint, option, widget, returnData);
    }
}

void QHildonStyle::drawPrimitive(PrimitiveElement element,
                              const QStyleOption *option,
                              QPainter *painter,
                              const QWidget *widget) const
{
    if (!QGtk::isThemeAvailable()) {
        QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
        return;
    }

    GtkStyle* style = QGtk::gtkStyle();
    QGtkPainter gtkPainter(painter);

    switch (element) {
    case PE_FrameLineEdit: {
        GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("GtkEntry"));

        if (option->state & State_HasFocus)
            GTK_WIDGET_SET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
        else
            GTK_WIDGET_UNSET_FLAGS(gtkEntry, GTK_HAS_FOCUS);

        gboolean interior_focus;
        gint focus_line_width;
        QRect rect = option->rect;
        QGtk::gtk_widget_style_get(gtkEntry,
                               "interior-focus", &interior_focus,
                               "focus-line-width", &focus_line_width, NULL);
        
        //Paint the LineEdits borders.
        gtkPainter.paintFlatBox(gtkEntry, "entry_bg", rect,
                                  option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, gtkEntry->style);

        if (!interior_focus && option->state & State_HasFocus)
            rect.adjust(focus_line_width, focus_line_width, -focus_line_width, -focus_line_width);

        gtkPainter.paintShadow(gtkEntry, "entry", rect, option->state & State_Enabled ? 
                               GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE, 
                               GTK_SHADOW_IN, gtkEntry->style,
                               option->state & State_HasFocus ? QLS("focus") : QString());

        if (!interior_focus && option->state & State_HasFocus)
            gtkPainter.paintShadow(gtkEntry, "entry", option->rect, option->state & State_Enabled ? 
                                   GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                                   GTK_SHADOW_IN, gtkEntry->style, QLS("GtkEntryShadowIn"));

    }
    break;

    case PE_PanelLineEdit: {
        QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
    }
    break;

    default:
        QGtkStyle::drawPrimitive(element, option, painter, widget);
    }
}

void QHildonStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                   QPainter *painter, const QWidget *widget) const
{
    if (!QGtk::isThemeAvailable()) {
        QCleanlooksStyle::drawComplexControl(control, option, painter, widget);
        return;
    }

    GtkStyle* style = QGtk::gtkStyle();
    QGtkPainter gtkPainter(painter);
    QColor button = option->palette.button().color();
    QColor dark;
    QColor grooveColor;
    QColor darkOutline;
    dark.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*1.9)),
                qMin(255, (int)(button.value()*0.7)));
    grooveColor.setHsv(button.hue(),
                       qMin(255, (int)(button.saturation()*2.6)),
                       qMin(255, (int)(button.value()*0.9)));
    darkOutline.setHsv(button.hue(),
                       qMin(255, (int)(button.saturation()*3.0)),
                       qMin(255, (int)(button.value()*0.6)));

    QColor alphaCornerColor;

    if (widget)
        alphaCornerColor = mergedColors(option->palette.color(widget->backgroundRole()), darkOutline);
    else
        alphaCornerColor = mergedColors(option->palette.background().color(), darkOutline);

    QPalette palette = option->palette;

    switch (control) {
    #ifndef QT_NO_SPINBOX
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            GtkWidget *gtkSpinButton = QGtk::gtkWidget(QLS("GtkSpinButton"));
            bool isEnabled = (spinBox->state & State_Enabled);
            bool hover = isEnabled && (spinBox->state & State_MouseOver);
            bool sunken = (spinBox->state & State_Sunken);
            bool upIsActive = (spinBox->activeSubControls == SC_SpinBoxUp);
            bool downIsActive = (spinBox->activeSubControls == SC_SpinBoxDown);
            bool reverse = (spinBox->direction == Qt::RightToLeft);

            GtkWidget *gtkEntry = QGtk::gtkWidget("HildonNumberEditor.GtkEntry");
            GtkWidget *gtkMinusButton = QGtk::gtkWidget("HildonNumberEditor.ne-minus-button");
            GtkWidget *gtkPlusButton = QGtk::gtkWidget("HildonNumberEditor.ne-plus-button");
            int xt = gtkEntry->style->xthickness;
            int yt = gtkEntry->style->ythickness;
            QRect plusRect = subControlRect(CC_SpinBox, option, SC_SpinBoxUp, widget);
            QRect minusRect = subControlRect(CC_SpinBox, option, SC_SpinBoxDown, widget);
            QRect entryRect = subControlRect(CC_SpinBox, option, SC_SpinBoxEditField, widget).adjusted(-xt, -yt, xt, yt);

            /* TODO: Do not replicate code from PE_FrameLineEdit */
            gboolean interior_focus;
            gint focus_line_width;
            QRect rect = entryRect;
            QGtk::gtk_widget_style_get(gtkEntry,
                                "interior-focus", &interior_focus,
                                "focus-line-width", &focus_line_width,
                                NULL);

            if (option->state & State_HasFocus)
                GTK_WIDGET_SET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
            else
                GTK_WIDGET_UNSET_FLAGS(gtkEntry, GTK_HAS_FOCUS);

#if defined Q_WS_HILDON && !defined Q_OS_FREMANTLE
            gtkPainter.paintFlatBox(gtkEntry, "entry_bg", entryRect, GTK_STATE_NORMAL, GTK_SHADOW_NONE, gtkEntry->style);

            if (!interior_focus && option->state & State_HasFocus)
                rect.adjust(focus_line_width, focus_line_width, -focus_line_width, -focus_line_width);

            gtkPainter.paintShadow(gtkEntry, "entry", option->rect, GTK_STATE_NORMAL, GTK_SHADOW_IN, gtkEntry->style);

            if (!interior_focus && option->state & State_HasFocus)
                gtkPainter.paintShadow(gtkEntry, "entry", option->rect, GTK_STATE_ACTIVE, GTK_SHADOW_IN, gtkEntry->style);
#endif
            /* Plus button */
            GtkStateType state = (upIsActive && sunken) ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL;
            if (!isEnabled)
                state = GTK_STATE_INSENSITIVE;

            gtkPainter.paintBox(gtkPlusButton, "button",  plusRect, state, GTK_SHADOW_OUT, gtkPlusButton->style);

            /* Minus button */
            state = (downIsActive && sunken) ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL;
            if (!isEnabled)
                state = GTK_STATE_INSENSITIVE;
            gtkPainter.paintBox(gtkMinusButton, "button",  minusRect, state, GTK_SHADOW_OUT, gtkMinusButton->style);
        }
        break;
#endif // QT_NO_SPINBOX

    default:
        QGtkStyle::drawComplexControl(control, option, painter, widget);
    }
}

void QHildonStyle::drawControl(ControlElement element,
                            const QStyleOption *option,
                            QPainter *painter,
                            const QWidget *widget) const
{
    QGtkStyle::drawControl(element, option, painter, widget);
}

QRect QHildonStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                SubControl subControl, const QWidget *widget) const
{
    QRect rect = QWindowsStyle::subControlRect(control, option, subControl, widget);
    if (!QGtk::isThemeAvailable())
        return QCleanlooksStyle::subControlRect(control, option, subControl, widget);

    switch (control) {
#ifndef QT_NO_SPINBOX
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            GtkWidget *gtkEntry = QGtk::gtkWidget("HildonNumberEditor.GtkEntry");
            int buttonSize = 30;
            int padding = 2;
            int h = qMax(spinbox->rect.height(), buttonSize);
            int w = qMax(spinbox->rect.width(), (buttonSize + padding) * 2);
            int x = spinbox->rect.x();
            int y = spinbox->rect.y();
            int buttonY = y + (h - buttonSize) / 2;
            int xt = gtkEntry->style->xthickness;
            int yt = gtkEntry->style->ythickness;

            switch (subControl) {
            case SC_SpinBoxUp:
                rect = QRect(x + w - buttonSize, buttonY, buttonSize, buttonSize);
                break;
            case SC_SpinBoxDown:
                rect = QRect(x, buttonY, buttonSize, buttonSize);
                break;
            case SC_SpinBoxEditField:
                rect = QRect(x + buttonSize + padding + xt, y + yt, w - 2 * (buttonSize + padding + xt), h - 2 * yt);
                break;
            case SC_SpinBoxFrame:
                rect = spinbox->rect;
            default:
                break;
            }
            rect = visualRect(spinbox->direction, spinbox->rect, rect);
        }
        break;
#endif // Qt_NO_SPINBOX
    default:
        return  QGtkStyle::subControlRect(control, option, subControl, widget);
    }
    return rect;
}

QSize QHildonStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                                  const QSize &size, const QWidget *widget) const
{
   return  QGtkStyle::sizeFromContents(type, option, size, widget);
}

QPixmap QHildonStyle::standardPixmap(StandardPixmap sp, const QStyleOption *option,
                                  const QWidget *widget) const
{
   return  QGtkStyle::standardPixmap(sp, option, widget);
}

QRect QHildonStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    return  QGtkStyle::subElementRect(element, option, widget);
}

QRect QHildonStyle::itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
{
    return QGtkStyle::itemPixmapRect(r, flags, pixmap);
}

void QHildonStyle::drawItemPixmap(QPainter *painter, const QRect &rect,
                            int alignment, const QPixmap &pixmap) const
{
    QGtkStyle::drawItemPixmap(painter, rect, alignment, pixmap);
}

QStyle::SubControl QHildonStyle::hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                              const QPoint &pt, const QWidget *w) const
{
    return QGtkStyle::hitTestComplexControl(cc, opt, pt, w);
}

QPixmap QHildonStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                        const QStyleOption *opt) const
{
    return QGtkStyle::generatedIconPixmap(iconMode, pixmap, opt);
}

void QHildonStyle::drawItemText(QPainter *painter, const QRect &rect, int alignment, const QPalette &pal,
                                    bool enabled, const QString& text, QPalette::ColorRole textRole) const
{
    QGtkStyle::drawItemText(painter, rect, alignment, pal, enabled, text, textRole);
}

QT_END_NAMESPACE

#endif

