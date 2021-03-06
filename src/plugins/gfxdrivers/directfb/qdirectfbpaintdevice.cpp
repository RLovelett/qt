/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QT_NO_DIRECTFB

#include "qdirectfbscreen.h"
#include "qdirectfbpaintdevice.h"
#include "qdirectfbpaintengine.h"

QDirectFBPaintDevice::QDirectFBPaintDevice(QDirectFBScreen *scr)
    : QCustomRasterPaintDevice(0), dfbSurface(0), lockedImage(0), screen(scr),
      lock(DFBSurfaceLockFlags(0)), mem(0), engine(0)
{}

QDirectFBPaintDevice::~QDirectFBPaintDevice()
{
    delete lockedImage;
    delete engine;
}


IDirectFBSurface *QDirectFBPaintDevice::directFBSurface() const
{
    return dfbSurface;
}


void QDirectFBPaintDevice::lockDirectFB(DFBSurfaceLockFlags flags)
{
    if (!(lock & flags)) {
        if (lock)
            unlockDirectFB();
        mem = QDirectFBScreen::lockSurface(dfbSurface, flags, &bpl);
        Q_ASSERT(mem);
        const QSize s = size();
        lockedImage = new QImage(mem, s.width(), s.height(), bpl,
                                 QDirectFBScreen::getImageFormat(dfbSurface));
        lock = flags;
    }
}


void QDirectFBPaintDevice::unlockDirectFB()
{
    if (!lockedImage || !QDirectFBScreen::instance())
        return;

    dfbSurface->Unlock(dfbSurface);
    delete lockedImage;
    lockedImage = 0;
    mem = 0;
    lock = DFBSurfaceLockFlags(0);
}


void *QDirectFBPaintDevice::memory() const
{
    return mem;
}


QImage::Format QDirectFBPaintDevice::format() const
{
    return QDirectFBScreen::getImageFormat(dfbSurface);
}


int QDirectFBPaintDevice::bytesPerLine() const
{
    if (bpl == -1) {
        // Can only get the stride when we lock the surface
        Q_ASSERT(!lockedImage);
        QDirectFBPaintDevice* that = const_cast<QDirectFBPaintDevice*>(this);
        that->lockDirectFB(DSLF_READ);
        Q_ASSERT(bpl != -1);
    }
    return bpl;
}


QSize QDirectFBPaintDevice::size() const
{
    int w, h;
    dfbSurface->GetSize(dfbSurface, &w, &h);
    return QSize(w, h);
}

int QDirectFBPaintDevice::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    if (!dfbSurface)
        return 0;

    int w, h;
    dfbSurface->GetSize(dfbSurface, &w, &h);

    switch (metric) {
    case QPaintDevice::PdmWidth:
        return w;
    case QPaintDevice::PdmHeight:
        return h;
    case QPaintDevice::PdmWidthMM:
        return (w * 1000) / dotsPerMeterX();
    case QPaintDevice::PdmHeightMM:
        return (h * 1000) / dotsPerMeterY();
    case QPaintDevice::PdmPhysicalDpiX:
    case QPaintDevice::PdmDpiX:
        return (dotsPerMeterX() * 254) / 10000; // 0.0254 meters-per-inch
    case QPaintDevice::PdmPhysicalDpiY:
    case QPaintDevice::PdmDpiY:
        return (dotsPerMeterY() * 254) / 10000; // 0.0254 meters-per-inch
    case QPaintDevice::PdmDepth:
        DFBSurfacePixelFormat format;
        dfbSurface->GetPixelFormat(dfbSurface, &format);
        return QDirectFBScreen::depth(format);
    case QPaintDevice::PdmNumColors: {
       if (lockedImage)
            return lockedImage->numColors();

        DFBResult result;
        IDirectFBPalette *palette = 0;
        unsigned int numColors = 0;

        result = dfbSurface->GetPalette(dfbSurface, &palette);
        if ((result != DFB_OK) || !palette)
            return 0;

        result = palette->GetSize(palette, &numColors);
        palette->Release(palette);
        if (result != DFB_OK)
            return 0;

        return numColors;
    }
    default:
        qCritical("QDirectFBPaintDevice::metric(): Unhandled metric!");
        return 0;
    }
}

QPaintEngine *QDirectFBPaintDevice::paintEngine() const
{
    return engine;
}

#endif

