/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/qwidget.h>
#include <QtCore/qlinkedlist.h>
#include <QtCore/qstack.h>

#ifdef QT_DEBUG
#include <QtCore/qfile.h>
#endif

#include "qgraphicsanchorlayout_p.h"

QT_BEGIN_NAMESPACE


QGraphicsAnchorPrivate::QGraphicsAnchorPrivate(int version)
    : QObjectPrivate(version), layoutPrivate(0), data(0)
{
}

QGraphicsAnchorPrivate::~QGraphicsAnchorPrivate()
{
    layoutPrivate->deleteAnchorData(data);
}

void QGraphicsAnchorPrivate::setSpacing(qreal value)
{
    if (data) {
        layoutPrivate->setAnchorSize(data, &value);
    } else {
        qWarning("QGraphicsAnchor::setSpacing: The anchor does not exist.");
    }
}

void QGraphicsAnchorPrivate::unsetSpacing()
{
    if (data) {
        layoutPrivate->setAnchorSize(data, 0);
    } else {
        qWarning("QGraphicsAnchor::setSpacing: The anchor does not exist.");
    }
}

qreal QGraphicsAnchorPrivate::spacing() const
{
    qreal size = 0;
    if (data) {
        layoutPrivate->anchorSize(data, 0, &size, 0);
    } else {
        qWarning("QGraphicsAnchor::setSpacing: The anchor does not exist.");
    }
    return size;
}


void AnchorData::refreshSizeHints(qreal effectiveSpacing)
{
    if (!isLayoutAnchor && from->m_item == to->m_item) {
        bool hasCenter = false;
        QGraphicsLayoutItem *item = from->m_item;

        if (QGraphicsAnchorLayoutPrivate::edgeOrientation(from->m_edge)
            == QGraphicsAnchorLayoutPrivate::Horizontal) {
            minSize = item->minimumWidth();
            prefSize = item->preferredWidth();
            maxSize = item->maximumWidth();
            hasCenter = (from->m_edge == Qt::AnchorHorizontalCenter
                         || to->m_edge == Qt::AnchorHorizontalCenter);
        } else {
            minSize = item->minimumHeight();
            prefSize = item->preferredHeight();
            maxSize = item->maximumHeight();
            hasCenter = (from->m_edge == Qt::AnchorVerticalCenter
                         || to->m_edge == Qt::AnchorVerticalCenter);
        }

        if (hasCenter) {
            minSize /= 2;
            prefSize /= 2;
            maxSize /= 2;
        }

        // Set the anchor effective sizes to preferred.
        //
        // Note: The idea here is that all items should remain at their
        // preferred size unless where that's impossible.  In cases where
        // the item is subject to restrictions (anchored to the layout
        // edges, for instance), the simplex solver will be run to
        // recalculate and override the values we set here.
        sizeAtMinimum = prefSize;
        sizeAtPreferred = prefSize;
        sizeAtMaximum = prefSize;

    } else if (!hasSize) {
        // Anchor has no size defined, use given default information
        minSize = effectiveSpacing;
        prefSize = effectiveSpacing;
        maxSize = effectiveSpacing;

        sizeAtMinimum = prefSize;
        sizeAtPreferred = prefSize;
        sizeAtMaximum = prefSize;
    }
}

void ParallelAnchorData::updateChildrenSizes()
{
    firstEdge->sizeAtMinimum = secondEdge->sizeAtMinimum = sizeAtMinimum;
    firstEdge->sizeAtPreferred = secondEdge->sizeAtPreferred = sizeAtPreferred;
    firstEdge->sizeAtMaximum = secondEdge->sizeAtMaximum = sizeAtMaximum;

    firstEdge->updateChildrenSizes();
    secondEdge->updateChildrenSizes();
}

void ParallelAnchorData::refreshSizeHints(qreal effectiveSpacing)
{
    // First refresh children information
    firstEdge->refreshSizeHints(effectiveSpacing);
    secondEdge->refreshSizeHints(effectiveSpacing);

    // ### should we warn if the parallel connection is invalid?
    // e.g. 1-2-3 with 10-20-30, the minimum of the latter is
    // bigger than the maximum of the former.

    minSize = qMax(firstEdge->minSize, secondEdge->minSize);
    maxSize = qMin(firstEdge->maxSize, secondEdge->maxSize);

    prefSize = qMax(firstEdge->prefSize, secondEdge->prefSize);
    prefSize = qMin(prefSize, maxSize);

    // See comment in AnchorData::refreshSizeHints() about sizeAt* values
    sizeAtMinimum = prefSize;
    sizeAtPreferred = prefSize;
    sizeAtMaximum = prefSize;
}

/*!
    \internal
    returns the factor in the interval [-1, 1].
    -1 is at Minimum
     0 is at Preferred
     1 is at Maximum
*/
static qreal getFactor(qreal value, qreal min, qreal pref, qreal max)
{
    // ### Maybe remove some of the assertions? (since outside is asserting us)
    Q_ASSERT(value > min || qFuzzyCompare(value, min));
    Q_ASSERT(value < max || qFuzzyCompare(value, max));

    if (qFuzzyCompare(value, min)) {
        return -1.0;
    } else if (qFuzzyCompare(value, pref)) {
        return 0.0;
    } else if (qFuzzyCompare(value, max)) {
        return 1.0;
    } else if (value < pref) {
        // Since value < pref and value != pref and min <= value,
        // we can assert that min < pref.
        Q_ASSERT(min < pref);
        return (value - min) / (pref - min) - 1;
    } else {
        // Since value > pref and value != pref and max >= value,
        // we can assert that max > pref.
        Q_ASSERT(max > pref);
        return (value - pref) / (max - pref);
    }
}

void SequentialAnchorData::updateChildrenSizes()
{
    // ### REMOVE ME
    // ### check whether we are guarantee to get those or we need to warn stuff at this
    // point.
    Q_ASSERT(sizeAtMinimum > minSize || qFuzzyCompare(sizeAtMinimum, minSize));
    Q_ASSERT(sizeAtMinimum < maxSize || qFuzzyCompare(sizeAtMinimum, maxSize));
    Q_ASSERT(sizeAtPreferred > minSize || qFuzzyCompare(sizeAtPreferred, minSize));
    Q_ASSERT(sizeAtPreferred < maxSize || qFuzzyCompare(sizeAtPreferred, maxSize));
    Q_ASSERT(sizeAtMaximum > minSize || qFuzzyCompare(sizeAtMaximum, minSize));
    Q_ASSERT(sizeAtMaximum < maxSize || qFuzzyCompare(sizeAtMaximum, maxSize));

    // Band here refers if the value is in the Minimum To Preferred
    // band (the lower band) or the Preferred To Maximum (the upper band).

    qreal minFactor = getFactor(sizeAtMinimum, minSize, prefSize, maxSize);
    qreal prefFactor = getFactor(sizeAtPreferred, minSize, prefSize, maxSize);
    qreal maxFactor = getFactor(sizeAtMaximum, minSize, prefSize, maxSize);

    for (int i = 0; i < m_edges.count(); ++i) {
        AnchorData *e = m_edges.at(i);

        qreal bandSize = minFactor > 0 ? e->maxSize - e->prefSize : e->prefSize - e->minSize;
        e->sizeAtMinimum = e->prefSize + bandSize * minFactor;

        bandSize = prefFactor > 0 ? e->maxSize - e->prefSize : e->prefSize - e->minSize;
        e->sizeAtPreferred = e->prefSize + bandSize * prefFactor;

        bandSize = maxFactor > 0 ? e->maxSize - e->prefSize : e->prefSize - e->minSize;
        e->sizeAtMaximum = e->prefSize + bandSize * maxFactor;

        e->updateChildrenSizes();
    }
}

void SequentialAnchorData::refreshSizeHints(qreal effectiveSpacing)
{
    minSize = 0;
    prefSize = 0;
    maxSize = 0;

    for (int i = 0; i < m_edges.count(); ++i) {
        AnchorData *edge = m_edges.at(i);

        // First refresh children information
        edge->refreshSizeHints(effectiveSpacing);

        minSize += edge->minSize;
        prefSize += edge->prefSize;
        maxSize += edge->maxSize;
    }

    // See comment in AnchorData::refreshSizeHints() about sizeAt* values
    sizeAtMinimum = prefSize;
    sizeAtPreferred = prefSize;
    sizeAtMaximum = prefSize;
}

#ifdef QT_DEBUG
void AnchorData::dump(int indent) {
    if (type == Parallel) {
        qDebug("%*s type: parallel:", indent, "");
        ParallelAnchorData *p = static_cast<ParallelAnchorData *>(this);
        p->firstEdge->dump(indent+2);
        p->secondEdge->dump(indent+2);
    } else if (type == Sequential) {
        SequentialAnchorData *s = static_cast<SequentialAnchorData *>(this);
        int kids = s->m_edges.count();
        qDebug("%*s type: sequential(%d):", indent, "", kids);
        for (int i = 0; i < kids; ++i) {
            s->m_edges.at(i)->dump(indent+2);
        }
    } else {
        qDebug("%*s type: Normal:", indent, "");
    }
}

#endif

QSimplexConstraint *GraphPath::constraint(const GraphPath &path) const
{
    // Calculate
    QSet<AnchorData *> cPositives;
    QSet<AnchorData *> cNegatives;
    QSet<AnchorData *> intersection;

    cPositives = positives + path.negatives;
    cNegatives = negatives + path.positives;

    intersection = cPositives & cNegatives;

    cPositives -= intersection;
    cNegatives -= intersection;

    // Fill
    QSimplexConstraint *c = new QSimplexConstraint;
    QSet<AnchorData *>::iterator i;
    for (i = cPositives.begin(); i != cPositives.end(); ++i)
        c->variables.insert(*i, 1.0);

    for (i = cNegatives.begin(); i != cNegatives.end(); ++i)
        c->variables.insert(*i, -1.0);

    return c;
}

#ifdef QT_DEBUG
QString GraphPath::toString() const
{
    QString string(QLatin1String("Path: "));
    foreach(AnchorData *edge, positives)
        string += QString::fromAscii(" (+++) %1").arg(edge->toString());

    foreach(AnchorData *edge, negatives)
        string += QString::fromAscii(" (---) %1").arg(edge->toString());

    return string;
}
#endif

QGraphicsAnchorLayoutPrivate::QGraphicsAnchorLayoutPrivate()
    : calculateGraphCacheDirty(1)
{
    for (int i = 0; i < NOrientations; ++i) {
        spacings[i] = -1;
        graphSimplified[i] = false;
    }
}

Qt::AnchorPoint QGraphicsAnchorLayoutPrivate::oppositeEdge(Qt::AnchorPoint edge)
{
    switch (edge) {
    case Qt::AnchorLeft:
        edge = Qt::AnchorRight;
        break;
    case Qt::AnchorRight:
        edge = Qt::AnchorLeft;
        break;
    case Qt::AnchorTop:
        edge = Qt::AnchorBottom;
        break;
    case Qt::AnchorBottom:
        edge = Qt::AnchorTop;
        break;
    default:
        break;
    }
    return edge;
}


/*!
 * \internal
 *
 * helper function in order to avoid overflowing anchor sizes
 * the returned size will never be larger than FLT_MAX
 *
 */
inline static qreal checkAdd(qreal a, qreal b)
{
    if (FLT_MAX - b  < a)
        return FLT_MAX;
    return a + b;
}

/*!
 * \internal
 *
 * Takes the sequence of vertices described by (\a before, \a vertices, \a after) and replaces
 * all anchors connected to the vertices in \a vertices with one simplified anchor between
 * \a before and \a after. The simplified anchor will be a placeholder for all the previous
 * anchors between \a before and \a after, and can be restored back to the anchors it is a
 * placeholder for.
 */
static bool simplifySequentialChunk(Graph<AnchorVertex, AnchorData> *graph,
                                    AnchorVertex *before,
                                    const QVector<AnchorVertex*> &vertices,
                                    AnchorVertex *after)
{
    int i;
#if defined(QT_DEBUG) && 0
    QString strVertices;
    for (i = 0; i < vertices.count(); ++i)
        strVertices += QString::fromAscii("%1 - ").arg(vertices.at(i)->toString());
    QString strPath = QString::fromAscii("%1 - %2%3").arg(before->toString(), strVertices, after->toString());
    qDebug("simplifying [%s] to [%s - %s]", qPrintable(strPath), qPrintable(before->toString()), qPrintable(after->toString()));
#endif

    qreal min = 0;
    qreal pref = 0;
    qreal max = 0;

    SequentialAnchorData *sequence = new SequentialAnchorData;
    AnchorVertex *prev = before;
    AnchorData *data;
    for (i = 0; i <= vertices.count(); ++i) {
        AnchorVertex *next = (i < vertices.count()) ? vertices.at(i) : after;
        data = graph->takeEdge(prev, next);
        min += data->minSize;
        pref += data->prefSize;
        max = checkAdd(max, data->maxSize);
        sequence->m_edges.append(data);
        prev = next;
    }

    // insert new
    sequence->minSize = min;
    sequence->prefSize = pref;
    sequence->maxSize = max;

    // Unless these values are overhidden by the simplex solver later-on,
    // anchors will keep their own preferred size.
    sequence->sizeAtMinimum = pref;
    sequence->sizeAtPreferred = pref;
    sequence->sizeAtMaximum = pref;

    sequence->setVertices(vertices);

    sequence->from = before;
    sequence->to = after;

    // data here is the last edge in the sequence
    // ### this seems to be here for supporting reverse order sequences,
    // but doesnt seem to be used right now
    if (data->from != vertices.last())
        qSwap(sequence->from, sequence->to);

    // Note that since layout 'edges' can't be simplified away from
    // the graph, it's safe to assume that if there's a layout
    // 'edge', it'll be in the boundaries of the sequence.
    sequence->isLayoutAnchor = (sequence->m_edges.first()->isLayoutAnchor
                                || sequence->m_edges.last()->isLayoutAnchor);

    AnchorData *newAnchor = sequence;
    if (AnchorData *oldAnchor = graph->takeEdge(before, after)) {
        newAnchor = new ParallelAnchorData(oldAnchor, sequence);

        newAnchor->isLayoutAnchor = (oldAnchor->isLayoutAnchor
                                     || sequence->isLayoutAnchor);

        min = qMax(oldAnchor->minSize, sequence->minSize);
        max = qMin(oldAnchor->maxSize, sequence->maxSize);

        pref = qMax(oldAnchor->prefSize, sequence->prefSize);
        pref = qMin(pref, max);

        newAnchor->minSize = min;
        newAnchor->prefSize = pref;
        newAnchor->maxSize = max;

        // Same as above, by default, keep preferred size.
        newAnchor->sizeAtMinimum = pref;
        newAnchor->sizeAtPreferred = pref;
        newAnchor->sizeAtMaximum = pref;
    }
    graph->createEdge(before, after, newAnchor);

    // True if we created a parallel anchor
    return newAnchor != sequence;
}

/*!
   \internal

   The purpose of this function is to simplify the graph.
   Simplification serves two purposes:
   1. Reduce the number of edges in the graph, (thus the number of variables to the equation
      solver is reduced, and the solver performs better).
   2. Be able to do distribution of sequences of edges more intelligently (esp. with sequential
      anchors)

   It is essential that it must be possible to restore simplified anchors back to their "original"
   form. This is done by restoreSimplifiedAnchor().

   There are two types of simplification that can be done:
   1. Sequential simplification
      Sequential simplification means that all sequences of anchors will be merged into one single
      anchor. Only anhcors that points in the same direction will be merged.
   2. Parallel simplification
      If a simplified sequential anchor is about to be inserted between two vertices in the graph
      and there already exist an anchor between those two vertices, a parallel anchor will be
      created that serves as a placeholder for the sequential anchor and the anchor that was
      already between the two vertices.

   The process of simplification can be described as:

   1. Simplify all sequences of anchors into one anchor.
      If no further simplification was done, go to (3)
      - If there already exist an anchor where the sequential anchor is supposed to be inserted,
        take that anchor out of the graph
      - Then create a parallel anchor that holds the sequential anchor and the anchor just taken
        out of the graph.
   2. Go to (1)
   3. Done


   * Gathering sequential anchors *
   The algorithm walks the graph in depth-first order, and only collects vertices that has two
   edges connected to it. If the vertex does not have two edges or if it is a layout edge,
   it will take all the previously collected vertices and try to create a simplified sequential
   anchor representing all the previously collected vertices.
   Once the simplified anchor is inserted, the collected list is cleared in order to find the next
   sequence to simplify.
   Note that there are some catches to this that are not covered by the above explanation.
*/
void QGraphicsAnchorLayoutPrivate::simplifyGraph(Orientation orientation)
{
    static bool noSimplification = !qgetenv("QT_ANCHORLAYOUT_NO_SIMPLIFICATION").isEmpty();
    if (noSimplification)
        return;

    if (graphSimplified[orientation])
        return;
    graphSimplified[orientation] = true;

#if 0
    qDebug("Simplifying Graph for %s",
           orientation == Horizontal ? "Horizontal" : "Vertical");
#endif

    AnchorVertex *rootVertex = graph[orientation].rootVertex();

    if (!rootVertex)
        return;

    bool dirty;
    do {
        dirty = simplifyGraphIteration(orientation);
    } while (dirty);
}

bool QGraphicsAnchorLayoutPrivate::simplifyGraphIteration(QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];
    AnchorVertex *v = g.rootVertex();

    QSet<AnchorVertex *> visited;
    QStack<AnchorVertex *> stack;
    stack.push(v);
    QVector<AnchorVertex*> candidates;

    const Qt::AnchorPoint centerEdge = pickEdge(Qt::AnchorHorizontalCenter, orientation);
    const Qt::AnchorPoint layoutEdge = oppositeEdge(v->m_edge);

    bool dirty = false;

    // walk depth-first.
    while (!stack.isEmpty()) {
        v = stack.pop();
        QList<AnchorVertex *> vertices = g.adjacentVertices(v);
        const int count = vertices.count();
        bool endOfSequence = (v->m_item == q && v->m_edge == layoutEdge) || count != 2;
        if (count == 2 && v->m_item != q) {
            candidates.append(v);
            if (visited.contains(vertices.first()) && visited.contains(vertices.last())) {
                // in case of a cycle
                endOfSequence = true;
            }
        }
        if (endOfSequence && candidates.count() >= 1) {
            int i;
            AnchorVertex *afterSequence= 0;
            AnchorVertex *beforeSequence = 0;
            // find the items before and after the valid sequence
            if (candidates.count() == 1) {
                QList<AnchorVertex *> beforeAndAfterVertices = g.adjacentVertices(candidates.at(0));
                Q_ASSERT(beforeAndAfterVertices.count() == 2);
                // Since we only have one vertex, we can pick
                // any of the two vertices to become before/after.
                afterSequence = beforeAndAfterVertices.last();
                beforeSequence = beforeAndAfterVertices.first();
            } else {
                QList<AnchorVertex *> adjacentOfSecondLastVertex = g.adjacentVertices(candidates.last());
                Q_ASSERT(adjacentOfSecondLastVertex.count() == 2);
                if (adjacentOfSecondLastVertex.first() == candidates.at(candidates.count() - 2))
                    afterSequence = adjacentOfSecondLastVertex.last();
                else
                    afterSequence = adjacentOfSecondLastVertex.first();

                QList<AnchorVertex *> adjacentOfSecondVertex = g.adjacentVertices(candidates.first());
                Q_ASSERT(adjacentOfSecondVertex.count() == 2);
                if (adjacentOfSecondVertex.first() == candidates.at(1))
                    beforeSequence = adjacentOfSecondVertex.last();
                else
                    beforeSequence = adjacentOfSecondVertex.first();
            }
            // The complete path of the sequence to simplify is: beforeSequence, <candidates>, afterSequence
            // where beforeSequence and afterSequence are the endpoints where the anchor is inserted
            // between.
#if defined(QT_DEBUG) && 0
            // ### DEBUG
            QString strCandidates;
            for (i = 0; i < candidates.count(); ++i)
                strCandidates += QString::fromAscii("%1 - ").arg(candidates.at(i)->toString());
            QString strPath = QString::fromAscii("%1 - %2%3").arg(beforeSequence->toString(), strCandidates, afterSequence->toString());
            qDebug("candidate list for sequential simplification:\n[%s]", qPrintable(strPath));
#endif

            bool forward = true;
            AnchorVertex *prev = beforeSequence;
            int intervalFrom = 0;

            // Check for directionality (from). We don't want to destroy that information,
            // thus we only combine anchors with the same direction.

            // "i" is the index *including* the beforeSequence and afterSequence vertices.
            for (i = 1; i <= candidates.count() + 1; ++i) {
                bool atVertexAfter = i > candidates.count();
                AnchorVertex *v1 = atVertexAfter ? afterSequence : candidates.at(i - 1);
                AnchorData *data = g.edgeData(prev, v1);
                Q_ASSERT(data);
                if (i == 1) {
                    forward = (prev == data->from ? true : false);
                } else if (forward != (prev == data->from) || atVertexAfter) {
                    int intervalTo = i;
                    if (forward != (prev == data->from))
                        --intervalTo;

                    // intervalFrom and intervalTo should now be indices to the vertex before and
                    // after the sequential anchor.
                    if (intervalTo - intervalFrom >= 2) {
                        // simplify in the range [intervalFrom, intervalTo]

                        // Trim off internal center anchors (Left-Center/Center-Right) from the
                        // start and the end of the sequence. We never want to simplify internal
                        // center anchors where there is an external anchor connected to the center.
                        AnchorVertex *intervalVertexFrom = intervalFrom == 0 ? beforeSequence : candidates.at(intervalFrom - 1);
                        int effectiveIntervalFrom = intervalFrom;
                        if (intervalVertexFrom->m_edge == centerEdge
                            && intervalVertexFrom->m_item == candidates.at(effectiveIntervalFrom)->m_item) {
                            ++effectiveIntervalFrom;
                            intervalVertexFrom = candidates.at(effectiveIntervalFrom - 1);
                        }
                        AnchorVertex *intervalVertexTo = intervalTo <= candidates.count() ? candidates.at(intervalTo - 1) : afterSequence;
                        int effectiveIntervalTo = intervalTo;
                        if (intervalVertexTo->m_edge == centerEdge
                            && intervalVertexTo->m_item == candidates.at(effectiveIntervalTo - 2)->m_item) {
                            --effectiveIntervalTo;
                            intervalVertexTo = candidates.at(effectiveIntervalTo - 1);
                        }
                        if (effectiveIntervalTo - effectiveIntervalFrom >= 2) {
                            QVector<AnchorVertex*> subCandidates;
                            if (forward) {
                               subCandidates = candidates.mid(effectiveIntervalFrom, effectiveIntervalTo - effectiveIntervalFrom - 1);
                            } else {
                                // reverse the order of the candidates.
                                qSwap(intervalVertexFrom, intervalVertexTo);
                                do {
                                    ++effectiveIntervalFrom;
                                    subCandidates.prepend(candidates.at(effectiveIntervalFrom - 1));
                                } while (effectiveIntervalFrom < effectiveIntervalTo - 1);
                            }
                            if (simplifySequentialChunk(&g, intervalVertexFrom, subCandidates, intervalVertexTo)) {
                                dirty = true;
                                break;
                            }
                            // finished simplification of chunk with same direction
                        }
                    }
                    if (forward == (prev == data->from))
                        --intervalTo;
                    intervalFrom = intervalTo;

                    forward = !forward;
                }
                prev = v1;
            }

            if (dirty)
                break;
        }

        if (endOfSequence)
            candidates.clear();

        for (int i = 0; i < count; ++i) {
            AnchorVertex *next = vertices.at(i);
            if (next->m_item == q && next->m_edge == centerEdge)
                continue;
            if (visited.contains(next))
                continue;
            stack.push(next);
        }

        visited.insert(v);
    }

    return dirty;
}

static void restoreSimplifiedAnchor(Graph<AnchorVertex, AnchorData> &g,
                                    AnchorData *edge,
                                    AnchorVertex *before,
                                    AnchorVertex *after)
{
    Q_ASSERT(edge->type != AnchorData::Normal);
#if 0
    static const char *anchortypes[] = {"Normal",
                                        "Sequential",
                                        "Parallel"};
    qDebug("Restoring %s edge.", anchortypes[int(edge->type)]);
#endif
    if (edge->type == AnchorData::Sequential) {
        SequentialAnchorData* seqEdge = static_cast<SequentialAnchorData*>(edge);
        // restore the sequential anchor
        AnchorVertex *prev = before;
        AnchorVertex *last = after;
        if (edge->from != prev)
            qSwap(last, prev);

        for (int i = 0; i < seqEdge->m_edges.count(); ++i) {
            AnchorVertex *v1 = (i < seqEdge->m_children.count()) ? seqEdge->m_children.at(i) : last;
            AnchorData *data = seqEdge->m_edges.at(i);
            if (data->type != AnchorData::Normal) {
                restoreSimplifiedAnchor(g, data, prev, v1);
            } else {
                g.createEdge(prev, v1, data);
            }
            prev = v1;
        }
    } else if (edge->type == AnchorData::Parallel) {
        ParallelAnchorData* parallelEdge = static_cast<ParallelAnchorData*>(edge);
        AnchorData *parallelEdges[2] = {parallelEdge->firstEdge,
                                        parallelEdge->secondEdge};
        for (int i = 0; i < 2; ++i) {
            AnchorData *data = parallelEdges[i];
            if (data->type == AnchorData::Normal) {
                g.createEdge(before, after, data);
            } else {
                restoreSimplifiedAnchor(g, data, before, after);
            }
        }
    }
}

void QGraphicsAnchorLayoutPrivate::restoreSimplifiedGraph(Orientation orientation)
{
    if (!graphSimplified[orientation])
        return;
    graphSimplified[orientation] = false;

#if 0
    qDebug("Restoring Simplified Graph for %s",
           orientation == Horizontal ? "Horizontal" : "Vertical");
#endif

    Graph<AnchorVertex, AnchorData> &g = graph[orientation];

    QList<QPair<AnchorVertex*, AnchorVertex*> > connections = g.connections();
    for (int i = 0; i < connections.count(); ++i) {
        AnchorVertex *v1 = connections.at(i).first;
        AnchorVertex *v2 = connections.at(i).second;
        AnchorData *edge = g.edgeData(v1, v2);
        if (edge->type != AnchorData::Normal) {
            AnchorData *oldEdge = g.takeEdge(v1, v2);
            restoreSimplifiedAnchor(g, edge, v1, v2);
            delete oldEdge;
        }
    }
}

QGraphicsAnchorLayoutPrivate::Orientation
QGraphicsAnchorLayoutPrivate::edgeOrientation(Qt::AnchorPoint edge)
{
    return edge > Qt::AnchorRight ? Vertical : Horizontal;
}

/*!
  \internal

  Create internal anchors to connect the layout edges (Left to Right and
  Top to Bottom).

  These anchors doesn't have size restrictions, that will be enforced by
  other anchors and items in the layout.
*/
void QGraphicsAnchorLayoutPrivate::createLayoutEdges()
{
    Q_Q(QGraphicsAnchorLayout);
    QGraphicsLayoutItem *layout = q;

    // Horizontal
    AnchorData *data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, Qt::AnchorLeft, layout,
              Qt::AnchorRight, data);
    data->skipInPreferred = 1;

    // Set the Layout Left edge as the root of the horizontal graph.
    AnchorVertex *v = internalVertex(layout, Qt::AnchorLeft);
    graph[Horizontal].setRootVertex(v);

    // Vertical
    data = new AnchorData(0, 0, QWIDGETSIZE_MAX);
    addAnchor(layout, Qt::AnchorTop, layout,
              Qt::AnchorBottom, data);
    data->skipInPreferred = 1;

    // Set the Layout Top edge as the root of the vertical graph.
    v = internalVertex(layout, Qt::AnchorTop);
    graph[Vertical].setRootVertex(v);
}

void QGraphicsAnchorLayoutPrivate::deleteLayoutEdges()
{
    Q_Q(QGraphicsAnchorLayout);

    Q_ASSERT(internalVertex(q, Qt::AnchorHorizontalCenter) == NULL);
    Q_ASSERT(internalVertex(q, Qt::AnchorVerticalCenter) == NULL);

    removeAnchor(q, Qt::AnchorLeft, q, Qt::AnchorRight);
    removeAnchor(q, Qt::AnchorTop, q, Qt::AnchorBottom);
}

void QGraphicsAnchorLayoutPrivate::createItemEdges(QGraphicsLayoutItem *item)
{
    Q_ASSERT(!graphSimplified[Horizontal] && !graphSimplified[Vertical]);

    items.append(item);

    // Horizontal
    int minimumSize = item->minimumWidth();
    int preferredSize = item->preferredWidth();
    int maximumSize = item->maximumWidth();

    AnchorData *data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, Qt::AnchorLeft, item,
              Qt::AnchorRight, data);

    // Vertical
    minimumSize = item->minimumHeight();
    preferredSize = item->preferredHeight();
    maximumSize = item->maximumHeight();

    data = new AnchorData(minimumSize, preferredSize, maximumSize);
    addAnchor(item, Qt::AnchorTop, item,
              Qt::AnchorBottom, data);
}

/*!
  \internal

  By default, each item in the layout is represented internally as
  a single anchor in each direction. For instance, from Left to Right.

  However, to support anchorage of items to the center of items, we
  must split this internal anchor into two half-anchors. From Left
  to Center and then from Center to Right, with the restriction that
  these anchors must have the same time at all times.
*/
void QGraphicsAnchorLayoutPrivate::createCenterAnchors(
    QGraphicsLayoutItem *item, Qt::AnchorPoint centerEdge)
{
    Orientation orientation;
    switch (centerEdge) {
    case Qt::AnchorHorizontalCenter:
        orientation = Horizontal;
        break;
    case Qt::AnchorVerticalCenter:
        orientation = Vertical;
        break;
    default:
        // Don't create center edges unless needed
        return;
    }

    Q_ASSERT(!graphSimplified[orientation]);

    // Check if vertex already exists
    if (internalVertex(item, centerEdge))
        return;

    // Orientation code
    Qt::AnchorPoint firstEdge;
    Qt::AnchorPoint lastEdge;

    if (orientation == Horizontal) {
        firstEdge = Qt::AnchorLeft;
        lastEdge = Qt::AnchorRight;
    } else {
        firstEdge = Qt::AnchorTop;
        lastEdge = Qt::AnchorBottom;
    }

    AnchorVertex *first = internalVertex(item, firstEdge);
    AnchorVertex *last = internalVertex(item, lastEdge);
    Q_ASSERT(first && last);

    // Create new anchors
    AnchorData *oldData = graph[orientation].edgeData(first, last);

    int minimumSize = oldData->minSize / 2;
    int preferredSize = oldData->prefSize / 2;
    int maximumSize = oldData->maxSize / 2;

    QSimplexConstraint *c = new QSimplexConstraint;
    AnchorData *data = new AnchorData(minimumSize, preferredSize, maximumSize);
    c->variables.insert(data, 1.0);
    addAnchor(item, firstEdge, item, centerEdge, data);

    data = new AnchorData(minimumSize, preferredSize, maximumSize);
    c->variables.insert(data, -1.0);
    addAnchor(item, centerEdge, item, lastEdge, data);

    itemCenterConstraints[orientation].append(c);

    // Remove old one
    removeAnchor(item, firstEdge, item, lastEdge);
}

void QGraphicsAnchorLayoutPrivate::removeCenterAnchors(
    QGraphicsLayoutItem *item, Qt::AnchorPoint centerEdge,
    bool substitute)
{
    Orientation orientation;
    switch (centerEdge) {
    case Qt::AnchorHorizontalCenter:
        orientation = Horizontal;
        break;
    case Qt::AnchorVerticalCenter:
        orientation = Vertical;
        break;
    default:
        // Don't remove edges that not the center ones
        return;
    }

    Q_ASSERT(!graphSimplified[orientation]);

    // Orientation code
    Qt::AnchorPoint firstEdge;
    Qt::AnchorPoint lastEdge;

    if (orientation == Horizontal) {
        firstEdge = Qt::AnchorLeft;
        lastEdge = Qt::AnchorRight;
    } else {
        firstEdge = Qt::AnchorTop;
        lastEdge = Qt::AnchorBottom;
    }

    AnchorVertex *center = internalVertex(item, centerEdge);
    if (!center)
        return;
    AnchorVertex *first = internalVertex(item, firstEdge);

    Q_ASSERT(first);
    Q_ASSERT(center);

    Graph<AnchorVertex, AnchorData> &g = graph[orientation];


    AnchorData *oldData = g.edgeData(first, center);
    // Remove center constraint
    for (int i = itemCenterConstraints[orientation].count() - 1; i >= 0; --i) {
        if (itemCenterConstraints[orientation][i]->variables.contains(oldData)) {
            delete itemCenterConstraints[orientation].takeAt(i);
            break;
        }
    }

    if (substitute) {
        // Create the new anchor that should substitute the left-center-right anchors.
        AnchorData *oldData = g.edgeData(first, center);

        int minimumSize = oldData->minSize * 2;
        int preferredSize = oldData->prefSize * 2;
        int maximumSize = oldData->maxSize * 2;

        AnchorData *data = new AnchorData(minimumSize, preferredSize, maximumSize);
        addAnchor(item, firstEdge, item, lastEdge, data);

        // Remove old anchors
        removeAnchor(item, firstEdge, item, centerEdge);
        removeAnchor(item, centerEdge, item, lastEdge);

    } else {
        // this is only called from removeAnchors()
        // first, remove all non-internal anchors
        QList<AnchorVertex*> adjacents = g.adjacentVertices(center);
        for (int i = 0; i < adjacents.count(); ++i) {
            AnchorVertex *v = adjacents.at(i);
            if (v->m_item != item) {
                removeAnchor(item, centerEdge, v->m_item, v->m_edge);
            }
        }
        // when all non-internal anchors is removed it will automatically merge the
        // center anchor into a left-right (or top-bottom) anchor. We must also delete that.
        // by this time, the center vertex is deleted and merged into a non-centered internal anchor
        removeAnchor(item, firstEdge, item, lastEdge);
    }
}


void QGraphicsAnchorLayoutPrivate::removeCenterConstraints(QGraphicsLayoutItem *item,
                                                           Orientation orientation)
{
    Q_ASSERT(!graphSimplified[orientation]);

    // Remove the item center constraints associated to this item
    // ### This is a temporary solution. We should probably use a better
    // data structure to hold items and/or their associated constraints
    // so that we can remove those easily

    AnchorVertex *first = internalVertex(item, orientation == Horizontal ?
                                       Qt::AnchorLeft :
                                       Qt::AnchorTop);
    AnchorVertex *center = internalVertex(item, orientation == Horizontal ?
                                        Qt::AnchorHorizontalCenter :
                                        Qt::AnchorVerticalCenter);

    // Skip if no center constraints exist
    if (!center)
        return;

    Q_ASSERT(first);
    AnchorData *internalAnchor = graph[orientation].edgeData(first, center);

    // Look for our anchor in all item center constraints, then remove it
    for (int i = 0; i < itemCenterConstraints[orientation].size(); ++i) {
        if (itemCenterConstraints[orientation][i]->variables.contains(internalAnchor)) {
            delete itemCenterConstraints[orientation].takeAt(i);
            break;
        }
    }
}

/*!
 * \internal
 *
 * Helper function that is called from the anchor functions in the public API.
 * If \a spacing is 0, it will pick up the spacing defined by the style.
 */
QGraphicsAnchor *QGraphicsAnchorLayoutPrivate::anchor(QGraphicsLayoutItem *firstItem,
                                                      Qt::AnchorPoint firstEdge,
                                                      QGraphicsLayoutItem *secondItem,
                                                      Qt::AnchorPoint secondEdge,
                                                      qreal *spacing)
{
    Q_Q(QGraphicsAnchorLayout);
    if ((firstItem == 0) || (secondItem == 0)) {
        qWarning("QGraphicsAnchorLayout::addAnchor(): "
                 "Cannot anchor NULL items");
        return 0;
    }

    if (firstItem == secondItem) {
        qWarning("QGraphicsAnchorLayout::addAnchor(): "
                 "Cannot anchor the item to itself");
        return 0;
    }

    if (edgeOrientation(secondEdge) != edgeOrientation(firstEdge)) {
        qWarning("QGraphicsAnchorLayout::addAnchor(): "
                 "Cannot anchor edges of different orientations");
        return 0;
    }

    // Guarantee that the graph is no simplified when adding this anchor,
    // anchor manipulation always happen in the full graph
    restoreSimplifiedGraph(edgeOrientation(firstEdge));

    // In QGraphicsAnchorLayout, items are represented in its internal
    // graph as four anchors that connect:
    //  - Left -> HCenter
    //  - HCenter-> Right
    //  - Top -> VCenter
    //  - VCenter -> Bottom

    // Ensure that the internal anchors have been created for both items.
    if (firstItem != q && !items.contains(firstItem)) {
        restoreSimplifiedGraph(edgeOrientation(firstEdge) == Horizontal ? Vertical : Horizontal);
        createItemEdges(firstItem);
        q->addLayoutItem(firstItem);
    }
    if (secondItem != q && !items.contains(secondItem)) {
        restoreSimplifiedGraph(edgeOrientation(firstEdge) == Horizontal ? Vertical : Horizontal);
        createItemEdges(secondItem);
        q->addLayoutItem(secondItem);
    }

    // Create center edges if needed
    createCenterAnchors(firstItem, firstEdge);
    createCenterAnchors(secondItem, secondEdge);

    // Use heuristics to find out what the user meant with this anchor.
    correctEdgeDirection(firstItem, firstEdge, secondItem, secondEdge);

    AnchorData *data;
    if (!spacing) {
        // If firstItem or secondItem is the layout itself, the spacing will default to 0.
        // Otherwise, the following matrix is used (questionmark means that the spacing
        // is queried from the style):
        //                from
        //  to      Left    HCenter Right
        //  Left    0       0       ?
        //  HCenter 0       0       0
        //  Right   ?       0       0
        if (firstItem != q
            && secondItem != q
            && pickEdge(firstEdge, Horizontal) != Qt::AnchorHorizontalCenter
            && oppositeEdge(firstEdge) == secondEdge) {
            data = new AnchorData;      // ask the style later
        } else {
            data = new AnchorData(0);   // spacing should be 0
        }
        addAnchor(firstItem, firstEdge, secondItem, secondEdge, data);
    } else if (*spacing >= 0) {
        data = new AnchorData(*spacing);
        addAnchor(firstItem, firstEdge, secondItem, secondEdge, data);
    } else {
        data = new AnchorData(-*spacing);
        addAnchor(secondItem, secondEdge, firstItem, firstEdge, data);
    }
    return acquireGraphicsAnchor(data);
}

void QGraphicsAnchorLayoutPrivate::addAnchor(QGraphicsLayoutItem *firstItem,
                                             Qt::AnchorPoint firstEdge,
                                             QGraphicsLayoutItem *secondItem,
                                             Qt::AnchorPoint secondEdge,
                                             AnchorData *data)
{
    Q_Q(QGraphicsAnchorLayout);

    // Guarantee that the graph is no simplified when adding this anchor,
    // anchor manipulation always happen in the full graph
    restoreSimplifiedGraph(edgeOrientation(firstEdge));

    // Is the Vertex (firstItem, firstEdge) already represented in our
    // internal structure?
    AnchorVertex *v1 = addInternalVertex(firstItem, firstEdge);
    AnchorVertex *v2 = addInternalVertex(secondItem, secondEdge);

    // Remove previous anchor
    // ### Could we update the existing edgeData rather than creating a new one?
    if (graph[edgeOrientation(firstEdge)].edgeData(v1, v2))
        removeAnchor(firstItem, firstEdge, secondItem, secondEdge);

    // Create a bi-directional edge in the sense it can be transversed both
    // from v1 or v2. "data" however is shared between the two references
    // so we still know that the anchor direction is from 1 to 2.
    data->from = v1;
    data->to = v2;
#ifdef QT_DEBUG
    data->name = QString::fromAscii("%1 --to--> %2").arg(v1->toString()).arg(v2->toString());
#endif
    // Keep track of anchors that are connected to the layout 'edges'
    data->isLayoutAnchor = (v1->m_item == q || v2->m_item == q);

    graph[edgeOrientation(firstEdge)].createEdge(v1, v2, data);
}

QGraphicsAnchor *QGraphicsAnchorLayoutPrivate::getAnchor(QGraphicsLayoutItem *firstItem,
                                                         Qt::AnchorPoint firstEdge,
                                                         QGraphicsLayoutItem *secondItem,
                                                         Qt::AnchorPoint secondEdge)
{
    Orientation orient = edgeOrientation(firstEdge);
    restoreSimplifiedGraph(orient);

    AnchorVertex *v1 = internalVertex(firstItem, firstEdge);
    AnchorVertex *v2 = internalVertex(secondItem, secondEdge);

    QGraphicsAnchor *graphicsAnchor = 0;

    AnchorData *data = graph[orient].edgeData(v1, v2);
    if (data)
        graphicsAnchor = acquireGraphicsAnchor(data);
    return graphicsAnchor;
}

void QGraphicsAnchorLayoutPrivate::removeAnchor(QGraphicsLayoutItem *firstItem,
                                                Qt::AnchorPoint firstEdge,
                                                QGraphicsLayoutItem *secondItem,
                                                Qt::AnchorPoint secondEdge)
{
    removeAnchor_helper(internalVertex(firstItem, firstEdge),
                        internalVertex(secondItem, secondEdge));
}

void QGraphicsAnchorLayoutPrivate::removeAnchor_helper(AnchorVertex *v1, AnchorVertex *v2)
{
    Q_ASSERT(v1 && v2);
    // Guarantee that the graph is no simplified when removing this anchor,
    // anchor manipulation always happen in the full graph
    Orientation o = edgeOrientation(v1->m_edge);
    restoreSimplifiedGraph(o);

    // Remove edge from graph
    graph[o].removeEdge(v1, v2);

    // Decrease vertices reference count (may trigger a deletion)
    removeInternalVertex(v1->m_item, v1->m_edge);
    removeInternalVertex(v2->m_item, v2->m_edge);
}

/*!
    \internal
    Only called from outside. (calls invalidate())
*/
void QGraphicsAnchorLayoutPrivate::deleteAnchorData(AnchorData *data)
{
    Q_Q(QGraphicsAnchorLayout);
    removeAnchor_helper(data->from, data->to);
    q->invalidate();
}

/*!
    \internal
    Only called from outside. (calls invalidate())
*/
void QGraphicsAnchorLayoutPrivate::setAnchorSize(AnchorData *data, const qreal *anchorSize)
{
    Q_Q(QGraphicsAnchorLayout);
    // ### we can avoid restoration if we really want to, but we would have to
    // search recursively through all composite anchors
    Q_ASSERT(data);
    restoreSimplifiedGraph(edgeOrientation(data->from->m_edge));
    if (anchorSize) {
        data->setFixedSize(*anchorSize);
    } else {
        data->unsetSize();
    }

    q->invalidate();
}

void QGraphicsAnchorLayoutPrivate::anchorSize(const AnchorData *data,
                                              qreal *minSize,
                                              qreal *prefSize,
                                              qreal *maxSize) const
{
    Q_ASSERT(minSize || prefSize || maxSize);
    Q_ASSERT(data);
    QGraphicsAnchorLayoutPrivate *that = const_cast<QGraphicsAnchorLayoutPrivate *>(this);
    that->restoreSimplifiedGraph(edgeOrientation(data->from->m_edge));

    if (minSize)
        *minSize = data->minSize;
    if (prefSize)
        *prefSize = data->prefSize;
    if (maxSize)
        *maxSize = data->maxSize;
}

AnchorVertex *QGraphicsAnchorLayoutPrivate::addInternalVertex(QGraphicsLayoutItem *item,
                                                              Qt::AnchorPoint edge)
{
    QPair<QGraphicsLayoutItem *, Qt::AnchorPoint> pair(item, edge);
    QPair<AnchorVertex *, int> v = m_vertexList.value(pair);

    if (!v.first) {
        Q_ASSERT(v.second == 0);
        v.first = new AnchorVertex(item, edge);
    }
    v.second++;
    m_vertexList.insert(pair, v);
    return v.first;
}

/**
 * \internal
 *
 * returns the AnchorVertex that was dereferenced, also when it was removed.
 * returns 0 if it did not exist.
 */
void QGraphicsAnchorLayoutPrivate::removeInternalVertex(QGraphicsLayoutItem *item,
                                                        Qt::AnchorPoint edge)
{
    QPair<QGraphicsLayoutItem *, Qt::AnchorPoint> pair(item, edge);
    QPair<AnchorVertex *, int> v = m_vertexList.value(pair);

    if (!v.first) {
        qWarning("This item with this edge is not in the graph");
        return;
    }

    v.second--;
    if (v.second == 0) {
        // Remove reference and delete vertex
        m_vertexList.remove(pair);
        delete v.first;
    } else {
        // Update reference count
        m_vertexList.insert(pair, v);

        if ((v.second == 2) &&
            ((edge == Qt::AnchorHorizontalCenter) ||
             (edge == Qt::AnchorVerticalCenter))) {
            removeCenterAnchors(item, edge, true);
        }
    }
}

void QGraphicsAnchorLayoutPrivate::removeVertex(QGraphicsLayoutItem *item, Qt::AnchorPoint edge)
{
    if (AnchorVertex *v = internalVertex(item, edge)) {
        Graph<AnchorVertex, AnchorData> &g = graph[edgeOrientation(edge)];
        const QList<AnchorVertex *> allVertices = graph[edgeOrientation(edge)].adjacentVertices(v);
        AnchorVertex *v2;
        foreach (v2, allVertices) {
            g.removeEdge(v, v2);
            removeInternalVertex(item, edge);
            removeInternalVertex(v2->m_item, v2->m_edge);
        }
    }
}

void QGraphicsAnchorLayoutPrivate::removeAnchors(QGraphicsLayoutItem *item)
{
    Q_ASSERT(!graphSimplified[Horizontal] && !graphSimplified[Vertical]);

    // remove the center anchor first!!
    removeCenterAnchors(item, Qt::AnchorHorizontalCenter, false);
    removeVertex(item, Qt::AnchorLeft);
    removeVertex(item, Qt::AnchorRight);

    removeCenterAnchors(item, Qt::AnchorVerticalCenter, false);
    removeVertex(item, Qt::AnchorTop);
    removeVertex(item, Qt::AnchorBottom);
}

/*!
  \internal

  Use heuristics to determine the correct orientation of a given anchor.

  After API discussions, we decided we would like expressions like
  anchor(A, Left, B, Right) to mean the same as anchor(B, Right, A, Left).
  The problem with this is that anchors could become ambiguous, for
  instance, what does the anchor A, B of size X mean?

     "pos(B) = pos(A) + X"  or  "pos(A) = pos(B) + X" ?

  To keep the API user friendly and at the same time, keep our algorithm
  deterministic, we use an heuristic to determine a direction for each
  added anchor and then keep it. The heuristic is based on the fact
  that people usually avoid overlapping items, therefore:

     "A, RIGHT to B, LEFT" means that B is to the LEFT of A.
     "B, LEFT to A, RIGHT" is corrected to the above anchor.

  Special correction is also applied when one of the items is the
  layout. We handle Layout Left as if it was another items's Right
  and Layout Right as another item's Left.
*/
void QGraphicsAnchorLayoutPrivate::correctEdgeDirection(QGraphicsLayoutItem *&firstItem,
                                                        Qt::AnchorPoint &firstEdge,
                                                        QGraphicsLayoutItem *&secondItem,
                                                        Qt::AnchorPoint &secondEdge)
{
    Q_Q(QGraphicsAnchorLayout);

    Qt::AnchorPoint effectiveFirst = firstEdge;
    Qt::AnchorPoint effectiveSecond = secondEdge;

    if (firstItem == q)
        effectiveFirst = QGraphicsAnchorLayoutPrivate::oppositeEdge(firstEdge);
    if (secondItem == q)
        effectiveSecond = QGraphicsAnchorLayoutPrivate::oppositeEdge(secondEdge);

    if (effectiveFirst < effectiveSecond) {

        // ### DEBUG
        /*        printf("Swapping Anchor from %s %d --to--> %s %d\n",
               firstItem->isLayout() ? "<layout>" :
               qPrintable(static_cast<QGraphicsWidget *>(firstItem)->data(0).toString()),
               firstEdge,
               secondItem->isLayout() ? "<layout>" :
               qPrintable(static_cast<QGraphicsWidget *>(secondItem)->data(0).toString()),
               secondEdge);
        */
        qSwap(firstItem, secondItem);
        qSwap(firstEdge, secondEdge);
    }
}

qreal QGraphicsAnchorLayoutPrivate::effectiveSpacing(Orientation orientation) const
{
    Q_Q(const QGraphicsAnchorLayout);
    qreal s = spacings[orientation];
    if (s < 0) {
        // ### make sure behaviour is the same as in QGraphicsGridLayout
        QGraphicsLayoutItem *parent = q->parentLayoutItem();
        while (parent && parent->isLayout()) {
            parent = parent->parentLayoutItem();
        }
        if (parent) {
            QGraphicsItem *parentItem = parent->graphicsItem();
            if (parentItem && parentItem->isWidget()) {
                QGraphicsWidget *w = static_cast<QGraphicsWidget*>(parentItem);
                s = w->style()->pixelMetric(orientation == Horizontal
                                            ? QStyle::PM_LayoutHorizontalSpacing
                                            : QStyle::PM_LayoutVerticalSpacing);
            }
        }
    }
    return s;
}

/*!
  \internal

  Called on activation. Uses Linear Programming to define minimum, preferred
  and maximum sizes for the layout. Also calculates the sizes that each item
  should assume when the layout is in one of such situations.
*/
void QGraphicsAnchorLayoutPrivate::calculateGraphs()
{
    if (!calculateGraphCacheDirty)
        return;

    calculateGraphs(Horizontal);
    calculateGraphs(Vertical);

    calculateGraphCacheDirty = 0;
}

// ### remove me:
QList<AnchorData *> getVariables(QList<QSimplexConstraint *> constraints)
{
    QSet<AnchorData *> variableSet;
    for (int i = 0; i < constraints.count(); ++i) {
        const QSimplexConstraint *c = constraints[i];
        foreach (QSimplexVariable *var, c->variables.keys()) {
            variableSet += static_cast<AnchorData *>(var);
        }
    }
    return variableSet.toList();
}

/*!
  \internal

  Calculate graphs is the method that puts together all the helper routines
  so that the AnchorLayout can calculate the sizes of each item.

  In a nutshell it should do:

  1) Update anchor nominal sizes, that is, the size that each anchor would
     have if no other restrictions applied. This is done by quering the
     layout style and the sizeHints of the items belonging to the layout.

  2) Simplify the graph by grouping together parallel and sequential anchors
     into "group anchors". These have equivalent minimum, preferred and maximum
     sizeHints as the anchors they replace.

  3) Check if we got to a trivial case. In some cases, the whole graph can be
     simplified into a single anchor. If so, use this information. If not,
     then call the Simplex solver to calculate the anchors sizes.

  4) Once the root anchors had its sizes calculated, propagate that to the
     anchors they represent.
*/
void QGraphicsAnchorLayoutPrivate::calculateGraphs(
    QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    // Simplify the graph
    simplifyGraph(orientation);

    // Reset the nominal sizes of each anchor based on the current item sizes
    setAnchorSizeHintsFromItems(orientation);

    // Traverse all graph edges and store the possible paths to each vertex
    findPaths(orientation);

    // From the paths calculated above, extract the constraints that the current
    // anchor setup impose, to our Linear Programming problem.
    constraintsFromPaths(orientation);

    // Split the constraints and anchors into groups that should be fed to the
    // simplex solver independently. Currently we find two groups:
    //
    //  1) The "trunk", that is, the set of anchors (items) that are connected
    //     to the two opposite sides of our layout, and thus need to stretch in
    //     order to fit in the current layout size.
    //
    //  2) The floating or semi-floating anchors (items) that are those which
    //     are connected to only one (or none) of the layout sides, thus are not
    //     influenced by the layout size.
    QList<QList<QSimplexConstraint *> > parts;
    parts = getGraphParts(orientation);

    // Now run the simplex solver to calculate Minimum, Preferred and Maximum sizes
    // of the "trunk" set of constraints and variables.
    // ### does trunk always exist? empty = trunk is the layout left->center->right
    QList<QSimplexConstraint *> trunkConstraints = parts[0];
    QList<QSimplexConstraint *> sizeHintConstraints;
    sizeHintConstraints = constraintsFromSizeHints(getVariables(trunkConstraints));
    trunkConstraints += sizeHintConstraints;

    // For minimum and maximum, use the path between the two layout sides as the
    // objective function.

    // Retrieve that path
    AnchorVertex *v = internalVertex(q, pickEdge(Qt::AnchorRight, orientation));
    GraphPath trunkPath = graphPaths[orientation].value(v);

    if (!trunkConstraints.isEmpty()) {
#if 0
        qDebug("Simplex used for trunk of %s",
               orientation == Horizontal ? "Horizontal" : "Vertical");
#endif

        // Solve min and max size hints for trunk
        QPair<qreal, qreal> minMax = solveMinMax(trunkConstraints, trunkPath);
        sizeHints[orientation][Qt::MinimumSize] = minMax.first;
        sizeHints[orientation][Qt::MaximumSize] = minMax.second;

        // Solve for preferred. The objective function is calculated from the constraints
        // and variables internally.
        solvePreferred(trunkConstraints);

        // Propagate the new sizes down the simplified graph, ie. tell the
        // group anchors to set their children anchors sizes.

        // ### we calculated variables already a few times, can't we reuse that?
        QList<AnchorData *> trunkVariables = getVariables(trunkConstraints);

        for (int i = 0; i < trunkVariables.count(); ++i)
            trunkVariables.at(i)->updateChildrenSizes();

        // Calculate and set the preferred size for the layout from the edge sizes that
        // were calculated above.
        qreal pref(0.0);
        foreach (const AnchorData *ad, trunkPath.positives) {
            pref += ad->sizeAtPreferred;
        }
        foreach (const AnchorData *ad, trunkPath.negatives) {
            pref -= ad->sizeAtPreferred;
        }
        sizeHints[orientation][Qt::PreferredSize] = pref;
    } else {
#if 0
        qDebug("Simplex NOT used for trunk of %s",
               orientation == Horizontal ? "Horizontal" : "Vertical");
#endif

        // No Simplex is necessary because the path was simplified all the way to a single
        // anchor.
        Q_ASSERT(trunkPath.positives.count() == 1);
        Q_ASSERT(trunkPath.negatives.count() == 0);

        AnchorData *ad = trunkPath.positives.toList()[0];
        ad->sizeAtMinimum = ad->minSize;
        ad->sizeAtPreferred = ad->prefSize;
        ad->sizeAtMaximum = ad->maxSize;

        // Propagate
        ad->updateChildrenSizes();

        sizeHints[orientation][Qt::MinimumSize] = ad->sizeAtMinimum;
        sizeHints[orientation][Qt::PreferredSize] = ad->sizeAtPreferred;
        sizeHints[orientation][Qt::MaximumSize] = ad->sizeAtMaximum;
    }

    // Delete the constraints, we won't use them anymore.
    qDeleteAll(sizeHintConstraints);
    sizeHintConstraints.clear();

    // For the other parts that not the trunk, solve only for the preferred size
    // that is the size they will remain at, since they are not stretched by the
    // layout.

    // Solve the other only for preferred, skip trunk
    for (int i = 1; i < parts.count(); ++i) {
        QList<QSimplexConstraint *> partConstraints = parts[i];
        QList<AnchorData *> partVariables = getVariables(partConstraints);
        Q_ASSERT(!partVariables.isEmpty());

        sizeHintConstraints = constraintsFromSizeHints(partVariables);
        partConstraints += sizeHintConstraints;
        solvePreferred(partConstraints);

        // Propagate size at preferred to other sizes. Semi-floats
        // always will be in their sizeAtPreferred.
        for (int j = 0; j < partVariables.count(); ++j) {
            AnchorData *ad = partVariables[j];
            Q_ASSERT(ad);
            ad->sizeAtMinimum = ad->sizeAtPreferred;
            ad->sizeAtMaximum = ad->sizeAtPreferred;
            ad->updateChildrenSizes();
        }

        // Delete the constraints, we won't use them anymore.
        qDeleteAll(sizeHintConstraints);
        sizeHintConstraints.clear();
    }

    // Clean up our data structures. They are not needed anymore since
    // distribution uses just interpolation.
    qDeleteAll(constraints[orientation]);
    constraints[orientation].clear();
    graphPaths[orientation].clear(); // ###
}

/*!
  \internal

  For graph edges ("anchors") that represent items, this method updates their
  intrinsic size restrictions, based on the item size hints.
*/
void QGraphicsAnchorLayoutPrivate::setAnchorSizeHintsFromItems(Orientation orientation)
{
    Graph<AnchorVertex, AnchorData> &g = graph[orientation];
    QList<QPair<AnchorVertex *, AnchorVertex *> > vertices = g.connections();

    qreal spacing = effectiveSpacing(orientation);

    for (int i = 0; i < vertices.count(); ++i) {
        AnchorData *data = g.edgeData(vertices.at(i).first, vertices.at(i).second);;
        Q_ASSERT(data->from && data->to);
        data->refreshSizeHints(spacing);
    }
}

/*!
  \internal

  This method walks the graph using a breadth-first search to find paths
  between the root vertex and each vertex on the graph. The edges
  directions in each path are considered and they are stored as a
  positive edge (left-to-right) or negative edge (right-to-left).

  The list of paths is used later to generate a list of constraints.
 */
void QGraphicsAnchorLayoutPrivate::findPaths(Orientation orientation)
{
    QQueue<QPair<AnchorVertex *, AnchorVertex *> > queue;

    QSet<AnchorData *> visited;

    AnchorVertex *root = graph[orientation].rootVertex();

    graphPaths[orientation].insert(root, GraphPath());

    foreach (AnchorVertex *v, graph[orientation].adjacentVertices(root)) {
        queue.enqueue(qMakePair(root, v));
    }

    while(!queue.isEmpty()) {
        QPair<AnchorVertex *, AnchorVertex *>  pair = queue.dequeue();
        AnchorData *edge = graph[orientation].edgeData(pair.first, pair.second);

        if (visited.contains(edge))
            continue;

        visited.insert(edge);
        GraphPath current = graphPaths[orientation].value(pair.first);

        if (edge->from == pair.first)
            current.positives.insert(edge);
        else
            current.negatives.insert(edge);

        graphPaths[orientation].insert(pair.second, current);

        foreach (AnchorVertex *v,
                graph[orientation].adjacentVertices(pair.second)) {
            queue.enqueue(qMakePair(pair.second, v));
        }
    }
}

/*!
  \internal

  Each vertex on the graph that has more than one path to it
  represents a contra int to the sizes of the items in these paths.

  This method walks the list of paths to each vertex, generate
  the constraints and store them in a list so they can be used later
  by the Simplex solver.
*/
void QGraphicsAnchorLayoutPrivate::constraintsFromPaths(Orientation orientation)
{
    foreach (AnchorVertex *vertex, graphPaths[orientation].uniqueKeys())
    {
        int valueCount = graphPaths[orientation].count(vertex);
        if (valueCount == 1)
            continue;

        QList<GraphPath> pathsToVertex = graphPaths[orientation].values(vertex);
        for (int i = 1; i < valueCount; ++i) {
            constraints[orientation] += \
                pathsToVertex[0].constraint(pathsToVertex[i]);
        }
    }
}

/*!
  \internal

  Create LP constraints for each anchor based on its minimum and maximum
  sizes, as specified in its size hints
*/
QList<QSimplexConstraint *> QGraphicsAnchorLayoutPrivate::constraintsFromSizeHints(
    const QList<AnchorData *> &anchors)
{
    QList<QSimplexConstraint *> anchorConstraints;
    for (int i = 0; i < anchors.size(); ++i) {
        QSimplexConstraint *c = new QSimplexConstraint;
        c->variables.insert(anchors[i], 1.0);
        c->constant = anchors[i]->minSize;
        c->ratio = QSimplexConstraint::MoreOrEqual;
        anchorConstraints += c;

        c = new QSimplexConstraint;
        c->variables.insert(anchors[i], 1.0);
        c->constant = anchors[i]->maxSize;
        c->ratio = QSimplexConstraint::LessOrEqual;
        anchorConstraints += c;
    }

    return anchorConstraints;
}

/*!
  \internal
*/
QList< QList<QSimplexConstraint *> >
QGraphicsAnchorLayoutPrivate::getGraphParts(Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);

    // Find layout vertices and edges for the current orientation.
    AnchorVertex *layoutFirstVertex = \
        internalVertex(q, pickEdge(Qt::AnchorLeft, orientation));

    AnchorVertex *layoutCentralVertex = \
        internalVertex(q, pickEdge(Qt::AnchorHorizontalCenter, orientation));

    AnchorVertex *layoutLastVertex = \
        internalVertex(q, pickEdge(Qt::AnchorRight, orientation));

    Q_ASSERT(layoutFirstVertex && layoutLastVertex);

    AnchorData *edgeL1 = NULL;
    AnchorData *edgeL2 = NULL;

    // The layout may have a single anchor between Left and Right or two half anchors
    // passing through the center
    if (layoutCentralVertex) {
        edgeL1 = graph[orientation].edgeData(layoutFirstVertex, layoutCentralVertex);
        edgeL2 = graph[orientation].edgeData(layoutCentralVertex, layoutLastVertex);
    } else {
        edgeL1 = graph[orientation].edgeData(layoutFirstVertex, layoutLastVertex);
    }

    QLinkedList<QSimplexConstraint *> remainingConstraints;
    for (int i = 0; i < constraints[orientation].count(); ++i) {
        remainingConstraints += constraints[orientation][i];
    }
    for (int i = 0; i < itemCenterConstraints[orientation].count(); ++i) {
        remainingConstraints += itemCenterConstraints[orientation][i];
    }

    QList<QSimplexConstraint *> trunkConstraints;
    QSet<QSimplexVariable *> trunkVariables;

    trunkVariables += edgeL1;
    if (edgeL2)
        trunkVariables += edgeL2;

    bool dirty;
    do {
        dirty = false;

        QLinkedList<QSimplexConstraint *>::iterator it = remainingConstraints.begin();
        while (it != remainingConstraints.end()) {
            QSimplexConstraint *c = *it;
            bool match = false;

            // Check if this constraint have some overlap with current
            // trunk variables...
            foreach (QSimplexVariable *ad, trunkVariables) {
                if (c->variables.contains(ad)) {
                    match = true;
                    break;
                }
            }

            // If so, we add it to trunk, and erase it from the
            // remaining constraints.
            if (match) {
                trunkConstraints += c;
                trunkVariables += QSet<QSimplexVariable *>::fromList(c->variables.keys());
                it = remainingConstraints.erase(it);
                dirty = true;
            } else {
                // Note that we don't erase the constraint if it's not
                // a match, since in a next iteration of a do-while we
                // can pass on it again and it will be a match.
                //
                // For example: if trunk share a variable with
                // remainingConstraints[1] and it shares with
                // remainingConstraints[0], we need a second iteration
                // of the do-while loop to match both.
                ++it;
            }
        }
    } while (dirty);

    QList< QList<QSimplexConstraint *> > result;
    result += trunkConstraints;

    if (!remainingConstraints.isEmpty()) {
        QList<QSimplexConstraint *> nonTrunkConstraints;
        QLinkedList<QSimplexConstraint *>::iterator it = remainingConstraints.begin();
        while (it != remainingConstraints.end()) {
            nonTrunkConstraints += *it;
            ++it;
        }
        result += nonTrunkConstraints;
    }

    return result;
}

/*!
  \internal

  Use the current vertices distance to calculate and set the geometry of
  each item.
*/
void QGraphicsAnchorLayoutPrivate::setItemsGeometries(const QRectF &geom)
{
    Q_Q(QGraphicsAnchorLayout);
    AnchorVertex *firstH, *secondH, *firstV, *secondV;

    qreal top;
    qreal left;
    qreal right;

    q->getContentsMargins(&left, &top, &right, 0);
    const Qt::LayoutDirection visualDir = visualDirection();
    if (visualDir == Qt::RightToLeft)
        qSwap(left, right);

    left += geom.left();
    top += geom.top();
    right = geom.right() - right;

    foreach (QGraphicsLayoutItem *item, items) {
        firstH = internalVertex(item, Qt::AnchorLeft);
        secondH = internalVertex(item, Qt::AnchorRight);
        firstV = internalVertex(item, Qt::AnchorTop);
        secondV = internalVertex(item, Qt::AnchorBottom);

        QRectF newGeom;
        newGeom.setTop(top + firstV->distance);
        newGeom.setBottom(top + secondV->distance);

        if (visualDir == Qt::LeftToRight) {
            newGeom.setLeft(left + firstH->distance);
            newGeom.setRight(left + secondH->distance);
        } else {
            newGeom.setLeft(right - secondH->distance);
            newGeom.setRight(right - firstH->distance);
        }
        item->setGeometry(newGeom);
    }
}

/*!
  \internal

  Calculate the position of each vertex based on the paths to each of
  them as well as the current edges sizes.
*/
void QGraphicsAnchorLayoutPrivate::calculateVertexPositions(
    QGraphicsAnchorLayoutPrivate::Orientation orientation)
{
    QQueue<QPair<AnchorVertex *, AnchorVertex *> > queue;
    QSet<AnchorVertex *> visited;

    // Get root vertex
    AnchorVertex *root = graph[orientation].rootVertex();

    root->distance = 0;
    visited.insert(root);

    // Add initial edges to the queue
    foreach (AnchorVertex *v, graph[orientation].adjacentVertices(root)) {
        queue.enqueue(qMakePair(root, v));
    }

    // Do initial calculation required by "interpolateEdge()"
    setupEdgesInterpolation(orientation);

    // Traverse the graph and calculate vertex positions, we need to
    // visit all pairs since each of them could have a sequential
    // anchor inside, which hides more vertices.
    while (!queue.isEmpty()) {
        QPair<AnchorVertex *, AnchorVertex *> pair = queue.dequeue();
        AnchorData *edge = graph[orientation].edgeData(pair.first, pair.second);

        // Both vertices were interpolated, and the anchor itself can't have other
        // anchors inside (it's not a complex anchor).
        if (edge->type == AnchorData::Normal && visited.contains(pair.second))
            continue;

        visited.insert(pair.second);
        interpolateEdge(pair.first, edge, orientation);

        QList<AnchorVertex *> adjacents = graph[orientation].adjacentVertices(pair.second);
        for (int i = 0; i < adjacents.count(); ++i) {
            if (!visited.contains(adjacents.at(i)))
                queue.enqueue(qMakePair(pair.second, adjacents.at(i)));
        }
    }
}

/*!
  \internal

  Calculate interpolation parameters based on current Layout Size.
  Must once before calling "interpolateEdgeSize()" for each edge.
*/
void QGraphicsAnchorLayoutPrivate::setupEdgesInterpolation(
    Orientation orientation)
{
    Q_Q(QGraphicsAnchorLayout);
    qreal lower, upper, current;

    if (orientation == Horizontal) {
        current = q->contentsRect().width();
    } else {
        current = q->contentsRect().height();
    }

    if (current < sizeHints[orientation][Qt::PreferredSize]) {
        interpolationInterval[orientation] = MinToPreferred;
        lower = sizeHints[orientation][Qt::MinimumSize];
        upper = sizeHints[orientation][Qt::PreferredSize];
    } else {
        interpolationInterval[orientation] = PreferredToMax;
        lower = sizeHints[orientation][Qt::PreferredSize];
        upper = sizeHints[orientation][Qt::MaximumSize];
    }

    if (upper == lower) {
        interpolationProgress[orientation] = 0;
    } else {
        interpolationProgress[orientation] = (current - lower) / (upper - lower);
    }
}

/*!
  \internal

  Calculate the current Edge size based on the current Layout size and the
  size the edge is supposed to have when:

   - the layout is at its minimum size.
   - the layout is at its preferred size.
   - the layout is at its maximum size.

   These three key values are calculated in advance using linear
   programming (more expensive) or the simplification algorithm, then
   subsequential resizes of the parent layout require a simple
   interpolation.

   If the edge is sequential or parallel, it's possible to have more
   vertices to be initalized, so it calls specialized functions that
   will recurse back to interpolateEdge().
 */
void QGraphicsAnchorLayoutPrivate::interpolateEdge(AnchorVertex *base,
                                                   AnchorData *edge,
                                                   Orientation orientation)
{
    qreal lower, upper;

    if (interpolationInterval[orientation] == MinToPreferred) {
        lower = edge->sizeAtMinimum;
        upper = edge->sizeAtPreferred;
    } else {
        lower = edge->sizeAtPreferred;
        upper = edge->sizeAtMaximum;
    }

    qreal edgeDistance = (interpolationProgress[orientation] * (upper - lower)) + lower;

    Q_ASSERT(edge->from == base || edge->to == base);

    if (edge->from == base)
        edge->to->distance = base->distance + edgeDistance;
    else
        edge->from->distance = base->distance - edgeDistance;

    // Process child anchors
    if (edge->type == AnchorData::Sequential)
        interpolateSequentialEdges(edge->from,
                                   static_cast<SequentialAnchorData *>(edge),
                                   orientation);
    else if (edge->type == AnchorData::Parallel)
        interpolateParallelEdges(edge->from,
                                 static_cast<ParallelAnchorData *>(edge),
                                 orientation);
}

void QGraphicsAnchorLayoutPrivate::interpolateParallelEdges(
    AnchorVertex *base, ParallelAnchorData *data, Orientation orientation)
{
    // In parallels the boundary vertices are already calculate, we
    // just need to look for sequential groups inside, because only
    // them may have new vertices associated.

    // First edge
    if (data->firstEdge->type == AnchorData::Sequential)
        interpolateSequentialEdges(base,
                                   static_cast<SequentialAnchorData *>(data->firstEdge),
                                   orientation);
    else if (data->firstEdge->type == AnchorData::Parallel)
        interpolateParallelEdges(base,
                                 static_cast<ParallelAnchorData *>(data->firstEdge),
                                 orientation);

    // Second edge
    if (data->secondEdge->type == AnchorData::Sequential)
        interpolateSequentialEdges(base,
                                   static_cast<SequentialAnchorData *>(data->secondEdge),
                                   orientation);
    else if (data->secondEdge->type == AnchorData::Parallel)
        interpolateParallelEdges(base,
                                 static_cast<ParallelAnchorData *>(data->secondEdge),
                                 orientation);
}

void QGraphicsAnchorLayoutPrivate::interpolateSequentialEdges(
    AnchorVertex *base, SequentialAnchorData *data, Orientation orientation)
{
    AnchorVertex *prev = base;

    // ### I'm not sure whether this assumption is safe. If not,
    // consider that m_edges.last() could be used instead (so
    // at(0) would be the one to be treated specially).
    Q_ASSERT(base == data->m_edges.at(0)->to || base == data->m_edges.at(0)->from);

    // Skip the last
    for (int i = 0; i < data->m_edges.count() - 1; ++i) {
        AnchorData *child = data->m_edges.at(i);
        interpolateEdge(prev, child, orientation);
        prev = child->to;
    }

    // Treat the last specially, since we already calculated it's end
    // vertex, so it's only interesting if it's a complex one
    if (data->m_edges.last()->type != AnchorData::Normal)
        interpolateEdge(prev, data->m_edges.last(), orientation);
}

QPair<qreal, qreal>
QGraphicsAnchorLayoutPrivate::solveMinMax(QList<QSimplexConstraint *> constraints,
                                          GraphPath path)
{
    QSimplex simplex;
    simplex.setConstraints(constraints);

    // Obtain the objective constraint
    QSimplexConstraint objective;
    QSet<AnchorData *>::const_iterator iter;
    for (iter = path.positives.constBegin(); iter != path.positives.constEnd(); ++iter)
        objective.variables.insert(*iter, 1.0);

    for (iter = path.negatives.constBegin(); iter != path.negatives.constEnd(); ++iter)
        objective.variables.insert(*iter, -1.0);

    simplex.setObjective(&objective);

    // Calculate minimum values
    qreal min = simplex.solveMin();

    // Save sizeAtMinimum results
    QList<QSimplexVariable *> variables = simplex.constraintsVariables();
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        ad->sizeAtMinimum = ad->result;
    }

    // Calculate maximum values
    qreal max = simplex.solveMax();

    // Save sizeAtMaximum results
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        ad->sizeAtMaximum = ad->result;
    }

    return qMakePair<qreal, qreal>(min, max);
}

void QGraphicsAnchorLayoutPrivate::solvePreferred(QList<QSimplexConstraint *> constraints)
{
    QList<AnchorData *> variables = getVariables(constraints);
    QList<QSimplexConstraint *> preferredConstraints;
    QList<QSimplexVariable *> preferredVariables;
    QSimplexConstraint objective;

    // Fill the objective coefficients for this variable. In the
    // end the objective function will be
    //
    //     z = n * (A_shrink + B_shrink + ...) + (A_grower + B_grower + ...)
    //
    // where n is the number of variables that have
    // slacks. Note that here we use the number of variables
    // as coefficient, this is to mark the "shrinker slack
    // variable" less likely to get value than the "grower
    // slack variable".

    // This will fill the values for the structural constraints
    // and we now fill the values for the slack constraints (one per variable),
    // which have this form (the constant A_pref was set when creating the slacks):
    //
    //      A + A_shrinker - A_grower = A_pref
    //
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        if (ad->skipInPreferred)
            continue;

        QSimplexVariable *grower = new QSimplexVariable;
        QSimplexVariable *shrinker = new QSimplexVariable;
        QSimplexConstraint *c = new QSimplexConstraint;
        c->variables.insert(ad, 1.0);
        c->variables.insert(shrinker, 1.0);
        c->variables.insert(grower, -1.0);
        c->constant = ad->prefSize;

        preferredConstraints += c;
        preferredVariables += grower;
        preferredVariables += shrinker;

        objective.variables.insert(grower, 1.0);
        objective.variables.insert(shrinker, variables.size());
    }


    QSimplex *simplex = new QSimplex;
    simplex->setConstraints(constraints + preferredConstraints);
    simplex->setObjective(&objective);

    // Calculate minimum values
    simplex->solveMin();

    // Save sizeAtPreferred results
    for (int i = 0; i < variables.size(); ++i) {
        AnchorData *ad = static_cast<AnchorData *>(variables[i]);
        ad->sizeAtPreferred = ad->result;
    }

    // Make sure we delete the simplex solver -before- we delete the
    // constraints used by it.
    delete simplex;

    // Delete constraints and variables we created.
    qDeleteAll(preferredConstraints);
    qDeleteAll(preferredVariables);
}

#ifdef QT_DEBUG
void QGraphicsAnchorLayoutPrivate::dumpGraph()
{
    QFile file(QString::fromAscii("anchorlayout.dot"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        qWarning("Could not write to %s", file.fileName().toLocal8Bit().constData());

    QString str = QString::fromAscii("digraph anchorlayout {\nnode [shape=\"rect\"]\n%1}");
    QString dotContents = graph[0].serializeToDot();
    dotContents += graph[1].serializeToDot();
    file.write(str.arg(dotContents).toLocal8Bit());

    file.close();
}
#endif

QT_END_NAMESPACE
