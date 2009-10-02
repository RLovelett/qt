/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtGui/QLineEdit>

#include "qpixmapcache.h"
#undef signals // Collides with GTK stymbols
#include "qgtkpainter_p.h"

#include <private/qcleanlooksstyle_p.h>


#if !defined(QT_NO_STYLE_HILDON) && defined(Q_WS_HILDON)

QT_BEGIN_NAMESPACE

static bool UsePixmapCache = true;

static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

// Note this is different from uniqueName as used in QGtkPainter
static QString uniqueName(const QString &key, const QStyleOption *option, const QSize &size)
{
    QString tmp;
    const QStyleOptionComplex *complexOption = qstyleoption_cast<const QStyleOptionComplex *>(option);
    tmp.sprintf("%s-%d-%d-%d-%lld-%dx%d", key.toLatin1().constData(), uint(option->state),
                option->direction, complexOption ? uint(complexOption->activeSubControls) : uint(0),
                option->palette.cacheKey(), size.width(), size.height());
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
    case PM_DialogButtonsButtonHeight:
        return 65;
    case PM_DialogButtonsButtonWidth:
        return 180;
    case PM_ToolBarIconSize:
        return 48;
    case PM_ButtonShiftHorizontal: {
        GtkWidget *gtkButton = QGtk::gtkWidget(QLS("HildonButton-finger"));
        guint horizontal_shift;
        QGtk::gtk_widget_style_get(gtkButton, "child-displacement-x", &horizontal_shift, NULL);
        return horizontal_shift;
    }
    case PM_ButtonShiftVertical: {
        GtkWidget *gtkButton = QGtk::gtkWidget(QLS("HildonButton-finger"));
        guint vertical_shift;
        QGtk::gtk_widget_style_get(gtkButton, "child-displacement-y", &vertical_shift, NULL);
        return vertical_shift;
    }
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
    case PE_PanelButtonCommand: {
        bool isDefault = false;
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton*>(option))
            isDefault = btn->features & QStyleOptionButton::DefaultButton;

        GtkStateType state = gtkPainter.gtkState(option);
        if (option->state & State_On || option->state & State_Sunken)
            state = GTK_STATE_ACTIVE;
        GtkWidget *gtkButton = QGtk::gtkWidget(QLS("HildonButton-finger"));
        gint focusWidth, focusPad;
        gboolean interiorFocus = false;
        QGtk::gtk_widget_style_get (gtkButton,
                                "focus-line-width", &focusWidth,
                                "focus-padding", &focusPad,
                                "interior-focus", &interiorFocus, NULL);

        style = gtkButton->style;

        QRect buttonRect = option->rect;

        QString key;
        if (isDefault) {
            key += QLS("def");
            GTK_WIDGET_SET_FLAGS(gtkButton, GTK_HAS_DEFAULT);
            gtkPainter.paintBox(gtkButton, "buttondefault", buttonRect, state, GTK_SHADOW_IN,
                                style, isDefault ? QLS("d") : QString());
        }

        bool hasFocus = option->state & State_HasFocus;

        if (hasFocus) {
            key += QLS("def");
            GTK_WIDGET_SET_FLAGS(gtkButton, GTK_HAS_FOCUS);
        }

        if (!interiorFocus)
            buttonRect = buttonRect.adjusted(focusWidth, focusWidth, -focusWidth, -focusWidth);

        GtkShadowType shadow = (option->state & State_Sunken || option->state & State_On ) ?
                               GTK_SHADOW_IN : GTK_SHADOW_OUT;

        gtkPainter.paintBox(gtkButton, "button", buttonRect, state, shadow,
                            style, key);
        if (isDefault)
            GTK_WIDGET_UNSET_FLAGS(gtkButton, GTK_HAS_DEFAULT);
        if (hasFocus)
            GTK_WIDGET_UNSET_FLAGS(gtkButton, GTK_HAS_FOCUS);
    }
    break;

    case PE_PanelLineEdit:
        if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("HildonEntry-finger"));

            uint resolve_mask = option->palette.resolve();

            //HildonEntry-finger could be ythickness = 0; We MUST adjust the height of the rect.
            QRect textRect = option->rect.adjusted(gtkEntry->style->xthickness, 
                                                   gtkEntry->style->ythickness ? gtkEntry->style->ythickness : gtkEntry->style->xthickness,
                                                   -gtkEntry->style->xthickness, 
                                                   -(gtkEntry->style->ythickness ? gtkEntry->style->ythickness : gtkEntry->style->xthickness) );

                 
            const QLineEdit *le = qobject_cast<const QLineEdit*>(widget);

            if (le && le->hasFrame()) {
                gtkPainter.paintFlatBox(gtkEntry, "entry_bg", option->rect,
                                        option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, gtkEntry->style);
                drawPrimitive(PE_FrameLineEdit, option, painter, widget);
            } else {
                //QLineEdit used as Delegate widgets (Eg: in QTableView) should not have lineWidth in Hildon.
                //In this case we have to cover the widget on the back.
                QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
            }

        }
        break;
    case PE_FrameLineEdit: {
        const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(option);
        if (!panel)
            break;

            //Draws a big frame
            GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("HildonEntry-finger"));

            gboolean interior_focus;
            gint focus_line_width;
            QRect rect = option->rect;
            QGtk::gtk_widget_style_get(gtkEntry,
                                   "interior-focus", &interior_focus,
                                   "focus-line-width", &focus_line_width, NULL);

            //See https://bugzilla.mozilla.org/show_bug.cgi?id=405421 for info about this hack
            g_object_set_data(G_OBJECT(gtkEntry), "transparent-bg-hint", GINT_TO_POINTER(TRUE));

            if (!interior_focus && option->state & State_HasFocus)
                rect.adjust(focus_line_width, focus_line_width, -focus_line_width, -focus_line_width);

       
            gtkPainter.paintShadow(gtkEntry, "entry", rect, option->state & State_Enabled ? 
                                   (option->state & State_HasFocus ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL) : GTK_STATE_INSENSITIVE,
                                   GTK_SHADOW_IN, gtkEntry->style,
                                   option->state & State_HasFocus ? QLS("focus") : QString());
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
    #ifndef QT_NO_COMBOBOX

    case CC_ComboBox:
        // See: http://live.gnome.org/GnomeArt/Tutorials/GtkThemes/GtkComboBox
        // and http://live.gnome.org/GnomeArt/Tutorials/GtkThemes/GtkComboBoxEntry
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            bool sunken = comboBox->state & State_On; // play dead, if combobox has no items
            BEGIN_STYLE_PIXMAPCACHE(QString::fromLatin1("cb-%0-%1").arg(sunken).arg(comboBox->editable));
            QGtkPainter gtkCachedPainter(p);
            gtkCachedPainter.setUsePixmapCache(false); // cached externally

            bool isEnabled = (comboBox->state & State_Enabled);
            bool focus = isEnabled && (comboBox->state & State_HasFocus);
            QColor buttonShadow = option->palette.dark().color();
            GtkStateType state = gtkPainter.gtkState(option);
            int appears_as_list = !styleHint(QStyle::SH_ComboBox_Popup, comboBox, widget);
            QPixmap cache;
            QString pixmapName;
            QStyleOptionComboBox comboBoxCopy = *comboBox;
            comboBoxCopy.rect = option->rect;

            bool reverse = (option->direction == Qt::RightToLeft);
            QRect rect = option->rect;
            QRect arrowButtonRect = subControlRect(CC_ComboBox, &comboBoxCopy,
                                                   SC_ComboBoxArrow, widget);
            QRect editRect = subControlRect(CC_ComboBox, &comboBoxCopy,
                                            SC_ComboBoxEditField, widget);

            GtkShadowType shadow = (option->state & State_Sunken || option->state & State_On ) ?
                                   GTK_SHADOW_IN : GTK_SHADOW_OUT;
            QString comboBoxPath = QLS(comboBox->editable ? "GtkComboBoxEntry" : "GtkComboBox");

            // We use the gtk widget to position arrows and separators for us
            GtkWidget *gtkCombo = QGtk::gtkWidget(comboBoxPath);
            GtkAllocation geometry = {0, 0, option->rect.width(), option->rect.height()};
            QGtk::gtk_widget_set_direction(gtkCombo, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
            QGtk::gtk_widget_size_allocate(gtkCombo, &geometry);

            QString buttonPath = comboBoxPath + QLS(".GtkToggleButton");
            GtkWidget *gtkToggleButton = QGtk::gtkWidget(buttonPath);
            QGtk::gtk_widget_set_direction(gtkToggleButton, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
            if (gtkToggleButton && (appears_as_list || comboBox->editable)) {
                if (focus)
                    GTK_WIDGET_SET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);
                // Draw the combo box as a line edit with a button next to it
                if (comboBox->editable || appears_as_list) {
                    GtkStateType frameState = (state == GTK_STATE_PRELIGHT) ? GTK_STATE_NORMAL : state;
                    QString entryPath = QLS(comboBox->editable ? "GtkComboBoxEntry.GtkEntry" : "GtkComboBox.GtkFrame");
                    GtkWidget *gtkEntry = QGtk::gtkWidget(entryPath);
                    QGtk::gtk_widget_set_direction(gtkEntry, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
                    QRect frameRect = option->rect;

                    if (reverse)
                        frameRect.setLeft(arrowButtonRect.right());
                    else
                        frameRect.setRight(arrowButtonRect.left());

                    uint resolve_mask = option->palette.resolve();
                    int xt = gtkEntry->style->xthickness;
                    int yt = gtkEntry->style->ythickness;
                    QRect contentRect = frameRect.adjusted(xt, yt, -xt, -yt);
                                     
                    if (comboBox->editable) {
                        gtkCachedPainter.paintFlatBox(gtkEntry, "entry_bg", contentRect,
                                                      option->state & State_Enabled ? GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                                                      GTK_SHADOW_NONE, gtkEntry->style, entryPath + QString::number(focus));
                    } else {
                        //Evil hack used by Gtkrc
                        GdkColor gdkBg;
                        QColor bg;
                        gdkBg = gtkCombo->style->base[option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE];
                        bg = QColor(gdkBg.red>>8, gdkBg.green>>8, gdkBg.blue>>8);
                        p->fillRect(contentRect, bg);
                    }

                    gtkCachedPainter.paintShadow(gtkEntry, comboBox->editable ? "entry" : "frame", frameRect, frameState,
                                           GTK_SHADOW_IN, gtkEntry->style, entryPath +
                                           QString::number(focus) + QString::number(comboBox->editable) +
                                           QString::number(option->direction));
                }

                GtkStateType buttonState = GTK_STATE_NORMAL;

                if (!(option->state & State_Enabled))
                    buttonState = GTK_STATE_INSENSITIVE;
                else if (option->state & State_Sunken || option->state & State_On)
                    buttonState = GTK_STATE_ACTIVE;
                else if (option->state & State_MouseOver && comboBox->activeSubControls & SC_ComboBoxArrow)
                    buttonState = GTK_STATE_PRELIGHT;

                QRect buttonrect = QRect(gtkToggleButton->allocation.x, gtkToggleButton->allocation.y,
                                         gtkToggleButton->allocation.width, gtkToggleButton->allocation.height);

                Q_ASSERT(gtkToggleButton);
                gtkCachedPainter.paintBox( gtkToggleButton, "button", arrowButtonRect, buttonState,
                                     shadow, gtkToggleButton->style, buttonPath +
                                     QString::number(focus) + QString::number(option->direction));
                if (focus)
                    GTK_WIDGET_UNSET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);
            } else {
                // Draw combo box as a button
                QRect buttonRect = option->rect;

                if (focus) // Clearlooks actually check the widget for the default state
                    GTK_WIDGET_SET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);
                gtkCachedPainter.paintBox(gtkToggleButton, "button",
                                    buttonRect, state,
                                    shadow, gtkToggleButton->style,
                                    buttonPath + QString::number(focus));
                if (focus)
                    GTK_WIDGET_UNSET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);

                // Draw the separator between label and arrows
                QString vSeparatorPath = buttonPath + QLS(".GtkHBox.GtkVSeparator");

                if (GtkWidget *gtkVSeparator = QGtk::gtkWidget(vSeparatorPath)) {
                    QRect vLineRect(gtkVSeparator->allocation.x,
                                    gtkVSeparator->allocation.y,
                                    gtkVSeparator->allocation.width,
                                    gtkVSeparator->allocation.height);

                    gtkCachedPainter.paintVline( gtkVSeparator, "vseparator",
                                           vLineRect, state, gtkVSeparator->style,
                                           0, vLineRect.height(), 0,  vSeparatorPath);


                    gint interiorFocus = true;
                    QGtk::gtk_widget_style_get(gtkToggleButton, "interior-focus", &interiorFocus, NULL);
                    int xt = interiorFocus ? gtkToggleButton->style->xthickness : 0;
                    int yt = interiorFocus ? gtkToggleButton->style->ythickness : 0;
                    if ((focus && (option->state & State_KeyboardFocusChange)))
                        gtkCachedPainter.paintFocus(gtkToggleButton, "button",
                                              option->rect.adjusted(xt, yt, -xt, -yt),
                                              option->state & State_Sunken ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL,
                                              gtkToggleButton->style);
                }
            }

            if (comboBox->subControls & SC_ComboBoxArrow) {
                if (!isEnabled)
                    state = GTK_STATE_INSENSITIVE;
                else if (sunken)
                    state = GTK_STATE_ACTIVE;
                else if (option->state & State_MouseOver)
                    state = GTK_STATE_PRELIGHT;
                else
                    state = GTK_STATE_NORMAL;

                QString arrowPath = comboBoxPath + QLS(appears_as_list ? ".GtkToggleButton.GtkArrow"
                                                    : ".GtkToggleButton.GtkHBox.GtkArrow");

                GtkWidget *gtkArrow = QGtk::gtkWidget(arrowPath);
                gfloat scale = 0.7;
                gint minSize = 15;
                QRect arrowWidgetRect;

                if (gtkArrow && !QGtk::gtk_check_version(2, 12, 0)) {
                    QGtk::gtk_widget_style_get(gtkArrow, "arrow-scaling", &scale, NULL);
                    QGtk::gtk_widget_style_get(gtkCombo, "arrow-size", &minSize, NULL);
                }
                if (gtkArrow) {
                    arrowWidgetRect = QRect(gtkArrow->allocation.x, gtkArrow->allocation.y,
                                            gtkArrow->allocation.width, gtkArrow->allocation.height);
                    style = gtkArrow->style;
                }

                // Note that for some reason the arrow-size is not properly respected with Hildon
                // Hence we enforce the minimum "arrow-size" ourselves
                int arrowSize = qMax(qMin(rect.height() - gtkCombo->style->ythickness * 2, minSize),
                                     qMin(arrowWidgetRect.width(), arrowWidgetRect.height()));
                QRect arrowRect(0, 0, static_cast<int>(arrowSize * scale), static_cast<int>(arrowSize * scale));

                arrowRect.moveCenter(arrowWidgetRect.center());

                if (sunken) {
                    int xoff, yoff;
                    GtkWidget *gtkButton = QGtk::gtkWidget(comboBoxPath + QLS(".GtkToggleButton"));
                    QGtk::gtk_widget_style_get(gtkButton, "child-displacement-x", &xoff, NULL);
                    QGtk::gtk_widget_style_get(gtkButton, "child-displacement-y", &yoff, NULL);
                    arrowRect = arrowRect.adjusted(xoff, yoff, xoff, yoff);
                }

                // Some styles such as Nimbus paint outside the arrowRect
                // hence we have provide the whole widget as the cliprect
                if (gtkArrow) {
                    gtkCachedPainter.setClipRect(option->rect);
                    gtkCachedPainter.paintArrow( gtkArrow, "arrow", arrowRect,
                                           GTK_ARROW_DOWN, state, GTK_SHADOW_NONE, TRUE,
                                           style, arrowPath + QString::number(option->direction));
                }
            }
            END_STYLE_PIXMAPCACHE;
        }
        break;
#endif // QT_NO_COMBOBOX

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
    if (!QGtk::isThemeAvailable()) {
        QCleanlooksStyle::drawControl(element, option, painter, widget);
        return;
    }

    GtkStyle* style = QGtk::gtkStyle();
    QGtkPainter gtkPainter(painter);

    switch (element) {

    default:
        QGtkStyle::drawControl(element, option, painter, widget);
    }
    
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
    QSize newSize = QCleanlooksStyle::sizeFromContents(type, option, size, widget);
    if (!QGtk::isThemeAvailable())
        return newSize;

    switch (type) {
    case CT_LineEdit: {
        const QLineEdit *le = qobject_cast<const QLineEdit*>(widget);
        if (le && !le->hasFrame()){
            newSize = QGtkStyle::sizeFromContents(type, option, size, widget);
            break;
        }
        GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("HildonEntry-finger"));
        newSize = size + QSize(2*gtkEntry->style->xthickness,
                               2*qMax(gtkEntry->style->ythickness, gtkEntry->style->xthickness));
    }
    break;
        case CT_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            GtkWidget *gtkButton = QGtk::gtkWidget(QLS("HildonButton-finger"));
            gint focusPadding, focusWidth;
            QGtk::gtk_widget_style_get(gtkButton, "focus-padding", &focusPadding, NULL);
            QGtk::gtk_widget_style_get(gtkButton, "focus-line-width", &focusWidth, NULL);
            newSize = size;
            newSize += QSize(2*gtkButton->style->xthickness + 4, 2*gtkButton->style->ythickness);
            newSize += QSize(2*(focusWidth + focusPadding + 2), 2*(focusWidth + focusPadding));

            GtkWidget *gtkButtonBox = QGtk::gtkWidget(QLS("GtkHButtonBox"));
            gint minWidth = 85, minHeight = 0;
            QGtk::gtk_widget_style_get(gtkButtonBox, "child-min-width", &minWidth,
                                   "child-min-height", &minHeight, NULL);

            //osso-hbuttonbox min height is 47, we want to set minHeight to 65px (Finger size)
            minHeight = qMax(minHeight, 65);

            if (!btn->text.isEmpty() && newSize.width() < minWidth)
                newSize.setWidth(minWidth);
            if (newSize.height() < minHeight)
                newSize.setHeight(minHeight);
        }

        break;
    default:
        return  QGtkStyle::sizeFromContents(type, option, size, widget);
    }
    return newSize;
}

QPixmap QHildonStyle::standardPixmap(StandardPixmap sp, const QStyleOption *option,
                                  const QWidget *widget) const
{
   return  QGtkStyle::standardPixmap(sp, option, widget);
}

QRect QHildonStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    QRect r;
    switch (element) {
    case SE_LineEditContents: {
        const QLineEdit *le = qobject_cast<const QLineEdit*>(widget);
        if (le && !le->hasFrame())
            return  QGtkStyle::subElementRect(element, option, widget);

        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            GtkWidget *gtkEntry = QGtk::gtkWidget(QLS("HildonEntry-finger"));
            int x = qMax(gtkEntry->style->xthickness, f->lineWidth);
            int y = qMax(gtkEntry->style->ythickness, f->lineWidth);
            r = f->rect.adjusted(x,y, -x, -y);
            r = visualRect(option->direction, option->rect, r);
        }
    }
    break;

    default:
        return  QGtkStyle::subElementRect(element, option, widget);
    }
    return r;
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

