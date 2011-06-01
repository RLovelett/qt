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

#include "qwssignalhandler_p.h"

#ifndef QT_NO_QWS_SIGNALHANDLER

#include <sys/types.h>
#ifndef QT_NO_QWS_MULTIPROCESS
# ifdef Q_OS_TKSE
#  include <semaphore.h>
#  include "util.h"
# else
#  include <sys/ipc.h>
#  include <sys/sem.h>
# endif

#  include <private/qcore_unix_p.h>
#endif
#include <signal.h>

QT_BEGIN_NAMESPACE

class QWSSignalHandlerPrivate : public QWSSignalHandler
{
public:
    QWSSignalHandlerPrivate() : QWSSignalHandler() {}
};


Q_GLOBAL_STATIC(QWSSignalHandlerPrivate, signalHandlerInstance);


QWSSignalHandler* QWSSignalHandler::instance()
{
    return signalHandlerInstance();
}

QWSSignalHandler::QWSSignalHandler()
{
    const int signums[] = { SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE,
                            SIGSEGV, SIGTERM, SIGBUS };
    const int n = sizeof(signums)/sizeof(int);

    for (int i = 0; i < n; ++i) {
        const int signum = signums[i];
        qt_sighandler_t old = signal(signum, handleSignal);
        if (old == SIG_IGN) // don't remove shm and semaphores when ignored
            signal(signum, old);
        else
            oldHandlers[signum] = (old == SIG_ERR ? SIG_DFL : old);
    }
}

QWSSignalHandler::~QWSSignalHandler()
{
#ifndef QT_NO_QWS_MULTIPROCESS
# ifdef Q_OS_TKSE
     /*
       This code causes data abort when shutdown QWSServer.
       Because QWSClient already calls removeSemaphore and Sfree "semId_type *psemno"
       in QWSLock::~QWSLock.
    */ 
    /*
    while (!psemaphores.isEmpty())
        removeSemaphore(psemaphores.last());
    */
# else
    while (!semaphores.isEmpty())
        removeSemaphore(semaphores.last());
# endif
#endif
}

#ifndef QT_NO_QWS_MULTIPROCESS
#ifdef Q_OS_TKSE
void QWSSignalHandler::removeSemaphore(semId_type *psemno)
#else
void QWSSignalHandler::removeSemaphore(int semno)
#endif
{
#ifdef Q_OS_TKSE
    const int index = psemaphores.lastIndexOf(psemno);
#else
    const int index = semaphores.lastIndexOf(semno);
#endif
    if (index != -1) {
#ifdef Q_OS_TKSE
        if (psemno->nid != 1) {
            sem_destroy(psemno->sem_id_e[0]); // BackingStore
            sem_destroy(psemno->sem_id_e[1]); // Communication
            sem_destroy(psemno->sem_id_e[2]); // RegionEvent
        } else {
            sem_unlink(psemno->fname);
        }
        psemaphores.remove(index);
#else
        qt_semun semval;
        semval.val = 0;
        semctl(semaphores.at(index), 0, IPC_RMID, semval);
        semaphores.remove(index);
#endif

    }
}
#endif // QT_NO_QWS_MULTIPROCESS

void QWSSignalHandler::handleSignal(int signum)
{
    QWSSignalHandler *h = instance();

    signal(signum, h->oldHandlers[signum]);

#ifndef QT_NO_QWS_MULTIPROCESS
# ifdef Q_OS_TKSE
    // assume nid = 1, it has named semaphore.
    for (int i = 0; i < h->psemaphores.size(); ++i) {
      if (h->psemaphores.at(i)->nid != 1) {
          sem_destroy(h->psemaphores.at(i)->sem_id_e[0]); // BackingStore
          sem_destroy(h->psemaphores.at(i)->sem_id_e[1]); // Communication
          sem_destroy(h->psemaphores.at(i)->sem_id_e[2]); // RegionEvent
          if (h->psemaphores.at(i)->sem_id_e[0])
              Sfree(h->psemaphores.at(i)->sem_id_e[0]);
          if (h->psemaphores.at(i)->sem_id_e[1])
              Sfree(h->psemaphores.at(i)->sem_id_e[1]);
          if (h->psemaphores.at(i)->sem_id_e[2])
              Sfree(h->psemaphores.at(i)->sem_id_e[2]);
          if (h->psemaphores.at(i))
              Sfree(h->psemaphores.at(i));
      } else {
          sem_unlink (h->psemaphores.at(i)->fname);
          if (h->psemaphores.at(i)->sem_id_e[0])
              free(h->psemaphores.at(i)->sem_id_e[0]);
      }
    }
# else
    qt_semun semval;
    semval.val = 0;
    for (int i = 0; i < h->semaphores.size(); ++i)
        semctl(h->semaphores.at(i), 0, IPC_RMID, semval);
# endif
#endif

    h->objects.clear();
    raise(signum);
}

QT_END_NAMESPACE

#endif // QT_QWS_NO_SIGNALHANDLER
