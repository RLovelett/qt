/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QWSGLPAINTDEVICE_GL_P_H
#define QWSGLPAINTDEVICE_GL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QGLWindowSurface class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QPaintDevice>

QT_BEGIN_NAMESPACE

class QWidget;
class QWSGLWindowSurface;
class QWSGLPaintDevicePrivate;

class Q_OPENGL_EXPORT QWSGLPaintDevice : public QPaintDevice
{
    Q_DECLARE_PRIVATE(QWSGLPaintDevice)
public:
    QWSGLPaintDevice(QWidget *widget);
    ~QWSGLPaintDevice();

    QPaintEngine *paintEngine() const;

    int metric(PaintDeviceMetric m) const;

    QWSGLWindowSurface *windowSurface() const;

private:
    friend class QWSGLWindowSurface;
    QWSGLPaintDevicePrivate *d_ptr;
};


QT_END_NAMESPACE

#endif // QWSGLPAINTDEVICE_GL_P_H
