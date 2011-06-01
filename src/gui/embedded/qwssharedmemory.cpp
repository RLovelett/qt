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

#include "qwssharedmemory_p.h"

#if !defined(QT_NO_QWS_MULTIPROCESS)

#ifdef Q_OS_TKSE
#include <sys/mman.h>
#include "util.h"
#else
#include <sys/shm.h>
#endif

QT_BEGIN_NAMESPACE

QWSSharedMemory::QWSSharedMemory()
#ifdef Q_OS_TKSE
   : shmBase(0), shmSize(0), character(0),  shmFd(-1), shmPid(-1), key((char *) -1)
#else
   : shmBase(0), shmSize(0), character(0),  shmId(-1), key(-1)
#endif
{
}


QWSSharedMemory::~QWSSharedMemory()
{
#ifdef Q_OS_TKSE
    if (!shmBase)
        return;

    if (shmPid != getpid())
        return;

#ifdef QT_SHM_DEBUG
    qDebug("QWSSharedMemory::Sfree : %p pid : %d shmPid : %d", shmBase, getpid(), shmPid);
#endif

    Sfree(shmBase);

    shmBase = 0;
    shmSize = 0;
    shmFd = -1;
#else
    detach();
#endif
}

/*
  man page says:
    On  Linux,  it is possible to attach a shared memory segment even if it
    is already marked to be deleted.  However, POSIX.1-2001 does not  spec-
    ify this behaviour and many other implementations do not support it.
*/

bool QWSSharedMemory::create(int size)
{
#ifdef Q_OS_TKSE
    if (shmBase) {
        Sfree(shmBase);
        shmBase = 0;
        shmSize = 0;
        shmFd = -1;
    }

    shmBase = Smalloc(size);
    if ( shmBase == 0 ) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::create attaching to shared memory");
        qWarning("Error attaching to shared memory id size : %d", size);
        shmBase = 0;
#endif
        return false;
    }
#ifdef QT_SHM_DEBUG
    qDebug("QWSSharedMemory::create : %p pid : %d", shmBase, getpid());
#endif
    shmFd = 1;
    shmPid = getpid();
    shmSize = size;
#else // Q_OS_TKSE

    if (shmId != -1)
        detach();
    shmId = shmget(IPC_PRIVATE, size, IPC_CREAT|0600);

    if (shmId == -1) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::create allocating shared memory");
        qWarning("Error allocating shared memory of size %d", size);
#endif
        return false;
    }
    shmBase = shmat(shmId,0,0);
    shmctl(shmId, IPC_RMID, 0);
    if (shmBase == (void*)-1) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::create attaching to shared memory");
        qWarning("Error attaching to shared memory id %d", shmId);
#endif
        shmBase = 0;
        return false;
    }
#endif // Q_OS_TKSE
    return true;
}

#ifdef  Q_OS_TKSE
bool QWSSharedMemory::attach(void *id)
{
    shmBase = id;
    return true;
}
#else
bool QWSSharedMemory::attach(int id)
{
    if (shmId == id)
        return id != -1;
    if (shmId != -1)
        detach();

    shmBase = shmat(id,0,0);
    if (shmBase == (void*)-1) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::attach attaching to shared memory");
        qWarning("Error attaching to shared memory 0x%x of size %d",
                 id, size());
#endif
        shmBase = 0;
        return false;
    }
    shmId = id;
    return true;
}
#endif

void QWSSharedMemory::detach ()
{
#ifndef Q_OS_TKSE // no need on POSIX.
    if (!shmBase)
        return;
    shmdt (shmBase);
    shmBase = 0;
    shmSize = 0;
    shmId = -1;
#endif
}

void QWSSharedMemory::setPermissions (mode_t mode)
{
#ifndef Q_OS_TKSE // Nothing to do on TKSE
   struct shmid_ds shm;
   shmctl (shmId, IPC_STAT, &shm);
   shm.shm_perm.mode = mode;
   shmctl (shmId, IPC_SET, &shm);
#endif
}

int QWSSharedMemory::size () const
{
#ifdef Q_OS_TKSE
    return shmSize;
#else
    struct shmid_ds shm;
    shmctl (shmId, IPC_STAT, &shm);
    return shm.shm_segsz;
#endif
}


// old API



QWSSharedMemory::QWSSharedMemory (int size, const QString &filename, char c)
{
  shmSize = size;
  shmFile = filename;
  shmBase = 0;
#ifdef Q_OS_TKSE
  shmFd = -1;
#else
  shmId = -1;
#endif
  character = c;
#ifdef Q_OS_TKSE
  key = (char *)shmFile.toLatin1().constData();
#else
  key = ftok (shmFile.toLatin1().constData(), c);
#endif
}



bool QWSSharedMemory::create ()
{
#ifdef Q_OS_TKSE
  shmFd = shm_open (key, O_RDWR | O_CREAT, 0666);
  shmPtr = mmap (NULL, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
#else
  shmId = shmget (key, shmSize, IPC_CREAT | 0666);
#endif
  return (shmFd != -1);
}

void QWSSharedMemory::destroy ()
{
#ifdef Q_OS_TKSE
  if (shmFd != -1)
    shm_unlink(key);
#else
  if (shmId != -1)
    shmctl(shmId, IPC_RMID, 0);
#endif
}

bool QWSSharedMemory::attach ()
{
#ifdef Q_OS_TKSE
  struct stat buf;

  fstat(shmFd, &buf);
  shmPtr = mmap (NULL, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

  return shmPtr != MAP_FAILED;
#else
  if (shmId == -1)
    shmId = shmget (key, shmSize, 0);

  shmBase = shmat (shmId, 0, 0);
  if ((long)shmBase == -1)
      shmBase = 0;

  return (long)shmBase != 0;
#endif
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_MULTIPROCESS
