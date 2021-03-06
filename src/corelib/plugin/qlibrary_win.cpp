/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qplatformdefs.h"
#include "qlibrary_p.h"
#include "qfile.h"
#include "qdir.h"
#include "qfileinfo.h"
#include "qdir.h"

#if defined(QT_NO_LIBRARY) && defined(Q_OS_WIN)
#undef QT_NO_LIBRARY
#pragma message("QT_NO_LIBRARY is not supported on Windows")
#endif

#include "qt_windows.h"

QT_BEGIN_NAMESPACE

extern QString qt_error_string(int code);

bool QLibraryPrivate::load_sys()
{
#ifdef Q_OS_WINCE
    QString attempt = QFileInfo(fileName).absoluteFilePath();
#else
    QString attempt = fileName;
#endif

    //avoid 'Bad Image' message box
    UINT oldmode = SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
    QT_WA({
        pHnd = LoadLibraryW((TCHAR*)QDir::toNativeSeparators(attempt).utf16());
    } , {
        pHnd = LoadLibraryA(QFile::encodeName(QDir::toNativeSeparators(attempt)).data());
    });
    
    if (pluginState != IsAPlugin) {
#if defined(Q_OS_WINCE)
        if (!pHnd && ::GetLastError() == ERROR_MOD_NOT_FOUND) {
            QString secondAttempt = fileName;
            QT_WA({
                pHnd = LoadLibraryW((TCHAR*)QDir::toNativeSeparators(secondAttempt).utf16());
            } , {
                pHnd = LoadLibraryA(QFile::encodeName(QDir::toNativeSeparators(secondAttempt)).data());
            });
        }
#endif
        if (!pHnd && ::GetLastError() == ERROR_MOD_NOT_FOUND) {
            attempt += QLatin1String(".dll");
            QT_WA({
                pHnd = LoadLibraryW((TCHAR*)QDir::toNativeSeparators(attempt).utf16());
            } , {
                pHnd = LoadLibraryA(QFile::encodeName(QDir::toNativeSeparators(attempt)).data());
            });
        }
    }

    SetErrorMode(oldmode);
    if (!pHnd) {
        errorString = QLibrary::tr("Cannot load library %1: %2").arg(fileName).arg(qt_error_string());
    }
    if (pHnd) {
        errorString.clear();
        QT_WA({
            TCHAR buffer[MAX_PATH + 1];
            ::GetModuleFileNameW(pHnd, buffer, MAX_PATH);
            attempt = QString::fromUtf16(reinterpret_cast<const ushort *>(&buffer));
        }, {
            char buffer[MAX_PATH + 1];
            ::GetModuleFileNameA(pHnd, buffer, MAX_PATH);
            attempt = QString::fromLocal8Bit(buffer);
        });
        const QDir dir =  QFileInfo(fileName).dir();
        const QString realfilename = attempt.mid(attempt.lastIndexOf(QLatin1Char('\\')) + 1);
        if (dir.path() == QLatin1String("."))
            qualifiedFileName = realfilename;
        else
            qualifiedFileName = dir.filePath(realfilename);
    }
    return (pHnd != 0);
}

bool QLibraryPrivate::unload_sys()
{
    if (!FreeLibrary(pHnd)) {
        errorString = QLibrary::tr("Cannot unload library %1: %2").arg(fileName).arg(qt_error_string());
        return false;
    }
    errorString.clear();
    return true;
}

void* QLibraryPrivate::resolve_sys(const char* symbol)
{
#ifdef Q_OS_WINCE
    void* address = (void*)GetProcAddress(pHnd, (const wchar_t*)QString::fromLatin1(symbol).utf16());
#else
    void* address = (void*)GetProcAddress(pHnd, symbol);
#endif
    if (!address) {
        errorString = QLibrary::tr("Cannot resolve symbol \"%1\" in %2: %3").arg(
            QString::fromAscii(symbol)).arg(fileName).arg(qt_error_string());
    } else {
        errorString.clear();
    }
    return address;
}
QT_END_NAMESPACE
