/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qwslock_p.h"

#ifndef QT_NO_QWS_MULTIPROCESS

#include "qwssignalhandler_p.h"

#include <qglobal.h>
#include <qdebug.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#ifdef Q_OS_TKSE
#include "util.h"
#else
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

#include <sys/time.h>
#include <time.h>
#ifdef Q_OS_LINUX
#include <linux/version.h>
#endif
#include <unistd.h>

#include <private/qcore_unix_p.h>

QT_BEGIN_NAMESPACE

#ifdef QT_NO_SEMAPHORE
#error QWSLock currently requires semaphores
#endif

QWSLock::QWSLock()
{
#ifdef Q_OS_TKSE // TKSE only supports POSIX semaphore.
    // Genarate semId
    psemId = (semId_type *)Smalloc(sizeof(semId_type));
    if ( psemId == 0 ) {
        perror("QWSLock::QWSLock");
        qFatal("NG : Smalloc failure");
    }
    if ((psemId->sem_id_e[BackingStore] = (sem_t*)Smalloc(sizeof(sem_t))) == 0) {
        perror("QWSLock::QWSLock");
        qFatal("NG : Smalloc failure 1");
    }
    if ((psemId->sem_id_e[Communication] = (sem_t*)Smalloc(sizeof(sem_t))) == 0) {
        perror("QWSLock::QWSLock");
        qFatal("NG : Smalloc failure 2");
    }
    if ((psemId->sem_id_e[RegionEvent] = (sem_t*)Smalloc(sizeof(sem_t))) == 0) {
        perror("QWSLock::QWSLock");
        qFatal("NG : Smalloc failure 3");
    }
    if (sem_init(psemId->sem_id_e[BackingStore], 1, 1) == -1) {
        perror("QWSLock::QWSLock");
        qFatal("Unable to initialize backingstore semaphore");
    }
    lockCount[BackingStore] = 0;
    if (sem_init(psemId->sem_id_e[Communication], 1, 1) == -1) {
        perror("QWSLock::QWSLock");
        qFatal("Unable to initialize communication semaphore");
    }
    lockCount[Communication] = 0;
    if (sem_init(psemId->sem_id_e[RegionEvent], 1, 0) == -1) {
        perror("QWSLock::QWSLock");
        qFatal("Unable to initialize communication semaphore");
    }
    psemId->nid = 3; // not named semaphore
    QWSSignalHandler::instance()->addSemaphore(psemId);
    pId = getpid();
    qDebug("QWSLock::QWSLock : %p pid : %d", psemId, getpid());
#else
    semId = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);

    if (semId == -1) {
        perror("QWSLock::QWSLock");
        qFatal("Unable to create semaphore");
    }
    QWSSignalHandler::instance()->addSemaphore(semId);

    qt_semun semval;
    semval.val = 1;

    if (semctl(semId, BackingStore, SETVAL, semval) == -1) {
        perror("QWSLock::QWSLock");
        qFatal("Unable to initialize backingstore semaphore");
    }
    lockCount[BackingStore] = 0;

    if (semctl(semId, Communication, SETVAL, semval) == -1) {
        perror("QWSLock::QWSLock");
        qFatal("Unable to initialize communication semaphore");
    }
    lockCount[Communication] = 0;

    semval.val = 0;
    if (semctl(semId, RegionEvent, SETVAL, semval) == -1) {
        perror("QWSLock::QWSLock");
        qFatal("Unable to initialize region event semaphore");
    }
#endif
}

#ifdef Q_OS_TKSE
QWSLock::QWSLock(semId_type *pid)
#else
QWSLock::QWSLock(int id)
#endif
{
#ifdef Q_OS_TKSE
    psemId = pid;
    QWSSignalHandler::instance()->addSemaphore(psemId);
    pId = -1; // This process won't execute Sfree
#else
    semId = id;
    QWSSignalHandler::instance()->addSemaphore(semId);
#endif
    lockCount[0] = lockCount[1] = 0;
}

QWSLock::~QWSLock()
{
#ifdef Q_OS_TKSE
    if (pId != getpid())
        return;

    if (SEMID_FAILED(psemId))
        return;

    qDebug("QWSLock::~QWSLock : %p pid : %d pId : %d", psemId, getpid());

    QWSSignalHandler::instance()->removeSemaphore(psemId);

    if (psemId->sem_id_e[BackingStore])
        Sfree(psemId->sem_id_e[BackingStore]);
    if (psemId->sem_id_e[Communication])
        Sfree(psemId->sem_id_e[Communication]);
    if (psemId->sem_id_e[RegionEvent])
        Sfree(psemId->sem_id_e[RegionEvent]);
    if (psemId)
        Sfree(psemId);
#else
    if (semId == -1)
        return;
    QWSSignalHandler::instance()->removeSemaphore(semId);
#endif
}

#ifdef Q_OS_TKSE
static bool forceLock(semId_type *pid, int semNum, int)
#else
static bool forceLock(int semId, int semNum, int)
#endif
{
#ifdef Q_OS_TKSE
    int ret = sem_wait(pid->sem_id_e[semNum]);
    return (ret == 0);
#else
    int ret;
    do {
        sembuf sops = { semNum, -1, 0 };

        // As the BackingStore lock is a mutex, and only one process may own
        // the lock, it's safe to use SEM_UNDO. On the other hand, the
        // Communication lock is locked by the client but unlocked by the
        // server and therefore can't use SEM_UNDO.
        if (semNum == QWSLock::BackingStore)
            sops.sem_flg |= SEM_UNDO;

        ret = semop(semId, &sops, 1);
        if (ret == -1 && errno != EINTR)
            qDebug("QWSLock::lock: %s", strerror(errno));
    } while (ret == -1 && errno == EINTR);

    return (ret != -1);
#endif
}

#ifdef Q_OS_TKSE
static bool up(semId_type *pid, int semNum)
#else
static bool up(int semId, int semNum)
#endif
{
#ifdef Q_OS_TKSE
    int ret = sem_post(pid->sem_id_e[semNum]);
    return (ret == 0);
#else
    int ret;
    do {
        sembuf sops = { semNum, 1, 0 };
        ret = semop(semId, &sops, 1);
        if (ret == -1 && errno != EINTR)
            qDebug("QWSLock::up: %s", strerror(errno));
    } while (ret == -1 && errno == EINTR);

    return (ret != -1);
#endif
}

#ifdef Q_OS_TKSE
static bool down(semId_type *pid, int semNum)
#else
static bool down(int semId, int semNum)
#endif
{
    int ret;
#ifdef Q_OS_TKSE
    do {
        ret = sem_wait(pid->sem_id_e[semNum]);
    } while (ret == -1 && errno == EINTR);
#else
    do {
        sembuf sops = { semNum, -1, 0 };
        ret = semop(semId, &sops, 1);
        if (ret == -1 && errno != EINTR)
            qDebug("QWSLock::down: %s", strerror(errno));
    } while (ret == -1 && errno == EINTR);
#endif

    return (ret != -1);
}

#ifdef Q_OS_TKSE
static int getValue(semId_type *pid, int semNum)
#else
static int getValue(int semId, int semNum)
#endif
{
    int ret;
#ifdef Q_OS_TKSE
    sem_getvalue(pid->sem_id_e[0], &ret);
#else
    do {
        ret = semctl(semId, semNum, GETVAL, 0);
        if (ret == -1 && errno != EINTR)
            qDebug("QWSLock::getValue: %s", strerror(errno));
    } while (ret == -1 && errno == EINTR);
#endif
    return ret;
}

bool QWSLock::lock(LockType type, int timeout)
{
//    qDebug("QWSLock::lock called : %d", type);

    if (type == RegionEvent)
#ifdef Q_OS_TKSE
        return up(psemId, RegionEvent);
#else
        return up(semId, RegionEvent);
#endif

    if (hasLock(type)) {
        ++lockCount[type];
        return true;
    }

#ifdef Q_OS_TKSE
    if (!forceLock(psemId, type, timeout))
#else
    if (!forceLock(semId, type, timeout))
#endif
        return false;

    ++lockCount[type];
    return true;
}

bool QWSLock::hasLock(LockType type)
{
    if (type == RegionEvent)
#ifdef Q_OS_TKSE
        return (getValue(psemId, RegionEvent) == 0);
#else
        return (getValue(semId, RegionEvent) == 0);
#endif

    return (lockCount[type] > 0);
}

void QWSLock::unlock(LockType type)
{
//    qDebug("QWSLock::unlock called : %d", type);

    if (type == RegionEvent) {
#ifdef Q_OS_TKSE
        down(psemId, RegionEvent);
#else
        down(semId, RegionEvent);
#endif
        return;
    }

    if (hasLock(type)) {
        --lockCount[type];
        if (hasLock(type))
            return;
    }

#ifdef Q_OS_TKSE
    int ret, count = 0;
    do {
        ret = sem_post(psemId->sem_id_e[type]);
        if (ret == 0)
            break;
        count++;
    } while (count < 10);
#else
    const int semNum = type;
    int ret;
    do {
        sembuf sops = {semNum, 1, 0};
        if (semNum == QWSLock::BackingStore)
            sops.sem_flg |= SEM_UNDO;

        ret = semop(semId, &sops, 1);
        if (ret == -1 && errno != EINTR)
            qDebug("QWSLock::unlock: %s", strerror(errno));
    } while (ret == -1 && errno == EINTR);
#endif
}

bool QWSLock::wait(LockType type, int timeout)
{
#ifdef Q_OS_TKSE
    bool ok = forceLock(psemId, type, timeout);
#else
    bool ok = forceLock(semId, type, timeout);
#endif
    if (ok)
        unlock(type);
    return ok;
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_MULTIPROCESS
