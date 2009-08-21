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
#include "qdebug.h"
#include "qhildoninputcontext_p.h"
#include "qpointer.h"
#include "qapplication.h"
#include "qclipboard.h"
#include "qplaintextedit.h"
#include "qlineedit.h"
#include "qtextedit.h"
#include "qtextbrowser.h"
#include "kernel/qevent_p.h"       //QKeyEventEx
#include "kernel/qapplication_p.h" //QApplicationPrivate::areXInputEventsUsed()
#include "qinputcontext.h"

#ifdef Q_WS_HILDON 

#define HIM_DEBUG

#define GDK_ISO_ENTER  0xfe34
#define COMPOSE_KEY    Qt::Key_Multi_key   // "Ch" key
#define LEVEL_KEY      Qt::Key_AltGr       //"Fn" key

#define LEVEL_KEY_MOD_MASK   0x4
#define STATE_LEVEL_KEY_MASK 0x80

#define STATE_CONTROL_MASK  1 << 2
#define STATE_SHIFT_MASK    1 << 0 

//Keyboard layout levels
#define NUMERIC_LEVEL 2
#define LOCKABLE_LEVEL 4

#define PRESSURE_THRESHOLD  0.40

/* TODO
   - Cleaning up

   Fremantle:
   - Read Gconf (?) Settings for Auto-Capitalization, Word completion and insert space after word
     Currently these are switched on.
   - TRIGGER STYLUS is sent all the time, this prevent the usage of fullscreen virtual keyboard (Bug tracked, waiting for a tix)
   - sendSurrounding when sendAllContents == true
   - hildon banner integration to show the modifiers status (Sticky/Lock).
   - put Q_OS_FREMANTLE in configure

   Diablo
   - Fix dead key support 
*/

extern bool qt_sendSpontaneousEvent(QObject*, QEvent*); //qapplication_x11.cpp

/*!
 * keysym to QString functions
 * These functions are part of qkeymapper_x11.cpp
 */
static const unsigned short katakanaKeysymsToUnicode[] = {
    0x0000, 0x3002, 0x300C, 0x300D, 0x3001, 0x30FB, 0x30F2, 0x30A1,
    0x30A3, 0x30A5, 0x30A7, 0x30A9, 0x30E3, 0x30E5, 0x30E7, 0x30C3,
    0x30FC, 0x30A2, 0x30A4, 0x30A6, 0x30A8, 0x30AA, 0x30AB, 0x30AD,
    0x30AF, 0x30B1, 0x30B3, 0x30B5, 0x30B7, 0x30B9, 0x30BB, 0x30BD,
    0x30BF, 0x30C1, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CB, 0x30CC,
    0x30CD, 0x30CE, 0x30CF, 0x30D2, 0x30D5, 0x30D8, 0x30DB, 0x30DE,
    0x30DF, 0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6, 0x30E8, 0x30E9,
    0x30EA, 0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F3, 0x309B, 0x309C
};

static const unsigned short cyrillicKeysymsToUnicode[] = {
    0x0000, 0x0452, 0x0453, 0x0451, 0x0454, 0x0455, 0x0456, 0x0457,
    0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x0000, 0x045e, 0x045f,
    0x2116, 0x0402, 0x0403, 0x0401, 0x0404, 0x0405, 0x0406, 0x0407,
    0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x0000, 0x040e, 0x040f,
    0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
    0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e,
    0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
    0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
    0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
    0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e,
    0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
    0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a
};

static const unsigned short greekKeysymsToUnicode[] = {
    0x0000, 0x0386, 0x0388, 0x0389, 0x038a, 0x03aa, 0x0000, 0x038c,
    0x038e, 0x03ab, 0x0000, 0x038f, 0x0000, 0x0000, 0x0385, 0x2015,
    0x0000, 0x03ac, 0x03ad, 0x03ae, 0x03af, 0x03ca, 0x0390, 0x03cc,
    0x03cd, 0x03cb, 0x03b0, 0x03ce, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
    0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f,
    0x03a0, 0x03a1, 0x03a3, 0x0000, 0x03a4, 0x03a5, 0x03a6, 0x03a7,
    0x03a8, 0x03a9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7,
    0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf,
    0x03c0, 0x03c1, 0x03c3, 0x03c2, 0x03c4, 0x03c5, 0x03c6, 0x03c7,
    0x03c8, 0x03c9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static const unsigned short technicalKeysymsToUnicode[] = {
    0x0000, 0x23B7, 0x250C, 0x2500, 0x2320, 0x2321, 0x2502, 0x23A1,
    0x23A3, 0x23A4, 0x23A6, 0x239B, 0x239D, 0x239E, 0x23A0, 0x23A8,
    0x23AC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x2264, 0x2260, 0x2265, 0x222B,
    0x2234, 0x221D, 0x221E, 0x0000, 0x0000, 0x2207, 0x0000, 0x0000,
    0x223C, 0x2243, 0x0000, 0x0000, 0x0000, 0x21D4, 0x21D2, 0x2261,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x221A, 0x0000,
    0x0000, 0x0000, 0x2282, 0x2283, 0x2229, 0x222A, 0x2227, 0x2228,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2202,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0192, 0x0000,
    0x0000, 0x0000, 0x0000, 0x2190, 0x2191, 0x2192, 0x2193, 0x0000
};

static const unsigned short specialKeysymsToUnicode[] = {
    0x25C6, 0x2592, 0x2409, 0x240C, 0x240D, 0x240A, 0x0000, 0x0000,
    0x2424, 0x240B, 0x2518, 0x2510, 0x250C, 0x2514, 0x253C, 0x23BA,
    0x23BB, 0x2500, 0x23BC, 0x23BD, 0x251C, 0x2524, 0x2534, 0x252C,
    0x2502, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static const unsigned short publishingKeysymsToUnicode[] = {
    0x0000, 0x2003, 0x2002, 0x2004, 0x2005, 0x2007, 0x2008, 0x2009,
    0x200a, 0x2014, 0x2013, 0x0000, 0x0000, 0x0000, 0x2026, 0x2025,
    0x2153, 0x2154, 0x2155, 0x2156, 0x2157, 0x2158, 0x2159, 0x215a,
    0x2105, 0x0000, 0x0000, 0x2012, 0x2329, 0x0000, 0x232a, 0x0000,
    0x0000, 0x0000, 0x0000, 0x215b, 0x215c, 0x215d, 0x215e, 0x0000,
    0x0000, 0x2122, 0x2613, 0x0000, 0x25c1, 0x25b7, 0x25cb, 0x25af,
    0x2018, 0x2019, 0x201c, 0x201d, 0x211e, 0x0000, 0x2032, 0x2033,
    0x0000, 0x271d, 0x0000, 0x25ac, 0x25c0, 0x25b6, 0x25cf, 0x25ae,
    0x25e6, 0x25ab, 0x25ad, 0x25b3, 0x25bd, 0x2606, 0x2022, 0x25aa,
    0x25b2, 0x25bc, 0x261c, 0x261e, 0x2663, 0x2666, 0x2665, 0x0000,
    0x2720, 0x2020, 0x2021, 0x2713, 0x2717, 0x266f, 0x266d, 0x2642,
    0x2640, 0x260e, 0x2315, 0x2117, 0x2038, 0x201a, 0x201e, 0x0000
};

static const unsigned short aplKeysymsToUnicode[] = {
    0x0000, 0x0000, 0x0000, 0x003c, 0x0000, 0x0000, 0x003e, 0x0000,
    0x2228, 0x2227, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x00af, 0x0000, 0x22a5, 0x2229, 0x230a, 0x0000, 0x005f, 0x0000,
    0x0000, 0x0000, 0x2218, 0x0000, 0x2395, 0x0000, 0x22a4, 0x25cb,
    0x0000, 0x0000, 0x0000, 0x2308, 0x0000, 0x0000, 0x222a, 0x0000,
    0x2283, 0x0000, 0x2282, 0x0000, 0x22a2, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x22a3, 0x0000, 0x0000, 0x0000
};

static const unsigned short koreanKeysymsToUnicode[] = {
    0x0000, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137,
    0x3138, 0x3139, 0x313a, 0x313b, 0x313c, 0x313d, 0x313e, 0x313f,
    0x3140, 0x3141, 0x3142, 0x3143, 0x3144, 0x3145, 0x3146, 0x3147,
    0x3148, 0x3149, 0x314a, 0x314b, 0x314c, 0x314d, 0x314e, 0x314f,
    0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156, 0x3157,
    0x3158, 0x3159, 0x315a, 0x315b, 0x315c, 0x315d, 0x315e, 0x315f,
    0x3160, 0x3161, 0x3162, 0x3163, 0x11a8, 0x11a9, 0x11aa, 0x11ab,
    0x11ac, 0x11ad, 0x11ae, 0x11af, 0x11b0, 0x11b1, 0x11b2, 0x11b3,
    0x11b4, 0x11b5, 0x11b6, 0x11b7, 0x11b8, 0x11b9, 0x11ba, 0x11bb,
    0x11bc, 0x11bd, 0x11be, 0x11bf, 0x11c0, 0x11c1, 0x11c2, 0x316d,
    0x3171, 0x3178, 0x317f, 0x3181, 0x3184, 0x3186, 0x318d, 0x318e,
    0x11eb, 0x11f0, 0x11f9, 0x0000, 0x0000, 0x0000, 0x0000, 0x20a9
};

static QChar keysymToUnicode(unsigned char byte3, unsigned char byte4)
{
    switch (byte3) {
    case 0x04:
        // katakana
        if (byte4 > 0xa0 && byte4 < 0xe0)
            return QChar(katakanaKeysymsToUnicode[byte4 - 0xa0]);
        else if (byte4 == 0x7e)
            return QChar(0x203e); // Overline
        break;
    case 0x06:
        // russian, use lookup table
        if (byte4 > 0xa0)
            return QChar(cyrillicKeysymsToUnicode[byte4 - 0xa0]);
        break;
    case 0x07:
        // greek
        if (byte4 > 0xa0)
            return QChar(greekKeysymsToUnicode[byte4 - 0xa0]);
        break;
    case 0x08:
        // technical
        if (byte4 > 0xa0)
            return QChar(technicalKeysymsToUnicode[byte4 - 0xa0]);
        break;
    case 0x09:
        // special
        if (byte4 >= 0xe0)
            return QChar(specialKeysymsToUnicode[byte4 - 0xe0]);
        break;
    case 0x0a:
        // publishing
        if (byte4 > 0xa0)
            return QChar(publishingKeysymsToUnicode[byte4 - 0xa0]);
        break;
    case 0x0b:
        // APL
        if (byte4 > 0xa0)
            return QChar(aplKeysymsToUnicode[byte4 - 0xa0]);
        break;
    case 0x0e:
        // Korean
        if (byte4 > 0xa0)
            return QChar(koreanKeysymsToUnicode[byte4 - 0xa0]);
        break;
    default:
        break;
    }
    return QChar(0x0);
}

static QString translateKeySym(KeySym keysym)
{
    if (!keysym) 
        return QString();

    // all keysyms smaller than 0xff00 are actally keys that can be mapped to unicode chars
    extern QTextCodec *qt_input_mapper; // from qapplication_x11.cpp
    QTextCodec *mapper = qt_input_mapper;
    QChar converted;

    QByteArray chars;
    int count=0;

    if (keysym < 0xff00) {
        unsigned char byte3 = (unsigned char)(keysym >> 8);
        int mib = -1;
        switch(byte3) {
        case 0: // Latin 1
        case 1: // Latin 2
        case 2: //latin 3
        case 3: // latin4
            mib = byte3 + 4; break;
        case 5: // arabic
            mib = 82; break;
        case 12: // Hebrew
            mib = 85; break;
        case 13: // Thai
            mib = 2259; break;
        case 4: // kana
        case 6: // cyrillic
        case 7: // greek
        case 8: // technical, no mapping here at the moment
        case 9: // Special
        case 10: // Publishing
        case 11: // APL
        case 14: // Korean, no mapping
            mib = -1; // manual conversion
            mapper = 0;
            converted = keysymToUnicode(byte3, keysym & 0xff);
        case 0x20:
            // currency symbols
            if (keysym >= 0x20a0 && keysym <= 0x20ac) {
                mib = -1; // manual conversion
                mapper = 0;
                converted = (uint)keysym;
            }
            break;
        default:
            break;
        }
        if (mib != -1) {
            mapper = QTextCodec::codecForMib(mib);
            if (chars.isEmpty())
                chars.resize(1);
            chars[0] = (unsigned char) (keysym & 0xff); // get only the fourth bit for conversion later
            count++;
        }
    } else if (keysym >= 0x1000000 && keysym <= 0x100ffff) {
        converted = (ushort) (keysym - 0x1000000);
        mapper = 0;
    }
    if (count < (int)chars.size()-1)
        chars[count] = '\0';

    QString text;
    if (!mapper && converted.unicode() != 0x0) {
        text = converted;
    } else if (!chars.isEmpty()) {
        // convert chars (8bit) to text (unicode).
        if (mapper)
            text = mapper->toUnicode(chars.data(), count, 0);
        if (text.isEmpty()) {
            // no mapper, or codec couldn't convert to unicode (this
            // can happen when running in the C locale or with no LANG
            // set). try converting from latin-1
            text = QString::fromLatin1(chars);
        }
    }
    return text;
}

/*! XkbLookupKeySym ( X11->display, event->nativeScanCode(), HILDON_IM_SHIFT_STICKY_MASK, &mods_rtrn, sym_rtrn)
 */
static QString translateKeycodeAndState(KeyCode key, uint state, quint32 &keysym){
    uint mods;
    KeySym *ks = reinterpret_cast<KeySym*>(&keysym);
    if ( XkbLookupKeySym ( X11->display, key, state, &mods, ks) )
        return translateKeySym(*ks);
    else
        return QString();
}

static Window findHildonIm()
{
    union
    {
        Window *win;
        unsigned char *val;
    } value;

    Window result = 0;
    ulong n = 0;
    ulong extra = 0;
    int format = 0;
    Atom realType;

    int status = XGetWindowProperty(X11->display, QX11Info::appRootWindow(),
                    ATOM(_HILDON_IM_WINDOW), 0L, 4L, 0,
                    XA_WINDOW, &realType, &format,
                    &n, &extra, (unsigned char **) &value.val);

    if (status == Success && realType == XA_WINDOW
          && format == HILDON_IM_WINDOW_ID_FORMAT && n == 1 && value.win != 0) {
        result = value.win[0];
        XFree(value.val);
    } else {
        qWarning("QHildonInputContext: Unable to get the Hildon IM window id");
    }

    return result;
}



/*! Send a key event to the IM, which makes it available to the plugins
 */
static void sendKeyEvent(QWidget *widget, QEvent::Type type, uint state, uint keyval, quint16 keycode)
{
    int gdkEventType;
    Window w = findHildonIm();

    if (!w)
        return;
    
    //Translate QEvent::Type in GDK_Event
    switch (type){
        case QEvent::KeyPress:
            gdkEventType = 8;
        break;
        case QEvent::KeyRelease:
            gdkEventType = 9;
        break;
        default:
            qWarning("QHildonInputContext: Event type not allowed");
            return;
    }

    XEvent ev;
    memset(&ev, 0, sizeof(XEvent));

    ev.xclient.type = ClientMessage;
    ev.xclient.window = w;
    ev.xclient.message_type = ATOM(_HILDON_IM_KEY_EVENT);
    ev.xclient.format = HILDON_IM_KEY_EVENT_FORMAT; 
 
    HildonIMKeyEventMessage *msg = reinterpret_cast<HildonIMKeyEventMessage *>(&ev.xclient.data);
    msg->input_window = widget->winId();

    msg->type = gdkEventType;
    msg->state = state;
    msg->keyval = keyval;
    msg->hardware_keycode = keycode;

    XSendEvent(X11->display, w, false, 0, &ev);
    XSync( X11->display, false );
}

static void setMaskState(int *mask,
                         HildonIMInternalModifierMask lock_mask,
                         HildonIMInternalModifierMask sticky_mask,
                         bool was_press_and_release)
{

  /* Pressing the key while already locked clears the state */
  if (*mask & lock_mask)
    *mask &= ~(lock_mask | sticky_mask);
  /* When the key is already sticky, a second press locks the key */
  else if (*mask & sticky_mask)
    *mask |= lock_mask;
#if 0 
  //TODO port needed for hildon_banner_show_information
  if (lock_mask & HILDON_IM_SHIFT_LOCK_MASK)
      hildon_banner_show_information (NULL, NULL, _("inpu_ib_mode_fn_locked"));
    else if (lock_mask & HILDON_IM_LEVEL_LOCK_MASK)
      hildon_banner_show_information (NULL, NULL, _("inpu_ib_mode_level_locked"));
#endif 
  /* Pressing the key for the first time stickies the key for one character,
     but only if no characters were entered while holding the key down */
  else if (was_press_and_release) {
    *mask |= sticky_mask;
  }
}

//TODO
static quint32 dead_key_to_unicode_combining_character(int qtkeycode)
{
  quint32 combining; //Unicode Hex value

  switch (qtkeycode)
  {
    case Qt::Key_Dead_Grave:            combining = 0x0300; break;
    case Qt::Key_Dead_Acute:            combining = 0x0301; break;
    case Qt::Key_Dead_Circumflex:       combining = 0x0302; break;
    case Qt::Key_Dead_Tilde:            combining = 0x0303; break;
    case Qt::Key_Dead_Macron:           combining = 0x0304; break;
    case Qt::Key_Dead_Breve:            combining = 0x032e; break;
    case Qt::Key_Dead_Abovedot:         combining = 0x0307; break;
    case Qt::Key_Dead_Diaeresis:        combining = 0x0308; break;
    case Qt::Key_Dead_Abovering:        combining = 0x030a; break;
    case Qt::Key_Dead_Doubleacute:      combining = 0x030b; break;
    case Qt::Key_Dead_Caron:            combining = 0x030c; break;
    case Qt::Key_Dead_Cedilla:          combining = 0x0327; break;
    case Qt::Key_Dead_Ogonek:           combining = 0x0328; break;
    case Qt::Key_Dead_Iota:             combining = 0; break; /* Cannot be combined */
    case Qt::Key_Dead_Voiced_Sound:     combining = 0; break; /* Cannot be combined */
    case Qt::Key_Dead_Semivoiced_Sound: combining = 0; break; /* Cannot be combined */
    case Qt::Key_Dead_Belowdot:         combining = 0x0323; break;
    case Qt::Key_Dead_Hook:             combining = 0x0309; break;
    case Qt::Key_Dead_Horn:             combining = 0x031b; break;
    default: combining = 0; break; /* Unknown dead key */
  }

  return combining;
}

/*! Sends the key as a spontaneous event.
 */
static void sendKey(QWidget *keywidget, int qtCode)
{
    QPointer<QWidget> guard = keywidget;

    KeySym keysym = NoSymbol;
    int keycode;

    switch (qtCode){
        case Qt::Key_Enter:
            keycode = 36;
        break;
        case Qt::Key_Tab:
            keycode = 66;
        break;
        case Qt::Key_Backspace:
            keycode = 22;
        break;
        default: 
        qWarning("keycode not allowed");
        return;
    }

    keysym = XKeycodeToKeysym(X11->display, keycode, 0);

    QKeyEventEx click(QEvent::KeyPress, qtCode, Qt::NoModifier , QString(), false, 1, keycode, keysym, 0);
    qt_sendSpontaneousEvent(keywidget, &click);

    // in case the widget was destroyed when the key went down
    if (guard.isNull()){
        return;
    }

    QKeyEventEx release(QEvent::KeyRelease, qtCode, Qt::NoModifier , QString(), false, 1, keycode, keysym, 0);
    qt_sendSpontaneousEvent(keywidget, &release);
}

/*!
 */
static void answerClipboardSelectionQuery(QWidget *widget)
{
    bool hasSelection = !widget->inputMethodQuery(Qt::ImCurrentSelection).toString().isEmpty();

    XEvent xev;
    Window w = findHildonIm();

    memset(&xev, 0, sizeof(xev));
    xev.xclient.type = ClientMessage;
    xev.xclient.window = w;
    xev.xclient.message_type = ATOM(_HILDON_IM_CLIPBOARD_SELECTION_REPLY);
    xev.xclient.format = HILDON_IM_CLIPBOARD_SELECTION_REPLY_FORMAT;
    xev.xclient.data.l[0] = hasSelection;

    XSendEvent(X11->display, w, false, 0, &xev);
}



const char *getNextPacketStart(const char *str)
{
    const char *candidate, *good;

    candidate = good = str;

    while (*candidate != 0) {
        ++candidate;
        if (candidate - str >= HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE)
            return good;
        good = candidate;
    }

    /* The whole string is small enough */
    return candidate;
}

KeySym getKeySymForLevel(int keycode, int level ){
    XkbDescPtr xkbDesc = XkbGetMap(X11->display, XkbAllClientInfoMask, XkbUseCoreKbd);
    KeySym keySym = XkbKeySymEntry(xkbDesc, keycode, level, 0);

    //Check for a not repated keysym
    KeySym keySymTest = XkbKeySymEntry(xkbDesc, keycode, 0, 1);
    if (keySym == keySymTest)
        return NoSymbol;

    return keySym;    
}

QHildonInputContext::QHildonInputContext(QObject* parent)
    : QInputContext(parent), timerId(-1), mask(0), 
      triggerMode(HILDON_IM_TRIGGER_NONE), commitMode(HILDON_IM_COMMIT_REDIRECT),
      inputMode(HILDON_GTK_INPUT_MODE_FULL), lastInternalChange(false)
{
}

QHildonInputContext::~QHildonInputContext()
{
    sendHildonCommand(HILDON_IM_HIDE);
}

QString QHildonInputContext::identifierName()
{
    return QLatin1String("hildon");
}

QString QHildonInputContext::language()
{
    //TODO GConf /apps/osso/inputmethod/hildon-im-languages
    return QString();
}

/*!reset the UI state 
 */
void QHildonInputContext::reset()
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::reset()" << focusWidget() <<  QApplication::focusWidget();
#endif
    QPointer<QWidget> oldFocus= focusWidget();
    
    if (oldFocus && QApplication::focusWidget() != oldFocus){
        oldFocus->removeEventFilter(oldFocus->inputContext());
        sendHildonCommand(HILDON_IM_CLEAR, oldFocus);
    }

    //Reset internals
    mask = 0;
    lastInternalChange = false;
}

bool QHildonInputContext::isComposing() const
{
    return false;
}

void QHildonInputContext::setFocusWidget(QWidget *w)
{
    if (!w)
        return;

    w->installEventFilter(this);
    QInputContext::setFocusWidget(w);
    sendHildonCommand(HILDON_IM_SETCLIENT, w);    
}

bool QHildonInputContext::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *w = static_cast<QWidget*>(obj);
    if (w) 
        QObject::eventFilter(obj, event);

    switch (event->type()){
    case QEvent::MouseButtonRelease:{
        //On the device, these events are sent at the same time of the TabletRelease ones
        if (QApplicationPrivate::areXInputEventsUsed()) break;

        //Emulate Finger poke in scratchbox
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
#ifndef Q_OS_FREMANTLE	
        if (me->button() == Qt::MidButton)
            triggerMode = HILDON_IM_TRIGGER_FINGER;
        else
            triggerMode = HILDON_IM_TRIGGER_STYLUS;
#else
	triggerMode = HILDON_IM_TRIGGER_FINGER;
#endif
        inputMode = w->inputMethodQuery(Qt::ImMode).toInt();
        toggleHildonMainIMUi(); //showHIMMainUI();
        break;
    }
    case QEvent::TabletPress:{
        textCursorPosOnPress = -1;
        break;
    }
    case QEvent::TabletMove:{
        //Moving the finger generate more than 1 TabletMove event
        if ( textCursorPosOnPress == -1){
            textCursorPosOnPress = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
        }
        break;
    }
    case QEvent::TabletRelease:{
        QTabletEvent *te = static_cast<QTabletEvent*>(event);

        //No HIM if there are popup widgets.
        if (QApplication::activePopupWidget()){ 
            break;
        }

#ifndef Q_OS_FREMANTLE
        if (te->pressure() >= PRESSURE_THRESHOLD){
            triggerMode = HILDON_IM_TRIGGER_FINGER;
        }else{
            triggerMode = HILDON_IM_TRIGGER_STYLUS;    
        }
#else
	triggerMode = HILDON_IM_TRIGGER_FINGER;
#endif
        inputMode = w->inputMethodQuery(Qt::ImMode).toInt();
        toggleHildonMainIMUi(); showHIMMainUI();
        break;
    }
    case QEvent::KeyPress:
    case QEvent::KeyRelease:{
        triggerMode = HILDON_IM_TRIGGER_KEYBOARD;
        return filterKeyPress(static_cast<QWidget*>(obj), 
                              static_cast<QKeyEvent*>(event));
    }
    default:
        //Remove compile warning
        break;
    }
    return QObject::eventFilter(obj, event);
}

//TODO
void QHildonInputContext::update()
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::update()";
#endif

    if (lastInternalChange) {
        //Autocase update
        checkSentenceStart();
        lastInternalChange = false;
    }
}

/*!  Shows/hides the Hildon Main Input Method Ui Window
 */
void QHildonInputContext::toggleHildonMainIMUi()
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::toggleHildonMainIMUi()";
#endif
    QPointer<QWidget> w = focusWidget();
    if (!w)
        return;
     
    if (!canUseIM(w)){
        sendHildonCommand(HILDON_IM_HIDE);
        return;
    }

     sendHildonCommand(HILDON_IM_SETCLIENT,w);
     if (timerId != -1){
         killTimer(timerId);
     }
     timerId = startTimer(HILDON_IM_DEFAULT_LAUNCH_DELAY);
}

/*! Shows the IM after a timeout.
 *  GTK implementation use this to improve the possibility
 *  to distinguish a finger poke.
 */
void QHildonInputContext::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() != timerId){
        return;
    }
    killTimer(timerId);

    if (QWidget *w = QApplication::focusWidget()){
        sendHildonCommand(HILDON_IM_SETNSHOW,w);
    }
}

/*! Filters spontaneous keyevents then elaborates them and updates the Hildon Main UI
 *  via XMessages. In some cases it creates and posts a new keyevent
 *  as no spontaneous event.
 */
bool QHildonInputContext::filterKeyPress(QWidget *keywidget,QKeyEvent *event){

    if (!canUseIM(keywidget))
        return false;

    //Avoid to filter events generated by this function.
    if (!event->spontaneous())
        return false;

    const quint32 state = event->nativeModifiers();
    const quint32 keycode = event->nativeScanCode();
    quint32 keysym= event->nativeVirtualKey();
    const int qtkeycode = event->key();

#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::filterKeyPress"
             << QString(" Mask:0x%1 state:0x%2 keycode:%3 keysym:0x%4 QtKey:0x%5 ").arg(mask,0,16)
                                                                        .arg(state,0,16)
                                                                        .arg(keycode)
                                                                        .arg(keysym,0,16)
                                                                        .arg(qtkeycode,0,16);
#endif

    //Drop auto repeated keys for COMPOSE_KEY
    if (qtkeycode == COMPOSE_KEY && event->isAutoRepeat()){
        return true;
    }

    //TODO MOVE
    static QWidget* lastKeywidget = 0;
    static int lastQtkeycode = 0;
    static qint32 combiningChar = 0; //Unicode rappresentation of the dead key.

    QString commitString; //String to commit to the Key Widget
 
    lastKeywidget = keywidget;
    //Reset static vars when the widget change.
    if (keywidget != lastKeywidget){
        mask = 0;
        lastKeywidget = 0;
        lastQtkeycode = 0;
        combiningChar = 0;
    }

    if (!qtkeycode)
        return true;

    //1. A dead key will not be immediately commited, but combined with the next key
    if (qtkeycode >= Qt::Key_Dead_Grave && qtkeycode <= Qt::Key_Dead_Horn)
        mask |= HILDON_IM_DEAD_KEY_MASK;
    else
        mask &= ~HILDON_IM_DEAD_KEY_MASK;

    if (mask & HILDON_IM_DEAD_KEY_MASK && combiningChar == 0)
    {        
        combiningChar = dead_key_to_unicode_combining_character(qtkeycode);//### WORKS? IMPROVE?
        return true;
    }

    /*2. Pressing any key while the compose key is pressed will keep that
     *   character from being directly submitted to the application. This
     *   allows the IM process to override the interpretation of the key 
     */
    if (qtkeycode == COMPOSE_KEY)
    {
        if (event->type() == QEvent::KeyPress)
            mask |= HILDON_IM_COMPOSE_MASK;
        else
            mask &= ~HILDON_IM_COMPOSE_MASK;
    }

    // 3 Sticky and locking keys initialization
    if (event->type() == QEvent::KeyRelease)
    {
        if (qtkeycode == Qt::Key_Shift )
        {
            setMaskState(&mask,
                         HILDON_IM_SHIFT_LOCK_MASK,
                         HILDON_IM_SHIFT_STICKY_MASK,
                         lastQtkeycode == Qt::Key_Shift);
        }else if (event->key() == LEVEL_KEY){
            setMaskState(&mask,
                         HILDON_IM_LEVEL_LOCK_MASK,
                         HILDON_IM_LEVEL_STICKY_MASK,
                         lastQtkeycode == LEVEL_KEY);
        }
    }
    //Update lastQtkeycode.
    lastQtkeycode=qtkeycode;

    if (qtkeycode == Qt::Key_Return || qtkeycode == Qt::Key_Enter || keysym == GDK_ISO_ENTER) {
        sendKeyEvent(keywidget, event->type(), state, keysym, keycode);
        lastInternalChange = true;
        return false;
    }else if (qtkeycode == Qt::Key_Tab){
        commitString = QString("\t");
    }

    /* 5. When the level key is in sticky or locked state, translate the
     *    keyboard state as if that level key was being held down.
     */
    if ((mask & (HILDON_IM_LEVEL_STICKY_MASK | HILDON_IM_LEVEL_LOCK_MASK)) || 
           state == LEVEL_KEY_MOD_MASK)
    {
        commitString = translateKeycodeAndState(keycode, STATE_LEVEL_KEY_MASK, keysym);
    }
   
    /* If the input mode is strictly numeric and the digits are level
     *  shifted on the layout, it's not necessary for the level key to
     *  be pressed at all. 
     */
    else if (options & HILDON_IM_AUTOLEVEL_NUMERIC &&
                (inputMode & HILDON_GTK_INPUT_MODE_FULL) == HILDON_GTK_INPUT_MODE_NUMERIC)
    {
        KeySym ks = getKeySymForLevel(keycode, NUMERIC_LEVEL); 
        QString string = translateKeySym(ks);

        if (!string.isEmpty()){
            keysym = ks;
            commitString = string;
        }
    }
    /* The input is forced to a predetermined level 
     */
    else if (options & HILDON_IM_LOCK_LEVEL)
    {
        KeySym ks = getKeySymForLevel(keycode, LOCKABLE_LEVEL); 
        QString string = translateKeySym(ks);

        if (!string.isEmpty()){
            keysym = ks;
            commitString = string;
        }
    } 
    /* Hardware keyboard autocapitalization  */
    if (autoUpper && inputMode & HILDON_GTK_INPUT_MODE_AUTOCAP)
    {
        QChar currentChar;
        KeySym lower = NoSymbol;
        KeySym upper = NoSymbol;

        if (commitString.isEmpty()){
            QString ks = translateKeySym(keysym);
            if (!ks.isEmpty())
                currentChar = ks.at(0);
        }else{
            currentChar = commitString.at(0);
        }

        XConvertCase(keysym, &lower, &upper);

        if (currentChar.isPrint()){
            if (state & STATE_SHIFT_MASK){
                currentChar = currentChar.toLower();
                keysym = lower;
            } else {
                currentChar = currentChar.toUpper();
                keysym = upper;
            }
            commitString = QString(currentChar); //sent to the widget
        }
    }

    //6. Shift lock or holding the shift down forces uppercase, ignoring autocap
    if (mask & HILDON_IM_SHIFT_LOCK_MASK || state & STATE_SHIFT_MASK)
    {
        KeySym lower = NoSymbol;
        KeySym upper = NoSymbol;
        XConvertCase(keysym, &lower, &upper);
        QString tempStr = translateKeySym(upper);
        if (!tempStr.isEmpty())
            commitString = tempStr.at(0);
    }else if (mask & HILDON_IM_SHIFT_STICKY_MASK){
        KeySym lower = NoSymbol;
        KeySym upper = NoSymbol;
        QString tempStr = translateKeySym(keysym);
        QChar currentChar;
        if (!tempStr.isEmpty()){
          currentChar = tempStr.at(0);
        
            /* Simulate shift key being held down in sticky state for non-printables  */
            if ( currentChar.isPrint() ){
                /*  For printable characters sticky shift negates the case,
                 *  including any autocapitalization changes 
                 */
                if ( currentChar.isUpper() ){
                    currentChar = currentChar.toLower();
                    lower = lower;
                }else{
                    currentChar = currentChar.toUpper();
                    upper = upper;
                }
                commitString = QString(currentChar); //sent to the widget  
            }
        }
    }
    
    //F. word completion manipulation (for fremantle)
    if (event->type() == QEvent::KeyPress &&
        commitMode == HILDON_IM_COMMIT_PREEDIT &&
        !preEditBuffer.isNull())
    {
        switch (qtkeycode){
            case Qt::Key_Right:{
                //TODO Move this code in commitPreeditData();
                QInputMethodEvent e;
                e.setCommitString(preEditBuffer + ' ');
                sendEvent(e);
                preEditBuffer.clear();
                return true;
            }
            case Qt::Key_Backspace:
            case Qt::Key_Left:{
                //TODO Move this code
                preEditBuffer.clear();
                QInputMethodEvent e;
                sendEvent(e);
                return true;
           }
        } 
    }

    //7. Sticky and lock state reset
    if (event->type() == QEvent::KeyPress)
    {
        if (qtkeycode != Qt::Key_Shift )
        {
            /* If not locked, pressing any character resets shift state */
            if ((mask & HILDON_IM_SHIFT_LOCK_MASK) == 0)
            {
                mask &= ~HILDON_IM_SHIFT_STICKY_MASK;
            }
        }
        if (qtkeycode != LEVEL_KEY)
        {
            /* If not locked, pressing any character resets level state */
            if ((mask & HILDON_IM_LEVEL_LOCK_MASK) == 0)
            {
                mask &= ~HILDON_IM_LEVEL_STICKY_MASK;
            }
        }
    }

    if (event->type() == QEvent::KeyRelease || state & STATE_CONTROL_MASK)
    {
#ifdef HIM_DEBUG
        qDebug() << QString("Sending state=0x%1 keysym=0x%2 keycode=%3").arg(state,0,16).arg(keysym,0,16).arg(keycode);
#endif
        sendKeyEvent(keywidget, event->type(), state, keysym, keycode);
        return false;
    }
 
     
    /* 8. Pressing a dead key twice, or if followed by a space, inputs
     *    the dead key's character representation 
     */
    if ((mask & HILDON_IM_DEAD_KEY_MASK || qtkeycode == Qt::Key_Space) && combiningChar)
    {
        qint32 last;
        last = dead_key_to_unicode_combining_character (qtkeycode);
        if ((last == combiningChar) || qtkeycode == Qt::Key_Space)
        {
            commitString = QString(combiningChar);
        }else{
            commitString = QString::fromUtf8(XKeysymToString(keysym));
        }
        combiningChar = 0;
    }else{
        /* Regular keypress */
        if (mask & HILDON_IM_COMPOSE_MASK)
        {
            sendKeyEvent(keywidget, event->type(),state, keysym, keycode);
            return true;
        }else{
            if ( commitString.isEmpty() && qtkeycode != Qt::Key_Backspace){
#ifdef HIM_DEBUG
                qDebug() << "this string" << event->text() << "will be committed";
#endif
                commitString = QString(event->text());
            }
        }
   }
    
    if ( !commitString.isEmpty() ){
        //entering a new character cleans the preedit buffer
        preEditBuffer.clear();
        QInputMethodEvent e;
        sendEvent(e);

        /* Pressing a dead key followed by a regular key combines to form
         * an accented character
         */
        if (combiningChar){ //FIXME
            commitString.append(combiningChar);//This will be sent to the widget
            const char *charStr = qPrintable(commitString);
            keysym = XStringToKeysym(charStr); //This will be sent to the IM
        }

        //Create the new event with the elaborate information,
        //then it adds the event to the events queue
        {
            QEvent::Type type = event->type();
            Qt::KeyboardModifiers modifiers= event->modifiers();
            //WARNING the qt keycode has not been updated!!
            QKeyEventEx *ke= new QKeyEventEx(type, keycode, modifiers, commitString, false, commitString.size(), keycode, keysym, state);
            QCoreApplication::postEvent(keywidget,ke);
        }

        //Send the new keysym 
        sendKeyEvent(keywidget, event->type(), state, keysym, keycode);
#if 0
        /* Non-printable characters invalidate any previous dead keys */
        if (qtkeycode != Qt::Key_Shift)
            combiningChar=0;
#endif
        lastInternalChange = true;
        return true;
    }else{
        //Send the new keysym 
        sendKeyEvent(keywidget, event->type(), state, keysym, keycode);
        if (qtkeycode == Qt::Key_Backspace){
            lastInternalChange = true; 
        }

        return false; 
    }
}

/*! Filters the XClientMessages sent by QApplication_x11
 */
bool QHildonInputContext::x11FilterEvent(QWidget *keywidget, XEvent *event)
{
    if (event->xclient.message_type == ATOM(_HILDON_IM_INSERT_UTF8)
            && event->xclient.format == HILDON_IM_INSERT_UTF8_FORMAT) {
#ifdef HIM_DEBUG
        qDebug() << "HILDON_IM_INSERT_UTF8_FORMAT" << options;
#endif
        HildonIMInsertUtf8Message *msg = (HildonIMInsertUtf8Message *)&event->xclient.data;
        insertUtf8(msg->msg_flag, QString::fromUtf8(msg->utf8_str));
        return true;

    }else if (event->xclient.message_type == ATOM(_HILDON_IM_COM)) {
        HildonIMComMessage *msg = (HildonIMComMessage *)&event->xclient.data;
        options = msg->options;
#ifdef HIM_DEBUG
        qDebug() << "OPTIONS" << options;
#endif

        switch (msg->type) {
        //Handle Keys msgs
        case HILDON_IM_CONTEXT_HANDLE_ENTER: {
            sendKey(keywidget, Qt::Key_Enter);
            return true; }
        case HILDON_IM_CONTEXT_HANDLE_TAB: {
            sendKey(keywidget, Qt::Key_Tab);
            return true; }
        case HILDON_IM_CONTEXT_HANDLE_BACKSPACE: {
            sendKey(keywidget, Qt::Key_Backspace);
            return true; }
        case HILDON_IM_CONTEXT_HANDLE_SPACE: {
            insertUtf8(HILDON_IM_MSG_CONTINUE, QChar(Qt::Key_Space));
            commitPreeditData();
            return true; }

        //Handle Clipboard msgs
        case HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY: {
            answerClipboardSelectionQuery(keywidget);
            return true; }
        case HILDON_IM_CONTEXT_CLIPBOARD_PASTE:
            if (QClipboard *clipboard = QApplication::clipboard()){
                QInputMethodEvent e;
                e.setCommitString(clipboard->text());
                sendEvent(e);
            }
            return true;
        case HILDON_IM_CONTEXT_CLIPBOARD_COPY: {
            if (QClipboard *clipboard = QApplication::clipboard())
                clipboard->setText(keywidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
            return true; }
        case HILDON_IM_CONTEXT_CLIPBOARD_CUT: {
            if (QClipboard *clipboard = QApplication::clipboard())
                clipboard->setText(keywidget->inputMethodQuery(Qt::ImCurrentSelection).toString());
            QInputMethodEvent ev;
            sendEvent(ev);
            return true; }

        //Handle commit mode msgs
        case HILDON_IM_CONTEXT_DIRECT_MODE: { //default mode
            #ifdef HIM_DEBUG
                qDebug() << "commitMode =  HILDON_IM_CONTEXT_DIRECT_MODE";
            #endif
            preEditBuffer.clear();
            commitMode = HILDON_IM_COMMIT_DIRECT;
            return true; }
        case HILDON_IM_CONTEXT_BUFFERED_MODE: {
            #ifdef HIM_DEBUG
                qDebug() << "commitMode =  HILDON_IM_CONTEXT_BUFFERED_MODE";
            #endif
            if (commitMode != HILDON_IM_COMMIT_BUFFERED){
                preEditBuffer = QString("");
                commitMode = HILDON_IM_COMMIT_BUFFERED;
            }
            return true; }
        case HILDON_IM_CONTEXT_REDIRECT_MODE: {
            #ifdef HIM_DEBUG
                qDebug() << "commitMode =  HILDON_IM_CONTEXT_REDIRECT_MODE";
            #endif
            preEditBuffer.clear();
            commitMode = HILDON_IM_COMMIT_REDIRECT;
            checkCommitMode();
            clearSelection();
            return true; }
        case HILDON_IM_CONTEXT_SURROUNDING_MODE: {
            #ifdef HIM_DEBUG
                qDebug() << "commitMode =  HILDON_IM_CONTEXT_SURROUNDING_MODE";
            #endif
            preEditBuffer.clear();
            commitMode = HILDON_IM_COMMIT_SURROUNDING;
            return true; }

        //Handle context
        case HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START: {
            #ifdef HIM_DEBUG
                qDebug() << "HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START";
            #endif
            checkSentenceStart();
            return true; }
        case HILDON_IM_CONTEXT_FLUSH_PREEDIT: {
            #ifdef HIM_DEBUG
                qDebug() << "commitMode = HILDON_IM_CONTEXT_FLUSH_PREEDIT";
            #endif
            commitPreeditData();
            return true; }
        case HILDON_IM_CONTEXT_REQUEST_SURROUNDING: {
            #ifdef HIM_DEBUG
                qDebug() << "HILDON_IM_CONTEXT_REQUEST_SURROUNDING";
            #endif
            checkCommitMode();
            sendSurrounding();
            //if (self->is_url_entry)
            //  hildon_im_context_send_command(self, HILDON_IM_SELECT_ALL);
            return true; }
        case HILDON_IM_CONTEXT_OPTION_CHANGED: {
            //Nothing to do
            return true; }
        case HILDON_IM_CONTEXT_CLEAR_STICKY: {
            mask &= ~(HILDON_IM_SHIFT_STICKY_MASK |
                      HILDON_IM_SHIFT_LOCK_MASK |
                      HILDON_IM_LEVEL_STICKY_MASK |
                      HILDON_IM_LEVEL_LOCK_MASK);
            return true; }

        //Unused 
        case HILDON_IM_CONTEXT_NUM_COM: {
            return true; }
        case HILDON_IM_CONTEXT_ENTER_ON_FOCUS: {
            return true; }
        case HILDON_IM_CONTEXT_WIDGET_CHANGED: {
            //Do we really need to set the mask=0? See reset();
            return true; }
        
#ifdef Q_OS_FREMANTLE
        case HILDON_IM_CONTEXT_CANCEL_PREEDIT: {
            #ifdef HIM_DEBUG
                qDebug() << "HILDON_IM_CONTEXT_CANCEL_PREEDIT";
            #endif
            preEditBuffer.clear();
            QInputMethodEvent e;            
            sendEvent(e);
            return true; }
        case HILDON_IM_CONTEXT_PREEDIT_MODE: {
            #ifdef HIM_DEBUG
                qDebug() << "commitMode = HILDON_IM_CONTEXT_PREEDIT_MODE";
            #endif
            preEditBuffer.clear();
            commitMode = HILDON_IM_COMMIT_PREEDIT;
            return true; } 
        case HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL: {
            #ifdef HIM_DEBUG
                qDebug() << "HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL";
            #endif
            checkCommitMode();
            sendSurrounding(true);
            //if (self->is_url_entry)
            //  hildon_im_context_send_command(self, HILDON_IM_SELECT_ALL);
            return true; }
#endif  
        default:
            qWarning() << "Hildon Input Method Message not handled" << msg->type;
        }
    }else if (event->xclient.message_type == ATOM(_HILDON_IM_SURROUNDING_CONTENT) &&
              event->xclient.format == HILDON_IM_SURROUNDING_CONTENT_FORMAT) {
        #ifdef HIM_DEBUG
                qDebug() << "HILDON_IM_SURROUNDING_CONTENT";
        #endif
        HildonIMSurroundingContentMessage *msg = reinterpret_cast<HildonIMSurroundingContentMessage*>(&event->xclient.data);

        if (!surrounding.isNull()) {
            if (msg->msg_flag == HILDON_IM_MSG_START) {
                surrounding.clear();
            }else if (msg->msg_flag == HILDON_IM_MSG_END) {
                //TODO? commitSurrounding();
                qWarning("commitSurrounding() NOT IMPLEMENTED YET");
                return true;
            }            
        }
        surrounding += QString::fromUtf8(msg->surrounding);
        return true;
    }else if (event->xclient.message_type == ATOM(_HILDON_IM_SURROUNDING) &&
                  event->xclient.format == HILDON_IM_SURROUNDING_FORMAT) {
        #ifdef HIM_DEBUG
                qDebug() << "HILDON_IM_SURROUNDING";
        #endif
        HildonIMSurroundingMessage *msg = reinterpret_cast<HildonIMSurroundingMessage*>(&event->xclient.data);
        setClientCursorLocation(msg->offset_is_relative, msg->cursor_offset );
        return true;
    }
    return false;
}

/*! Some widgets have the IM always enabled.
 *  In some particular cases it don't need the IM
 *  Eg:  If the widget is read only or if we have 
 *        complex widgets like QWebView or 
 *        QGraphicsView. 
 */
bool QHildonInputContext::canUseIM(QWidget *w)
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::canUseIM" << w;
#endif

    bool enabled = false;
    
    if ( w && w->testAttribute(Qt::WA_InputMethodEnabled) ){

        //is it in readOnlyMode?
        QLineEdit *le = qobject_cast<QLineEdit*>(w);
        QTextEdit *te = qobject_cast<QTextEdit*>(w);
        QPlainTextEdit *pte = qobject_cast<QPlainTextEdit*>(w);

        if (le){
            enabled=!le->isReadOnly();
        }else if (te){
            enabled=!te->isReadOnly();
        }else if (pte){
            enabled=!pte->isReadOnly();
        }else{
            //These queries return a valid values only for input elements
            //If the user click in an input element, the cursor return a valid
            //value and the selection will be an empty string.
            //Vice versa if the user select a text.
            enabled=  (w->inputMethodQuery(Qt::ImCursorPosition).isNull() !=
                       w->inputMethodQuery(Qt::ImCurrentSelection).toString().isEmpty());
        }
    }
    return enabled;
}

/*! Ask the client widget to insert the specified text at the cursor
 *  position, by triggering the commit signal on the context
 */
void QHildonInputContext::insertUtf8(int flag, const QString& text)
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::insertUtf8 flag=" << flag << "text=" << text;
#endif
    QString cleanText= text;
    lastInternalChange = true;

    //TODO HILDON_IM_AUTOCORRECT is used by the hadwriting plugin
    //Writing CiAo in the plugin add Ciao in the widget.
    if (options & HILDON_IM_AUTOCORRECT){
        qWarning() << "HILDON_IM_AUTOCORRECT Not Implemented Yet";
    }
    
    //Delete suroundings when we are using the preeditbuffer.
    // Eg: For the HandWriting plugin 
    if (!preEditBuffer.isNull()) {
#ifdef HIM_DEBUG
    qDebug() << "  preEditBuffer=" << preEditBuffer;
#endif        
        QInputMethodEvent e;
        int charCount = preEditBuffer.length(); 
        e.setCommitString(QString(), -charCount, charCount);
        sendEvent(e);
    } 

    //Updates preEditBuffer
    if (!preEditBuffer.isNull()){
        if (flag == HILDON_IM_MSG_START) {
            preEditBuffer = text;
        }else{
            preEditBuffer.append(text);
        }
        cleanText = preEditBuffer;
    }
    
    //Adds the actual text
    switch (commitMode) {
        case HILDON_IM_COMMIT_PREEDIT: { //Fremantle specific code
            //Fill preEditBuffer
            preEditBuffer = text;

            //Creating attribute list
            QList<QInputMethodEvent::Attribute> list;
            QInputMethodEvent::Attribute cursor(QInputMethodEvent::Cursor, 0, cleanText.length(), QColor(0, 0, 255, 127));
            list.append(cursor);

            QTextCharFormat textCharFormat;
            textCharFormat.setFontUnderline(true);
            textCharFormat.setBackground(focusWidget()->palette().highlight());
            textCharFormat.setForeground(focusWidget()->palette().highlightedText());
            QInputMethodEvent::Attribute textFormat(QInputMethodEvent::TextFormat, 0, cleanText.length(), textCharFormat);
            list.append(textFormat);

            QInputMethodEvent e(cleanText, list);            
            sendEvent(e);
        }break;
        case HILDON_IM_COMMIT_BUFFERED: //Diablo Handwriting
        case HILDON_IM_COMMIT_DIRECT:
        case HILDON_IM_COMMIT_REDIRECT:{
            QInputMethodEvent e;
            e.setCommitString(cleanText);
            sendEvent(e);
        }break;
        default:
            qWarning() << "Commit mode " << commitMode << " not handled by InsertText method";
    }
}

void QHildonInputContext::clearSelection()
{
#ifdef HIM_DEBUG
    qDebug() <<  "QHildonInputContext::clearSelection()";
#endif
    QWidget *w = focusWidget();
    int textCursorPos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    QString selection = w->inputMethodQuery(Qt::ImCurrentSelection).toString();

    if (selection.isEmpty())
        return;
    
    //Remove the selection
    QInputMethodEvent e;
    e.setCommitString(selection);
    sendEvent(e);

    //Move the cursor backward if the text has been selected from right to left
    if (textCursorPos < textCursorPosOnPress){
        QInputMethodEvent e;
        e.setCommitString(QString(), -selection.length(),0);
        sendEvent(e);
    }
}

void QHildonInputContext::sendHildonCommand(HildonIMCommand cmd, QWidget *widget)
{
#ifdef HIM_DEBUG
    qDebug() <<  "QHildonInputContext::sendHildonCommand cmd="<< cmd << "widget=" << widget;
#endif
    Window w = findHildonIm();

    if (!w){
        return;
    }

    XEvent ev;
    memset(&ev, 0, sizeof(XEvent));

    ev.xclient.type = ClientMessage;
    ev.xclient.window = w;
    ev.xclient.message_type = ATOM(_HILDON_IM_ACTIVATE);
    ev.xclient.format = HILDON_IM_ACTIVATE_FORMAT;

    HildonIMActivateMessage *msg = reinterpret_cast<HildonIMActivateMessage *>(&ev.xclient.data);

    if (widget){
        msg->input_window = widget->winId();
        msg->app_window = widget->window()->winId();
    }else if ( cmd != HILDON_IM_HIDE ){
        qWarning() << "Invalid Hildon Command:" << cmd;
        return;
    }

    if ( cmd == HILDON_IM_HIDE && timerId != -1){
        killTimer(timerId);
    }

    if (cmd == HILDON_IM_SETCLIENT || cmd == HILDON_IM_SETNSHOW){
        sendInputMode();
    }

    msg->cmd = cmd;
    //msg->input_mode = inputMode;
    msg->trigger = triggerMode;

    XSendEvent(X11->display, w, false, 0, &ev);
    XSync(X11->display, False);
}


/*!
 */
void QHildonInputContext::sendX11Event(XEvent *event)
{
#ifdef HIM_DEBUG
    qDebug() <<  "QHildonInputContext::sendX11Event" << event;
#endif
    Window w = findHildonIm();

    if (!w){
        return;
    }

    event->xclient.type = ClientMessage;
    event->xclient.window = w;

    XSendEvent(X11->display, w, false, 0, event);
    XSync(X11->display, False);
}

/*! Shows the Hildon Input Method Main UI
 */
void QHildonInputContext::showHIMMainUI() //### REMOVE
{
    //Force QInputContext to use the fucused widget.
#ifdef HIM_DEBUG
    qDebug() <<  "QHildonInputContext::showHIMMainUI()"
             <<  "QApplication::focusWidget=" << QApplication::focusWidget()
             <<  "focusWidget=" << focusWidget();
#endif
    //QInputContext::setFocusWidget(QApplication::focusWidget());
    toggleHildonMainIMUi();
}

//CONTEXT
/*! Updates the IM with the autocap state at the active cursor position
 */
void QHildonInputContext::checkSentenceStart()
{
#ifdef HIM_DEBUG
    qDebug() <<  "QHildonInputContext::contextCheckSentenceStart()";
#endif

    QWidget *w = focusWidget();
    if (!w){
        return;
    }

    if ((inputMode & (HILDON_GTK_INPUT_MODE_ALPHA | HILDON_GTK_INPUT_MODE_AUTOCAP)) !=
            (HILDON_GTK_INPUT_MODE_ALPHA | HILDON_GTK_INPUT_MODE_AUTOCAP)) {
        /* If autocap is off, but the mode contains alpha, send autocap message.
         * The important part is that when entering a numerical entry the autocap
         * is not defined, and the plugin sets the mode appropriate for the language */
        if (inputMode & HILDON_GTK_INPUT_MODE_ALPHA){
            autoUpper = false;
            sendHildonCommand(HILDON_IM_LOW,w);
        }
        return;
    }
    
    QString surrounding = w->inputMethodQuery(Qt::ImSurroundingText).toString();
    int cpos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();

    if (surrounding.isEmpty()) { 
        autoUpper = true;
        sendHildonCommand(HILDON_IM_UPP, w);
        return;
    }

    QRegExp r(".*[!?.][\\s]+");
    QRegExp c("[¿¡]");
    //Improve performance: Don't make sense analyzing more than N chars before the cursor
    QString left = surrounding.left(cpos).right(10);
    QString right = left.right(1);
    QString notRemoved = left.remove(r);

    if(!notRemoved.count() || right.contains(c)){
        autoUpper = options & HILDON_IM_AUTOCASE;
        sendHildonCommand(HILDON_IM_UPP, w);
    }else{
        autoUpper = false;
        sendHildonCommand(HILDON_IM_LOW, w);
    }
}

void QHildonInputContext::commitPreeditData()
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::commitPreeditData()";
#endif

    if (!preEditBuffer.isNull())
        preEditBuffer = QString("");
}

void QHildonInputContext::checkCommitMode() //### REMOVE?
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::checkCommitMode()" << commitMode;
#endif
    //if (m_commitMode == HILDON_IM_COMMIT_REDIRECT)
    //    m_commitMode = HILDON_IM_COMMIT_SURROUNDING;
        
}

/*! Send the text of the client widget surrounding the active cursor position,
 *  as well as the cursor position in the surrounding, to the IM
 */
void QHildonInputContext::sendSurrounding(bool sendAllContents)
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::sendSurrounding sendAllContents=" << sendAllContents 
             << "focusWidget=" << focusWidget();
#endif
    QWidget *w = focusWidget();

    if (!w)
        return;
    
    QString surrounding;
    int cpos;
    if (sendAllContents){

         //Try to detect the kind of widget
        QTextEdit *te = qobject_cast<QTextEdit*>(w);
        QPlainTextEdit *pte = qobject_cast<QPlainTextEdit*>(w);

        if (te){
            surrounding = te->text();
            cpos = te->textCursor().position();      
        }else if (pte){
            surrounding = pte->toPlainText();
            cpos = pte->textCursor().position(); 
        }else{
            surrounding = w->inputMethodQuery(Qt::ImSurroundingText).toString();
            cpos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
        }
    }else{
        surrounding = w->inputMethodQuery(Qt::ImSurroundingText).toString();
        cpos = w->inputMethodQuery(Qt::ImCursorPosition).toInt();
    }

    XEvent xev;
    HildonIMSurroundingContentMessage *surroundingContentMsg = 0;
    int flag = HILDON_IM_MSG_START;

    //Sending a null to clean the plugin.
    if (surrounding.isEmpty()) {
        memset(&xev, 0, sizeof(XEvent));
        xev.xclient.message_type = ATOM(_HILDON_IM_SURROUNDING_CONTENT);
        xev.xclient.format = HILDON_IM_SURROUNDING_CONTENT_FORMAT;

        surroundingContentMsg = reinterpret_cast<HildonIMSurroundingContentMessage*>(&xev.xclient.data);
        surroundingContentMsg->msg_flag = flag;
        surroundingContentMsg->surrounding[0] = '\0';

        sendX11Event(&xev);
        
        sendSurroundingHeader(0);
        return;
    }
    
    // Split surrounding context into pieces that are small enough
    // to send in a x message
    QByteArray ba = surrounding.toUtf8(); 
    const char *utf8 = ba.data();
    while (*utf8){
        const char *nextStart = getNextPacketStart(utf8);
        unsigned int len = nextStart - utf8;

        //this call will take care of adding the null terminator
        memset(&xev, 0, sizeof(XEvent));
        xev.xclient.message_type = ATOM(_HILDON_IM_SURROUNDING_CONTENT);
        xev.xclient.format = HILDON_IM_SURROUNDING_CONTENT_FORMAT;

        surroundingContentMsg = reinterpret_cast<HildonIMSurroundingContentMessage*>(&xev.xclient.data);
        surroundingContentMsg->msg_flag = flag;
        memcpy(surroundingContentMsg->surrounding, utf8, len);

        sendX11Event(&xev);

        utf8 = nextStart;
        flag = HILDON_IM_MSG_CONTINUE;
    } 
    sendSurroundingHeader(cpos);
}

void QHildonInputContext::sendSurroundingHeader(int offset)
{
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::sendSurroundingHeader offset=" << offset;
#endif
    XEvent xev;
    /* Send the cursor offset in the surrounding */
    memset(&xev, 0, sizeof(XEvent));
    xev.xclient.message_type = ATOM(_HILDON_IM_SURROUNDING);
    xev.xclient.format = HILDON_IM_SURROUNDING_FORMAT;

    HildonIMSurroundingMessage *surroundingMsg = reinterpret_cast<HildonIMSurroundingMessage *>(&xev.xclient.data);
    surroundingMsg->commit_mode = commitMode;
    surroundingMsg->cursor_offset = offset;

    sendX11Event(&xev);
}

/*! Notify IM of any input mode changes 
 */
void QHildonInputContext::inputModeChanged(){
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::inputModeChanged";
#endif

#if 0
  //TODO
  if ((input_mode & HILDON_GTK_INPUT_MODE_ALPHA) == 0  &&
      (input_mode & HILDON_GTK_INPUT_MODE_HEXA)  == 0  &&
      ( (input_mode & HILDON_GTK_INPUT_MODE_NUMERIC) != 0 ||
        (input_mode & HILDON_GTK_INPUT_MODE_TELE)    != 0))
  {
    self->mask = HILDON_IM_LEVEL_LOCK_MASK | HILDON_IM_LEVEL_STICKY_MASK;
  }
  else
  {
    self->mask &= ~HILDON_IM_LEVEL_LOCK_MASK;
    self->mask &= ~HILDON_IM_LEVEL_STICKY_MASK;
  }
#endif  
  /* Notify IM of any input mode changes in cases where the UI is
     already visible. */
  sendInputMode();
}

void QHildonInputContext::sendInputMode(){
#ifdef HIM_DEBUG
    qDebug() <<  "QHildonInputContext::sendInputMode";
#endif
    Window w = findHildonIm();

    if (!w){
        return;
    }

    XEvent ev;
    memset(&ev, 0, sizeof(XEvent));

    ev.xclient.type = ClientMessage;
    ev.xclient.window = w;
    ev.xclient.message_type = ATOM(_HILDON_IM_INPUT_MODE);
    ev.xclient.format = HILDON_IM_INPUT_MODE_FORMAT;

    HildonIMInputModeMessage *msg = reinterpret_cast<HildonIMInputModeMessage *>(&ev.xclient.data);
    HildonGtkInputMode input_mode = HILDON_GTK_INPUT_MODE_DICTIONARY;//inputMode;
    HildonGtkInputMode default_input_mode =  HILDON_GTK_INPUT_MODE_FULL;

    msg->input_mode = input_mode;
    msg->default_input_mode = default_input_mode;

    XSendEvent(X11->display, w, false, 0, &ev);
    XSync(X11->display, False);
}

/*! In redirect mode we use a proxy widget (fullscreen vkb). When the cursor position
 *  changes there, the HIM update the cursor position in the client (Qt application)
 */
void QHildonInputContext::setClientCursorLocation(int offsetIsRelative, int cursorOffset)
{   
#ifdef HIM_DEBUG
    qDebug() << "QHildonInputContext::setClientCursorLocation offsetIsRelative=" << offsetIsRelative 
             << "cursorOffset" << cursorOffset;
#endif
    if (!offsetIsRelative){
        qWarning("setClientCursorLocation can't manage absolute cursor Offsets");
        return;
    }

    //Move the cursor
    //NOTE: To move the cursor changes in customWidget::inputMethodEvent(QInputMethodEvent *e) are needed.
    QInputMethodEvent e;
    e.setCommitString(QString(), cursorOffset,0);
    sendEvent(e);
}
#endif
