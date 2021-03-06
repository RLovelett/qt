/*
 * This file is part of the HTML widget for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef RenderView_h
#define RenderView_h

#include "FrameView.h"
#include "Frame.h"
#include "LayoutState.h"
#include "RenderBlock.h"

namespace WebCore {

class RenderView : public RenderBlock {
public:
    RenderView(Node*, FrameView*);
    virtual ~RenderView();

    virtual const char* renderName() const { return "RenderView"; }

    virtual bool isRenderView() const { return true; }

    virtual void layout();
    virtual void calcWidth();
    virtual void calcHeight();
    virtual void calcPrefWidths();
    virtual FloatPoint localToAbsolute(FloatPoint localPoint = FloatPoint(), bool fixed = false, bool useTransforms = false) const;
    virtual FloatPoint absoluteToLocal(FloatPoint containerPoint, bool fixed = false, bool useTransforms = false) const;
    virtual FloatQuad localToAbsoluteQuad(const FloatQuad&, bool fixed = false) const;
    
    int docHeight() const;
    int docWidth() const;

    // The same as the FrameView's layoutHeight/layoutWidth but with null check guards.
    int viewHeight() const;
    int viewWidth() const;
    
    float zoomFactor() const { return m_frameView->frame() && m_frameView->frame()->shouldApplyPageZoom() ? m_frameView->frame()->zoomFactor() : 1.0f; }

    FrameView* frameView() const { return m_frameView; }

    virtual bool hasOverhangingFloats() { return false; }

    virtual void computeAbsoluteRepaintRect(IntRect&, bool fixed = false);
    virtual void repaintViewRectangle(const IntRect&, bool immediate = false);

    virtual void paint(PaintInfo&, int tx, int ty);
    virtual void paintBoxDecorations(PaintInfo&, int tx, int ty);

    void setSelection(RenderObject* start, int startPos, RenderObject* end, int endPos);
    void clearSelection();
    virtual RenderObject* selectionStart() const { return m_selectionStart; }
    virtual RenderObject* selectionEnd() const { return m_selectionEnd; }

    bool printing() const;
    void setPrintImages(bool enable) { m_printImages = enable; }
    bool printImages() const { return m_printImages; }
    void setTruncatedAt(int y) { m_truncatedAt = y; m_bestTruncatedAt = m_truncatorWidth = 0; m_forcedPageBreak = false; }
    void setBestTruncatedAt(int y, RenderObject *forRenderer, bool forcedBreak = false);
    int bestTruncatedAt() const { return m_bestTruncatedAt; }

    int truncatedAt() const { return m_truncatedAt; }

    virtual void absoluteRects(Vector<IntRect>&, int tx, int ty, bool topLevel = true);
    virtual void absoluteQuads(Vector<FloatQuad>&, bool topLevel = true);

    IntRect selectionBounds(bool clipToVisibleContent = true) const;

    void setMaximalOutlineSize(int o) { m_maximalOutlineSize = o; }
    int maximalOutlineSize() const { return m_maximalOutlineSize; }

    virtual IntRect viewRect() const;

    virtual void selectionStartEnd(int& startPos, int& endPos) const;

    IntRect printRect() const { return m_printRect; }
    void setPrintRect(const IntRect& r) { m_printRect = r; }

    void updateWidgetPositions();
    void addWidget(RenderObject*);
    void removeWidget(RenderObject*);

    // layoutDelta is used transiently during layout to store how far an object has moved from its
    // last layout location, in order to repaint correctly
    const IntSize& layoutDelta() const { return m_layoutDelta; }
    void addLayoutDelta(const IntSize& delta) { m_layoutDelta += delta; }

    void pushLayoutState(RenderBox* renderer, const IntSize& offset)
    {
        if (m_layoutStateDisableCount || m_frameView->needsFullRepaint())
            return;
        m_layoutState = new (renderArena()) LayoutState(m_layoutState, renderer, offset);
    }

    void pushLayoutState(RenderObject*);

    void popLayoutState()
    {
        if (m_layoutStateDisableCount || m_frameView->needsFullRepaint())
            return;
        LayoutState* state = m_layoutState;
        m_layoutState = state->m_next;
        state->destroy(renderArena());
    }

    LayoutState* layoutState() const { return m_layoutStateDisableCount ? 0 : m_layoutState; }

    // Suspends the LayoutState optimization. Used under transforms that cannot be represented by
    // LayoutState (common in SVG) and when manipulating the render tree during layout in ways
    // that can trigger repaint of a non-child (e.g. when a list item moves its list marker around).
    void disableLayoutState() { m_layoutStateDisableCount++; }
    void enableLayoutState() { ASSERT(m_layoutStateDisableCount > 0); m_layoutStateDisableCount--; }

private:
    // selectionRect should never be called on a RenderView
    virtual IntRect selectionRect(bool);

protected:
    FrameView* m_frameView;

    RenderObject* m_selectionStart;
    RenderObject* m_selectionEnd;
    int m_selectionStartPos;
    int m_selectionEndPos;

    // used to ignore viewport width when printing to the printer
    bool m_printImages;
    int m_truncatedAt;

    int m_maximalOutlineSize; // Used to apply a fudge factor to dirty-rect checks on blocks/tables.
    IntRect m_printRect; // Used when printing.

    typedef HashSet<RenderObject*> RenderObjectSet;

    RenderObjectSet m_widgets;

private:
    int m_bestTruncatedAt;
    int m_truncatorWidth;
    bool m_forcedPageBreak;
    IntSize m_layoutDelta;
    LayoutState* m_layoutState;
    unsigned m_layoutStateDisableCount;
};

// Stack-based class to assist with LayoutState push/pop
class LayoutStateMaintainer : Noncopyable {
public:
    // ctor to push now
    LayoutStateMaintainer(RenderView* view, RenderBox* root, IntSize offset, bool shouldPush = true)
        : m_view(view)
        , m_shouldPushPop(shouldPush)
        , m_didStart(false)
        , m_didEnd(false)
    {
        push(root, offset);
    }
    
    // ctor to maybe push later
    LayoutStateMaintainer(RenderView* view)
        : m_view(view)
        , m_shouldPushPop(true)
        , m_didStart(false)
        , m_didEnd(false)
    {
    }
    
    ~LayoutStateMaintainer()
    {
        ASSERT(m_didStart == m_didEnd);   // if this fires, it means that someone did a push(), but forgot to pop().
    }

    void pop()
    {
        if (m_didStart) {
            ASSERT(!m_didEnd);
            if (m_shouldPushPop)
                m_view->popLayoutState();
            else
                m_view->enableLayoutState();
            m_didEnd = true;
        }
    }

    void push(RenderBox* root, IntSize offset)
    {
        ASSERT(!m_didStart);
        if (m_shouldPushPop)
            m_view->pushLayoutState(root, offset);
        else
            m_view->disableLayoutState();
        m_didStart = true;
    }
    
    bool didPush() const { return m_didStart; }

private:
    RenderView* m_view;
    bool m_shouldPushPop : 1;   // true if we should push/pop, rather than disable/enable
    bool m_didStart : 1;        // true if we did a push or disable
    bool m_didEnd : 1;          // true if we popped or re-enabled
};

} // namespace WebCore

#endif // RenderView_h
