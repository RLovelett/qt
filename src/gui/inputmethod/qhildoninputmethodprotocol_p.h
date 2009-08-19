/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** This file is part of the QtGui module of the Maemo Qt Toolkit.
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
#ifdef Q_WS_HILDON

#include "qx11info_x11.h"
#include "private/qt_x11_p.h"

/******* from hildon-im-protocol **********/

#define HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE (20 - sizeof(int))

/* Commit modes
   Determines how text is inserted into the client widget

   Buffered mode:  Each new commit replaces any previous commit to the
   client widget until FLUSH_PREEDIT is called.

   Direct mode (default): Each commit is immediately appended to the
   client widget at the cursor position.

   Redirect mode: Proxies input and cursor movement from one text widget
   into another (potentially off-screen) widget. Used when implementing
   fullscreen IM plugins for widgets that contain text formatting.

   Surrounding mode: Each commit replaces the current text surrounding
   the cursor position (see gtk_im_context_get_surrounding).
*/
typedef enum
{
  HILDON_IM_COMMIT_DIRECT,
  HILDON_IM_COMMIT_REDIRECT,
  HILDON_IM_COMMIT_SURROUNDING,
  HILDON_IM_COMMIT_BUFFERED,
  HILDON_IM_COMMIT_PREEDIT
} HildonIMCommitMode;

/* Type markers for IM messages that span several ClientMessages */
enum
{
  HILDON_IM_MSG_START,
  HILDON_IM_MSG_CONTINUE,
  HILDON_IM_MSG_END
};

/* Message carrying surrounding interpretation info, sent by both IM and context */
typedef struct
{
  HildonIMCommitMode commit_mode;
  int offset_is_relative;
  int cursor_offset;
} HildonIMSurroundingMessage;

/* The surrounding text, sent by both IM and context */
typedef struct
{
  int msg_flag;
  char surrounding[HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE];
} HildonIMSurroundingContentMessage;

enum
{
    HILDON_IM_ACTIVATE_FORMAT = 8,
//  HILDON_IM_COM_FORMAT =8,
    HILDON_IM_INSERT_UTF8_FORMAT = 8,
    HILDON_IM_KEY_EVENT_FORMAT = 8,
    HILDON_IM_SURROUNDING_CONTENT_FORMAT = 8,
    HILDON_IM_SURROUNDING_FORMAT = 8,
    HILDON_IM_INPUT_MODE_FORMAT = 8,
    HILDON_IM_PREEDIT_COMMITTED_FORMAT = 8,
    HILDON_IM_PREEDIT_COMMITTED_CONTENT_FORMAT = 8,
    HILDON_IM_CLIPBOARD_SELECTION_REPLY_FORMAT = 32,
//  HILDON_IM_CLIPBOARD_FORMAT = 32
    HILDON_IM_WINDOW_ID_FORMAT = 32,
    HILDON_IM_DEFAULT_LAUNCH_DELAY = 70
}; /* IM ClientMessage formats */

/* IM commands, from context to IM process */
enum HildonIMCommand
{
    HILDON_IM_MODE,       // Update the hildon-input-mode property
    HILDON_IM_SHOW,       // Show the IM UI
    HILDON_IM_HIDE,       // Hide the IM UI
    HILDON_IM_UPP,        // Uppercase autocap state at cursor
    HILDON_IM_LOW,        // Lowercase autocap state at cursor
    HILDON_IM_DESTROY,    // DEPRECATED
    HILDON_IM_CLEAR,      // Clear the IM UI state
    HILDON_IM_SETCLIENT,  // Set the client window
    HILDON_IM_SETNSHOW,   // Set the client and show the IM window 
    HILDON_IM_SELECT_ALL, // Select the text in the plugin

    HILDON_IM_SHIFT_LOCKED,
    HILDON_IM_SHIFT_UNLOCKED,
    HILDON_IM_MOD_LOCKED,
    HILDON_IM_MOD_UNLOCKED,

    /* always last */
    HILDON_IM_NUM_COMMANDS
};

enum HildonIMTrigger
{
    HILDON_IM_TRIGGER_NONE = -1,
    HILDON_IM_TRIGGER_STYLUS,
    HILDON_IM_TRIGGER_FINGER,
    HILDON_IM_TRIGGER_KEYBOARD,
    HILDON_IM_TRIGGER_UNKNOWN
};

// Command activation message, from context to IM (see HildonIMCommand)
struct HildonIMActivateMessage
{
    Window input_window;
    Window app_window;
    HildonIMCommand cmd;
    HildonIMTrigger trigger;
};

// Text insertion message, from IM to context
struct HildonIMInsertUtf8Message
{
    int msg_flag;
    char utf8_str[HILDON_IM_CLIENT_MESSAGE_BUFFER_SIZE];
};

// IM communications, from IM process to context
typedef enum
{
  HILDON_IM_CONTEXT_HANDLE_ENTER,           /* Virtual enter activated */
  HILDON_IM_CONTEXT_HANDLE_TAB,             /* Virtual tab activated */
  HILDON_IM_CONTEXT_HANDLE_BACKSPACE,       /* Virtual backspace activated */
  HILDON_IM_CONTEXT_HANDLE_SPACE,           /* Virtual space activated */
  HILDON_IM_CONTEXT_CONFIRM_SENTENCE_START, /* Query the autocap state at cursor */
  HILDON_IM_CONTEXT_FLUSH_PREEDIT,          /* Finalize the preedit to the client widget */
  HILDON_IM_CONTEXT_CANCEL_PREEDIT,          /* Clean the preedit buffer */

  /* See HildonIMCommitMode for a description of the commit modes */
  HILDON_IM_CONTEXT_BUFFERED_MODE,
  HILDON_IM_CONTEXT_DIRECT_MODE,
  HILDON_IM_CONTEXT_REDIRECT_MODE,
  HILDON_IM_CONTEXT_SURROUNDING_MODE,
  HILDON_IM_CONTEXT_PREEDIT_MODE,

  HILDON_IM_CONTEXT_CLIPBOARD_COPY,            /* Copy client selection to clipboard */
  HILDON_IM_CONTEXT_CLIPBOARD_CUT,             /* Cut client selection to clipboard */
  HILDON_IM_CONTEXT_CLIPBOARD_PASTE,           /* Paste clipboard selection to client */
  HILDON_IM_CONTEXT_CLIPBOARD_SELECTION_QUERY, /* Query if the client has an active selection */
  HILDON_IM_CONTEXT_REQUEST_SURROUNDING,       /* Request the content surrounding the cursor */
  HILDON_IM_CONTEXT_REQUEST_SURROUNDING_FULL,          /* Request the contents of the text widget */
  HILDON_IM_CONTEXT_WIDGET_CHANGED,            /* IM detected that the client widget changed */
  HILDON_IM_CONTEXT_OPTION_CHANGED,            /* The OptionMask for the active context is updated */
  HILDON_IM_CONTEXT_CLEAR_STICKY,              /* Clear the sticky key state */
  HILDON_IM_CONTEXT_ENTER_ON_FOCUS,            /* Generate a virtual enter key event on focus in */

  HILDON_IM_CONTEXT_SPACE_AFTER_COMMIT,
  HILDON_IM_CONTEXT_NO_SPACE_AFTER_COMMIT,

  /* always last */
  HILDON_IM_CONTEXT_NUM_COM
} HildonIMCommunication;

// IM context toggle options.
enum HildonIMOptionMask
{
  HILDON_IM_AUTOCASE          = 1 << 0, // Suggest case based on the cursor's position in sentence
  HILDON_IM_AUTOCORRECT       = 1 << 1, // Limited automatic error correction of commits
  HILDON_IM_AUTOLEVEL_NUMERIC = 1 << 2, // Default to appropriate key-level in numeric-only clients
  HILDON_IM_LOCK_LEVEL        = 1 << 3  // Lock the effective key-level at pre-determined value
};

// Communication message from IM to context
struct HildonIMComMessage
{
    Window input_window;
    HildonIMCommunication type;
    HildonIMOptionMask options;
};

// Key event message, from context to IM
typedef struct
{
  Window input_window;
  int type;
  unsigned int state;
  unsigned int keyval;
  unsigned int hardware_keycode;
} HildonIMKeyEventMessage;


typedef enum {
  HILDON_IM_SHIFT_STICKY_MASK     = 1 << 0,
  HILDON_IM_SHIFT_LOCK_MASK       = 1 << 1,
  HILDON_IM_LEVEL_STICKY_MASK     = 1 << 2,
  HILDON_IM_LEVEL_LOCK_MASK       = 1 << 3,
  HILDON_IM_COMPOSE_MASK          = 1 << 4,
  HILDON_IM_DEAD_KEY_MASK         = 1 << 5,
} HildonIMInternalModifierMask;


#endif
