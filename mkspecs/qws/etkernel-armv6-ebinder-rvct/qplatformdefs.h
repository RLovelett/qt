/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake spec of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglobal.h"

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif

#include <unistd.h>

#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <dynload.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

namespace std {
#ifndef __uint8_t
typedef unsigned char __uint8_t;
#endif
#ifndef __uint32_t
typedef unsigned int __uint32_t;
#endif
};
#include <netinet/in.h>
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
#endif

#define QT_USE_XOPEN_LFS_EXTENSIONS
#include "../../common/posix/qplatformdefs.h"

#undef QT_SOCKLEN_T

#if defined(__GLIBC__) && (__GLIBC__ >= 2)
#define QT_SOCKLEN_T            socklen_t
#else
#define QT_SOCKLEN_T            int
#endif

#undef QT_OPEN_LARGEFILE
#define QT_OPEN_LARGEFILE       0

#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE >= 500)
#define QT_SNPRINTF		::snprintf
#define QT_VSNPRINTF		::vsnprintf
#endif

#undef QT_LSTAT
#define QT_LSTAT		::stat

#ifdef __GNUC__
#  include <stdarg.h>
#endif

#define QT_QWS_TEMP_DIR QString::fromLatin1(getenv("TMPDIR"))
