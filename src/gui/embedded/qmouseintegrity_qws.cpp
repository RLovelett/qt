/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef QT_NO_QWS_MOUSE_INTEGRITY

#include "qmouseintegrity_qws.h"
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qthread.h>

#include <INTEGRITY.h>


typedef Address MOUSEHandler;
typedef struct MOUSEMessageStruct
{
    Value x;
    Value y;
    Value z;
    Value buttons;
} MOUSEMessage;

static Error MOUSE_Init(MOUSEHandler *handler, Boolean *isabsolute);
static Error MOUSE_SynchronousGetPosition(MOUSEHandler handler, MOUSEMessage *msg,
                                          Boolean absolute);
static Error MOUSE_ShouldFilter(MOUSEHandler handler, Boolean *filter);

QT_BEGIN_NAMESPACE

class QIntMouseListenThread;

class QIntMousePrivate : public QObject
{
    Q_OBJECT
    friend class QIntMouseListenTaskThread;
Q_SIGNALS:
    void mouseDataAvailable(int x, int y, int buttons);
public:
    QIntMousePrivate(QIntMouseHandler *handler);
    ~QIntMousePrivate();
    void dataReady(int x, int y, int buttons) { emit mouseDataAvailable(x, y, buttons); }
    bool calibrated;
    bool waitforread;
    bool suspended;
    QIntMouseListenThread *mousethread;

private:
    QIntMouseHandler *handler;
};

class QIntMouseListenThread : public QThread
{
protected:
    QIntMousePrivate *imp;
    bool loop;
public:
    QIntMouseListenThread(QIntMousePrivate *im) : QThread(), imp(im) {};
    ~QIntMouseListenThread() {};
    void run();
    void stoploop() { loop = false; };
};


QIntMouseHandler::QIntMouseHandler(const QString &driver, const QString &device)
    : QObject(), QWSCalibratedMouseHandler(driver, device)
{
    QPoint test(1,1);
    d = new QIntMousePrivate(this);
    connect(d, SIGNAL(mouseDataAvailable(int, int, int)), this, SLOT(readMouseData(int, int, int)));

    d->calibrated = (test != transform(test));

    d->mousethread->start();
}

QIntMouseHandler::~QIntMouseHandler()
{
    disconnect(d, SIGNAL(mouseDataAvailable(int, int, int)), this, SLOT(readMouseData(int, int, int)));
    delete d;
}

void QIntMouseHandler::resume()
{
    d->suspended = true;
}

void QIntMouseHandler::suspend()
{
    d->suspended = false;
}

void QIntMouseHandler::readMouseData(int x, int y, int buttons)
{
    d->waitforread = false;
    if (d->suspended)
        return;
    if (d->calibrated) {
        sendFiltered(QPoint(x, y), buttons);
    } else {
        QPoint pos;
        pos = transform(QPoint(x, y));
        limitToScreen(pos);
        mouseChanged(pos, buttons, 0);
    }
}

void QIntMouseHandler::clearCalibration()
{
    QWSCalibratedMouseHandler::clearCalibration();
}

void QIntMouseHandler::calibrate(const QWSPointerCalibrationData *data)
{
    QWSCalibratedMouseHandler::calibrate(data);
}

void QIntMouseListenThread::run(void)
{
    MOUSEHandler handler;
    MOUSEMessage msg;
    Boolean filter;
    Boolean isabsolute;
    loop = true;
    CheckSuccess(MOUSE_Init(&handler, &isabsolute));
    CheckSuccess(MOUSE_ShouldFilter(handler, &filter));
    if (!filter)
        imp->calibrated = false;
    imp->waitforread = false;
    do {
        MOUSE_SynchronousGetPosition(handler, &msg, isabsolute);
        imp->dataReady(msg.x, msg.y, msg.buttons);
    } while (loop);
    QThread::exit(0);
}

QIntMousePrivate::QIntMousePrivate(QIntMouseHandler *handler)
    : QObject()
{
    this->handler = handler;
    suspended = false;
    mousethread = new QIntMouseListenThread(this);
}

QIntMousePrivate::~QIntMousePrivate()
{
    mousethread->stoploop();
    mousethread->wait();
    delete mousethread;
}

QT_END_NAMESPACE

#include "qmouseintegrity_qws.moc"

typedef struct USBMouseStruct
{
    Connection mouseconn;
    Buffer mousemsg[2];
    Value x;
    Value y;
} USBMouse;

USBMouse mousedev;

Error MOUSE_Init(MOUSEHandler *handler, Boolean *isabsolute)
{
    Error E;
    bool loop = true;
    memset((void*)&mousedev, 0, sizeof(USBMouse));
    mousedev.mousemsg[0].BufferType = DataImmediate;
    mousedev.mousemsg[1].BufferType = DataImmediate | LastBuffer;
    do {
        E = RequestResource((Object*)&mousedev.mouseconn,
                "MouseClient", "!systempassword");
        if (E == Success) {
            *isabsolute = true;
            loop = false;
        } else {
            E = RequestResource((Object*)&mousedev.mouseconn,
                    "USBMouseClient", "!systempassword");
            if (E == Success) {
                *isabsolute = false;
                loop = false;
            }
        }
        if (loop)
            sleep(1);
    } while (loop);
    *handler = (MOUSEHandler)&mousedev;
    return Success;
}

Error MOUSE_SynchronousGetPosition(MOUSEHandler handler, MOUSEMessage *msg,
        Boolean isabsolute)
{
    signed long x;
    signed long y;
    USBMouse *mdev = (USBMouse *)handler;
    mdev->mousemsg[0].Transferred = 0;
    mdev->mousemsg[1].Transferred = 0;
    SynchronousReceive(mdev->mouseconn, mdev->mousemsg);
    if (isabsolute) {
        x = (signed long)mdev->mousemsg[0].Length;
        y = (signed long)mdev->mousemsg[1].TheAddress;
    } else {
        x = mdev->x + (signed long)mdev->mousemsg[0].Length;
        y = mdev->y + (signed long)mdev->mousemsg[1].TheAddress;
    }
    if (x < 0)
        mdev->x = 0;
    else
        mdev->x = x;
    if (y < 0)
        mdev->y = 0;
    else
        mdev->y = y;
    msg->x = mdev->x;
    msg->y = mdev->y;
    msg->buttons = mdev->mousemsg[0].TheAddress;
    return Success;
}

Error MOUSE_ShouldFilter(MOUSEHandler handler, Boolean *filter)
{
    if (filter == NULL)
        return Failure;
    *filter = false;
    return Success;
}

#endif // QT_NO_QWS_MOUSE_INTEGRITY
