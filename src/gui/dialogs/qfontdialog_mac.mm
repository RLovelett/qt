/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfontdialog_p.h"
#if !defined(QT_NO_FONTDIALOG) && defined(Q_WS_MAC)
#include <qapplication.h>
#include <qdialogbuttonbox.h>
#include <qlineedit.h>
#include <private/qapplication_p.h>
#include <private/qfont_p.h>
#include <private/qfontengine_p.h>
#include <private/qt_mac_p.h>
#include <qdebug.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#if !CGFLOAT_DEFINED
typedef float CGFloat;  // Should only not be defined on 32-bit platforms
#endif

QT_USE_NAMESPACE

// should a priori be kept in sync with qcolordialog_mac.mm
const CGFloat ButtonMinWidth = 78.0;
const CGFloat ButtonMinHeight = 32.0;
const CGFloat ButtonSpacing = 0.0;
const CGFloat ButtonTopMargin = 0.0;
const CGFloat ButtonBottomMargin = 7.0;
const CGFloat ButtonSideMargin = 9.0;

// looks better with some margins
const CGFloat DialogTopMargin = 7.0;
const CGFloat DialogSideMargin = 9.0;

const int StyleMask = NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask;

@class QCocoaFontPanelDelegate;


#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_5

@protocol NSWindowDelegate <NSObject> @end

#endif

@interface QCocoaFontPanelDelegate : NSObject <NSWindowDelegate> {
    NSFontPanel *mFontPanel;
    NSView *mStolenContentView;
    NSButton *mOkButton;
    NSButton *mCancelButton;
    QFontDialogPrivate *mPriv;
    NSFont *mCocoaFont;
    QFont *mQtFont;
    BOOL mPanelHackedWithButtons;
    CGFloat mDialogExtraWidth;
    CGFloat mDialogExtraHeight;
    NSModalSession mModalSession;
}
- (id)initWithFontPanel:(NSFontPanel *)panel
      stolenContentView:(NSView *)stolenContentView
               okButton:(NSButton *)okButton
           cancelButton:(NSButton *)cancelButton
                   priv:(QFontDialogPrivate *)priv
             extraWidth:(CGFloat)extraWidth
            extraHeight:(CGFloat)extraHeight;
- (void)changeFont:(id)sender;
- (void)changeAttributes:(id)sender;
- (void)setModalSession:(NSModalSession)session;
- (BOOL)windowShouldClose:(id)window;
- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize;
- (void)relayout;
- (void)relayoutToContentSize:(NSSize)frameSize;
- (void)onOkClicked;
- (void)onCancelClicked;
- (NSFontPanel *)fontPanel;
- (NSWindow *)actualPanel;
- (NSSize)dialogExtraSize;
- (void)setQtFont:(const QFont &)newFont;
- (QFont)qtFont;
- (void)finishOffWithCode:(NSInteger)result;
- (void)cleanUpAfterMyself;
@end

@implementation QCocoaFontPanelDelegate
- (id)initWithFontPanel:(NSFontPanel *)panel
       stolenContentView:(NSView *)stolenContentView
                okButton:(NSButton *)okButton
            cancelButton:(NSButton *)cancelButton
                    priv:(QFontDialogPrivate *)priv
              extraWidth:(CGFloat)extraWidth
             extraHeight:(CGFloat)extraHeight
{
    self = [super init];
    mFontPanel = panel;
    mStolenContentView = stolenContentView;
    mOkButton = okButton;
    mCancelButton = cancelButton;
    mPriv = priv;
    mCocoaFont = 0;
    mPanelHackedWithButtons = (okButton != 0);
    mDialogExtraWidth = extraWidth;
    mDialogExtraHeight = extraHeight;
    mModalSession = 0;

    if (mPanelHackedWithButtons) {
        [self relayout];

        [okButton setAction:@selector(onOkClicked)];
        [okButton setTarget:self];

        [cancelButton setAction:@selector(onCancelClicked)];
        [cancelButton setTarget:self];
    }
    mQtFont = new QFont();
    return self;
}

- (void)dealloc
{
    if (mCocoaFont)
        [mCocoaFont release];
    delete mQtFont;
    [super dealloc];
}

- (void)changeFont:(id)sender
{
    Q_UNUSED(sender);

    QFont newFont;

    if (mCocoaFont)
        [mCocoaFont autorelease];
    NSFont *dummyFont = [NSFont userFontOfSize:12.0];
    mCocoaFont = [sender convertFont:dummyFont];
    if (mCocoaFont) {
        [mCocoaFont retain];

        int pSize = qRound([mCocoaFont pointSize]);
        QString family(QCFString::toQString(reinterpret_cast<CFStringRef>([mCocoaFont familyName])));
        QString typeface(QCFString::toQString(reinterpret_cast<CFStringRef>([mCocoaFont fontName])));
//        qDebug() << "original family" << family << "typeface" << typeface << "psize" << pSize;
        int hyphenPos = typeface.indexOf(QLatin1Char('-'));
        if (hyphenPos != -1) {
            typeface.remove(0, hyphenPos + 1);
        } else {
            typeface = QLatin1String("Normal");
        }
//        qDebug() << " massaged family" << family << "typeface" << typeface << "psize" << pSize;
        newFont = QFontDatabase().font(family, typeface, pSize);
        newFont.setUnderline(mQtFont->underline());
        newFont.setStrikeOut(mQtFont->strikeOut());
    }

    [self setQtFont:newFont];
    if (mPriv)
        mPriv->updateSampleFont(*mQtFont);
}

- (void)changeAttributes:(id)sender
{
    NSDictionary *dummyAttribs = [NSDictionary dictionary];
    NSDictionary *attribs = [sender convertAttributes:dummyAttribs];

#ifdef QT_MAC_USE_COCOA
    for (id key in attribs) {
#else
    NSEnumerator *enumerator = [attribs keyEnumerator];
    id key;
    while((key = [enumerator nextObject])) {
#endif
        NSNumber *number = static_cast<NSNumber *>([attribs objectForKey:key]);
        if ([key isEqual:NSUnderlineStyleAttributeName]) {
            mQtFont->setUnderline([number intValue] != NSUnderlineStyleNone);
        } else if ([key isEqual:NSStrikethroughStyleAttributeName]) {
            mQtFont->setStrikeOut([number intValue] != NSUnderlineStyleNone);
        }
    }

    if (mPriv)
        mPriv->updateSampleFont(*mQtFont);
}

- (void)setModalSession:(NSModalSession)session
{
    Q_ASSERT(!mModalSession);
    mModalSession = session;
}

- (BOOL)windowShouldClose:(id)window
{
    Q_UNUSED(window);
    if (mPanelHackedWithButtons) {
        [self onCancelClicked];
    } else {
        [self finishOffWithCode:NSCancelButton];
    }
    return true;
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
    if (mFontPanel == window) {
        proposedFrameSize = [static_cast<id <NSWindowDelegate> >(mFontPanel) windowWillResize:mFontPanel toSize:proposedFrameSize];
    } else {
        /*
            Ugly hack: NSFontPanel rearranges the layout of its main
            component in windowWillResize:toSize:. So we temporarily
            restore the stolen content view to its rightful owner,
            call windowWillResize:toSize:, and steal the content view
            again.
        */
        [mStolenContentView removeFromSuperview];
        [mFontPanel setContentView:mStolenContentView];
        NSSize extraSize = [self dialogExtraSize];
        proposedFrameSize.width -= extraSize.width;
        proposedFrameSize.height -= extraSize.height;
        proposedFrameSize = [static_cast<id <NSWindowDelegate> >(mFontPanel) windowWillResize:mFontPanel toSize:proposedFrameSize];
        NSRect frameRect = { { 0.0, 0.0 }, proposedFrameSize };
        [mFontPanel setFrame:frameRect display:NO];
        [mFontPanel setContentView:0];
        [[window contentView] addSubview:mStolenContentView];
        proposedFrameSize.width += extraSize.width;
        proposedFrameSize.height += extraSize.height;
    }
    if (mPanelHackedWithButtons) {
        NSRect frameRect = { { 0.0, 0.0 }, proposedFrameSize };
        NSRect contentRect = [NSWindow contentRectForFrameRect:frameRect styleMask:[window styleMask]];
        [self relayoutToContentSize:contentRect.size];
    }
    return proposedFrameSize;
}

- (void)relayout
{
    [self relayoutToContentSize:[[mStolenContentView superview] frame].size];
}

- (void)relayoutToContentSize:(NSSize)frameSize;
{
    Q_ASSERT(mPanelHackedWithButtons);

    [mOkButton sizeToFit];
    NSSize okSizeHint = [mOkButton frame].size;

    [mCancelButton sizeToFit];
    NSSize cancelSizeHint = [mCancelButton frame].size;

    const CGFloat ButtonWidth = qMin(qMax(ButtonMinWidth,
                                          qMax(okSizeHint.width, cancelSizeHint.width)),
                                     CGFloat((frameSize.width - 2.0 * ButtonSideMargin
                                              - ButtonSpacing) * 0.5));
    const CGFloat ButtonHeight = qMax(ButtonMinHeight,
                                     qMax(okSizeHint.height, cancelSizeHint.height));

    const CGFloat X = DialogSideMargin;
    const CGFloat Y = ButtonBottomMargin + ButtonHeight + ButtonTopMargin;

    NSRect okRect = { { frameSize.width - ButtonSideMargin - ButtonWidth,
                        ButtonBottomMargin },
                      { ButtonWidth, ButtonHeight } };
    [mOkButton setFrame:okRect];
    [mOkButton setNeedsDisplay:YES];

    NSRect cancelRect = { { okRect.origin.x - ButtonSpacing - ButtonWidth,
                            ButtonBottomMargin },
                            { ButtonWidth, ButtonHeight } };
    [mCancelButton setFrame:cancelRect];
    [mCancelButton setNeedsDisplay:YES];

    NSRect stolenCVRect = { { X, Y },
                            { frameSize.width - X - X, frameSize.height - Y - DialogTopMargin } };
    [mStolenContentView setFrame:stolenCVRect];
    [mStolenContentView setNeedsDisplay:YES];

    [[mStolenContentView superview] setNeedsDisplay:YES];
}

- (void)onOkClicked
{
    Q_ASSERT(mPanelHackedWithButtons);
    [[mStolenContentView window] close];
    [self finishOffWithCode:NSOKButton];
}

- (void)onCancelClicked
{
    Q_ASSERT(mPanelHackedWithButtons);
    [[mStolenContentView window] close];
    [self finishOffWithCode:NSCancelButton];
}

- (NSFontPanel *)fontPanel
{
    return mFontPanel;
}

- (NSWindow *)actualPanel
{
    return [mStolenContentView window];
}

- (NSSize)dialogExtraSize
{
    // this must be recomputed each time, because sometimes the
    // NSFontPanel has the NSDocModalWindowMask flag set, and sometimes
    // not -- which affects the frame rect vs. content rect measurements

    // take the different frame rectangles into account for dialogExtra{Width,Height}
    NSRect someRect = { { 0.0, 0.0 }, { 100000.0, 100000.0 } };
    NSRect sharedFontPanelContentRect = [mFontPanel contentRectForFrameRect:someRect];
    NSRect ourPanelContentRect = [NSWindow contentRectForFrameRect:someRect styleMask:StyleMask];

    NSSize result = { mDialogExtraWidth, mDialogExtraHeight };
    result.width -= ourPanelContentRect.size.width - sharedFontPanelContentRect.size.width;
    result.height -= ourPanelContentRect.size.height - sharedFontPanelContentRect.size.height;
    return result;
}

- (void)setQtFont:(const QFont &)newFont
{
    delete mQtFont;
    mQtFont = new QFont(newFont);
}

- (QFont)qtFont
{
    return *mQtFont;
}

- (void)finishOffWithCode:(NSInteger)code
{
    if (mPriv) {
        if (mModalSession) {
            [NSApp endModalSession:mModalSession];
            mModalSession = 0;
        }

        // temporary hack to work around bug in deleteLater() in Qt/Mac Cocoa
#if 1
        bool deleteDialog = mPriv->fontDialog()->testAttribute(Qt::WA_DeleteOnClose);
        mPriv->fontDialog()->setAttribute(Qt::WA_DeleteOnClose, false);
#endif
        mPriv->done((code == NSOKButton) ? QDialog::Accepted : QDialog::Rejected);
#if 1
        if (deleteDialog)
            delete mPriv->fontDialog();
#endif
    } else {
        [NSApp stopModalWithCode:code];
    }
}

- (void)cleanUpAfterMyself
{
    if (mPanelHackedWithButtons) {
        NSView *ourContentView = [mFontPanel contentView];

        // return stolen stuff to its rightful owner
        [mStolenContentView removeFromSuperview];
        [mFontPanel setContentView:mStolenContentView];

        [mOkButton release];
        [mCancelButton release];
        [ourContentView release];
    }
    [mFontPanel setDelegate:nil];
    [[NSFontManager sharedFontManager] setDelegate:nil];
    [[NSFontManager sharedFontManager] setTarget:nil];
}
@end

QT_BEGIN_NAMESPACE

extern void macStartInterceptNSPanelCtor();
extern void macStopInterceptNSPanelCtor();
extern NSButton *macCreateButton(const char *text, NSView *superview);

void *QFontDialogPrivate::openCocoaFontPanel(const QFont &initial,
        QWidget *parent, const QString &title, QFontDialog::FontDialogOptions options,
        QFontDialogPrivate *priv)
{
    Q_UNUSED(parent);   // we would use the parent if only NSFontPanel could be a sheet
    QMacCocoaAutoReleasePool pool;

    /*
        The standard Cocoa font panel has no OK or Cancel button and
        is created as a utility window. For strange reasons (which seem
        to stem from the fact that the font panel is based on a NIB
        file), the approach we use for the color panel doesn't work for
        the font panel (and, inversely, the approach we use here doesn't
        quite work for color panel, and crashed last time I tried). So
        instead, we take the following steps:

         1. Constructs a plain NSPanel that looks the way we want it
            to look. Specifically, if the NoButtons option is off, we
            construct a panel without the NSUtilityWindowMask flag
            and with buttons (OK and Cancel).

         2. Steal the content view from the shared NSFontPanel and
            put it inside our new NSPanel's content view, together
            with the OK and Cancel buttons.

         3. Lay out the original content view and the buttons when
            the font panel is shown and whenever it is resized.

         4. Clean up after ourselves.

         PS. Some customization is also done in QCocoaApplication
         validModesForFontPanel:.
    */

    Qt::WindowModality modality = Qt::ApplicationModal;
    if (priv)
        modality = priv->fontDialog()->windowModality();

    bool needButtons = !(options & QFontDialog::NoButtons);
    // don't need our own panel if the title bar isn't visible anyway (in a sheet)
    bool needOwnPanel = (needButtons && modality != Qt::WindowModal);

    bool sharedFontPanelExisted = [NSFontPanel sharedFontPanelExists];
    NSFontPanel *sharedFontPanel = [NSFontPanel sharedFontPanel];
    [sharedFontPanel setHidesOnDeactivate:false];

    // hack to ensure that QCocoaApplication's validModesForFontPanel:
    // implementation is honored
    if (!sharedFontPanelExisted && needOwnPanel) {
        [sharedFontPanel makeKeyAndOrderFront:sharedFontPanel];
        [sharedFontPanel close];
    }

    NSPanel *ourPanel = 0;
    NSView *stolenContentView = 0;
    NSButton *okButton = 0;
    NSButton *cancelButton = 0;

    CGFloat dialogExtraWidth = 0.0;
    CGFloat dialogExtraHeight = 0.0;

    if (!needOwnPanel) {
        // we can reuse the NSFontPanel unchanged
        ourPanel = sharedFontPanel;
    } else {
        // compute dialogExtra{Width,Height}
        dialogExtraWidth = 2.0 * DialogSideMargin;
        dialogExtraHeight = DialogTopMargin + ButtonTopMargin + ButtonMinHeight
                            + ButtonBottomMargin;

        // compute initial contents rectangle
        NSRect contentRect = [sharedFontPanel contentRectForFrameRect:[sharedFontPanel frame]];
        contentRect.size.width += dialogExtraWidth;
        contentRect.size.height += dialogExtraHeight;

        // create the new panel
        ourPanel = [[NSPanel alloc] initWithContentRect:contentRect
                                              styleMask:StyleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:YES];
        [ourPanel setReleasedWhenClosed:YES];
    }

    stolenContentView = [sharedFontPanel contentView];

    if (needButtons) {
        // steal the font panel's contents view
        [stolenContentView retain];
        [sharedFontPanel setContentView:0];

        // create a new content view and add the stolen one as a subview
        NSRect frameRect = { { 0.0, 0.0 }, { 0.0, 0.0 } };
        NSView *ourContentView = [[NSView alloc] initWithFrame:frameRect];
        [ourContentView addSubview:stolenContentView];

        // create OK and Cancel buttons and add these as subviews
        okButton = macCreateButton("&OK", ourContentView);
        cancelButton = macCreateButton("Cancel", ourContentView);

        [ourPanel setContentView:ourContentView];
        [ourPanel setDefaultButtonCell:[okButton cell]];
    }

    // create a delegate and set it
    QCocoaFontPanelDelegate *delegate =
            [[QCocoaFontPanelDelegate alloc] initWithFontPanel:sharedFontPanel
                                             stolenContentView:stolenContentView
                                                      okButton:okButton
                                                  cancelButton:cancelButton
                                                          priv:priv
                                                    extraWidth:dialogExtraWidth
                                                   extraHeight:dialogExtraHeight];
    [ourPanel setDelegate:delegate];
    [[NSFontManager sharedFontManager] setDelegate:delegate];
    [[NSFontManager sharedFontManager] setTarget:delegate];
    setFont(delegate, initial);

    // hack to get correct initial layout
    NSRect frameRect = [ourPanel frame];
    frameRect.size.width += 1.0;
    [ourPanel setFrame:frameRect display:NO];
    frameRect.size.width -= 1.0;
    frameRect.size = [delegate windowWillResize:ourPanel toSize:frameRect.size];
    [ourPanel setFrame:frameRect display:NO];
    [ourPanel center];

    [ourPanel setTitle:(NSString*)(CFStringRef)QCFString(title)];

    if (priv) {
        switch (modality) {
        case Qt::WindowModal:
            if (parent) {
#ifndef QT_MAC_USE_COCOA
                WindowRef hiwindowRef = qt_mac_window_for(parent);
                NSWindow *window =
                    [[NSWindow alloc] initWithWindowRef:hiwindowRef];
                // Cocoa docs say I should retain the Carbon ref.
                CFRetain(hiwindowRef);
#else
                NSWindow *window = qt_mac_window_for(parent);
#endif
                [NSApp beginSheet:ourPanel
                   modalForWindow:window
                    modalDelegate:0
                   didEndSelector:0
                      contextInfo:0];
#ifndef QT_MAC_USE_COCOA
                [window release];
#endif
                break;
            }
            // fallthrough
        case Qt::ApplicationModal:
            [delegate setModalSession:[NSApp beginModalSessionForWindow:ourPanel]];
            break;
        default:
            [ourPanel makeKeyAndOrderFront:ourPanel];
        }
    }

    return delegate;
}

void QFontDialogPrivate::closeCocoaFontPanel(void *delegate)
{
    QCocoaFontPanelDelegate *theDelegate = static_cast<QCocoaFontPanelDelegate *>(delegate);
    NSWindow *ourPanel = [theDelegate actualPanel];
    [ourPanel close];
    [theDelegate cleanUpAfterMyself];
    [theDelegate autorelease];
}

QFont QFontDialogPrivate::execCocoaFontPanel(bool *ok, const QFont &initial,
        QWidget *parent, const QString &title, QFontDialog::FontDialogOptions options)
{
    QMacCocoaAutoReleasePool pool;
    QCocoaFontPanelDelegate *delegate =
            static_cast<QCocoaFontPanelDelegate *>(
                openCocoaFontPanel(initial, parent, title, options));
    NSWindow *ourPanel = [delegate actualPanel];
    [ourPanel retain];
    int rval = [NSApp runModalForWindow:ourPanel];
    QFont font([delegate qtFont]);
    [ourPanel release];
    [delegate cleanUpAfterMyself];
    [delegate release];
    bool isOk = ((options & QFontDialog::NoButtons) || rval == NSOKButton);
    if (ok)
        *ok = isOk;
    if (isOk) {
        return font;
    } else {
        return initial;
    }
}

void QFontDialogPrivate::setFont(void * delegate, const QFont &font)
{
    QFontEngine *fe = font.d->engineForScript(QUnicodeTables::Common);
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    if (qstrcmp(fe->name(), "CoreText") == 0) {
        const NSFont *nsFont = reinterpret_cast<const NSFont *>(static_cast<QCoreTextFontEngineMulti *>(fe)->ctfont);
        [[NSFontManager sharedFontManager] setSelectedFont:nsFont isMultiple:NO];
    }
#endif
    [static_cast<QCocoaFontPanelDelegate *>(delegate) setQtFont:font];
}

QT_END_NAMESPACE

#endif
