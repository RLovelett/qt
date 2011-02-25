/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "inc/pips.h"

#if defined(Q_OS_SYMBIAN)
#include <unistd.h>
#endif

QT_BEGIN_NAMESPACE

PipsProcess::PipsProcess(QObject *parent):
        QObject(parent),
        usePips(false),
        process(NULL),
        childProcessStream(NULL),
        childProcessFD(0),
        buffer(NULL),
        readBuffer(""),
        nbytes(0)
{
}

PipsProcess::~PipsProcess()
{
}

void PipsProcess::setUsePips(bool usePipsAPI)
{
    usePips = usePipsAPI;
}

void PipsProcess::start(const QString & program)
{
    if (!usePips) {
        process = new QProcess(this);
        connect(process, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(processError(QProcess::ProcessError)));
        connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(processFinished(int, QProcess::ExitStatus)));
        connect(process, SIGNAL(readyReadStandardError()),
                this, SLOT(processReadyStdErr()));
        connect(process, SIGNAL(readyReadStandardOutput()),
                this, SLOT(processReadyStdOut()));
        process->start(program);
    } else {
#if defined(Q_OS_SYMBIAN)

        if (!buffer)
            buffer = (char *) malloc(100);
        if (!buffer) {
            processError(QProcess::FailedToStart);
            return;
        }

        QByteArray commandChar = program.toAscii();
        childProcessStream = popen(commandChar.data(), "r");

        if (childProcessStream != NULL) {
            childProcessFD = childProcessStream->_file;
            memset(buffer, 0, sizeof(buffer));

            do {
                nbytes = read(childProcessFD, buffer, sizeof(buffer));
                if (nbytes > 0 ) {
                    readBuffer.append(QByteArray(buffer, nbytes));
                    emit readyReadStandardOutput();
                }
            } while (nbytes > 0 );

            int exitCode = pclose(childProcessStream);
            emit finished(exitCode, (exitCode == -1) ? QProcess::CrashExit : QProcess::NormalExit);
        } else {
            processError(QProcess::FailedToStart);
        }

        if (buffer) {
            free(buffer);
            buffer = NULL;
        }
#endif

    }
}

bool PipsProcess::waitForFinished ()
{
    if (!usePips)
        return process->waitForFinished();
    else
        return true;
}

void PipsProcess::kill()
{
    if (!usePips)
        process->kill();
}

QByteArray PipsProcess::readAllStandardError()
{
    if (!usePips)
        return process->readAllStandardError();
    else
        return "";
}

QByteArray PipsProcess::readAllStandardOutput()
{
    if (!usePips)
        return process->readAllStandardOutput();
    else {
        QByteArray retStr = readBuffer;
        readBuffer.clear();
        return retStr;
    }
}

void PipsProcess::processError(QProcess::ProcessError error)
{
    emit this->error(error);
}

void PipsProcess::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit this->finished(exitCode, exitStatus);
}

void PipsProcess::processReadyStdErr()
{
    emit this->readyReadStandardError();
}

void PipsProcess::processReadyStdOut()
{
    emit this->readyReadStandardOutput();
}

QT_END_NAMESPACE

