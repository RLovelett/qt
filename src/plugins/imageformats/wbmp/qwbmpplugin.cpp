/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <qimageiohandler.h>
#include <qdebug.h>

#ifndef QT_NO_IMAGEFORMATPLUGIN

#ifdef QT_NO_IMAGEFORMAT_WBMP
#undef QT_NO_IMAGEFORMAT_WBMP
#endif
#include "qwbmphandler.h"

QT_BEGIN_NAMESPACE

class QWBMPPlugin : public QImageIOPlugin
{
public:
    virtual QStringList keys() const;
    virtual QImageIOPlugin::Capabilities capabilities(QIODevice* device, const QByteArray& format) const;
    virtual QImageIOHandler* create(QIODevice* device, const QByteArray& format = QByteArray()) const;
};

QImageIOPlugin::Capabilities QWBMPPlugin::capabilities(QIODevice* device, const QByteArray& format) const
{
    if (format == "wbmp")
        return Capabilities(CanRead | CanWrite);

    if (!format.isEmpty())
        return 0;

    if (!device->isOpen())
        return 0;

    Capabilities cap;
    if (device->isReadable() && QWBMPHandler::canRead(device))
        cap |= CanRead;

    if (device->isWritable())
        cap |= CanWrite;

    return cap;
}

QImageIOHandler* QWBMPPlugin::create(QIODevice* device, const QByteArray& format) const
{
    QImageIOHandler* handler = new QWBMPHandler(device);

    handler->setFormat(format);
    return handler;
}

QStringList QWBMPPlugin::keys() const
{
    return QStringList() << QLatin1String("wbmp");
}

Q_EXPORT_PLUGIN2(qwmp, QWBMPPlugin);

QT_END_NAMESPACE

#endif /* QT_NO_IMAGEFORMATPLUGIN */
