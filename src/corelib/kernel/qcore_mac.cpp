/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <private/qcore_mac_p.h>
#include <new>

QT_BEGIN_NAMESPACE

QString QCFString::toQString(CFStringRef str)
{
    if (!str)
        return QString();

    CFIndex length = CFStringGetLength(str);
    if (length == 0)
        return QString();

    QString string(length, Qt::Uninitialized);
    CFStringGetCharacters(str, CFRangeMake(0, length), reinterpret_cast<UniChar *>(const_cast<QChar *>(string.unicode())));

    return string;
}

QCFString::operator QString() const
{
    if (string.isEmpty() && type)
        const_cast<QCFString*>(this)->string = toQString(type);
    return string;
}

CFStringRef QCFString::toCFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

QCFString::operator CFStringRef() const
{
    if (!type) {
        if (string.d->data != string.d->array)
            const_cast<QCFString*>(this)->string.realloc(); // ### Qt5: do we really need this stupid user protection?
        const_cast<QCFString*>(this)->type =
            CFStringCreateWithCharactersNoCopy(0,
                                               reinterpret_cast<const UniChar *>(string.unicode()),
                                               string.length(),
                                               kCFAllocatorNull);
    }
    return type;
}

QT_END_NAMESPACE
