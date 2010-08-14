/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "QtTest/qmemcheck.h"
#include "QtTest/qbenchmark.h"
#include "QtTest/private/qmemcheck_p.h"
#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qxmltestlogger_p.h"

#if (defined Q_OS_LINUX) || (defined Q_OS_MAC)
#include "3rdparty/memcheck_p.h"
#endif

#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#endif

#if (defined Q_OS_WIN) && (defined Q_CC_MSVC)
#include <crtdbg.h>
#endif

QT_BEGIN_NAMESPACE

namespace QTest
{
    static unsigned long heapCount = 0;
#ifdef Q_OS_SYMBIAN
    //for symbian tests, we also check for request or handler leaks
    static int requestCount = 0;
    static int handlerCount = 0;
#endif

#if (defined Q_OS_WIN) && (defined Q_CC_MSVC)
    _CrtMemState heapState;
#endif

    static int expectedLeak = 0;
}

void QTest::beginMemLeakCheck()
{
    QTest::heapCount = 0;
#if (defined Q_OS_LINUX) || (defined Q_OS_MAC)
    //for linux, get the current heap count using valgrind
    unsigned long base_leaked, base_dubious, base_reachable, base_suppressed;
    VALGRIND_DO_LEAK_CHECK;
    VALGRIND_COUNT_LEAKS(base_leaked, base_dubious, base_reachable, base_suppressed);
    QTest::heapCount = base_leaked;
#elif (defined Q_OS_SYMBIAN)
    //using Symbian native API to get heap usage
    TInt totalSize=0;
    User::AllocSize(totalSize);
    QTest::heapCount = totalSize;
    QTest::requestCount = RThread().RequestCount();
    TInt handlerInThread, handlerInProcess;
    RThread().HandleCount(handlerInThread, handlerInProcess);
    QTest::handlerCount = handlerInThread;
#elif (defined Q_OS_WIN) && (defined Q_CC_MSVC)
    _CrtMemCheckpoint(&heapState);
#endif

    QTest::expectedLeak = 0;
}

/*! \internal
*/
bool QTest::endMemLeakCheck()
{
    int leakCount = 0;
#if (defined Q_OS_LINUX) || (defined Q_OS_MAC)
    unsigned long leaked, dubious, reachable, suppressed;
    VALGRIND_DO_LEAK_CHECK;
    VALGRIND_COUNT_LEAKS(leaked, dubious, reachable, suppressed);
    leakCount = leaked - QTest::heapCount;
#elif (defined Q_OS_SYMBIAN)
    //check for request leak
    int leakedRequest = RThread().RequestCount() - QTest::requestCount;
    if (leakedRequest > 0) {
        QTest::qFail("request leaked in test case", 0, 0);
    }
    //check for handler leak
    TInt handlerInThread, handlerInProcess;
    RThread().HandleCount(handlerInThread, handlerInProcess);
    int leakedHandler = handlerInThread - QTest::handlerCount;
    if (leakedHandler > 0) {
        QTest::qFail("handler leaked in test case", 0, 0);
    }
    //check for memory leak
    TInt totalSize=0;
    User::AllocSize(totalSize);
    leakCount = totalSize - QTest::heapCount;

#elif (defined Q_OS_WIN) && (defined Q_CC_MSVC)
    _CrtMemState curState;
    _CrtMemState diffState;
    _CrtMemCheckpoint(&curState);
    if (_CrtMemDifference(&diffState, &QTest::heapState, &curState)) {
        leakCount = diffState.lTotalCount;
    }
#endif
    if (leakCount > 0) {
        if (leakCount == QTest::expectedLeak) {
            //the leak is expected, ignore it
            return true;
        }
        QTestCharBuffer buf;
        QTest::qt_asprintf(&buf, "memory leaked (%d) bytes", leakCount);
        QTest::qFail(buf.data(), 0, 0);
        return false; 
    }
    return true;
}

void QTest::qExpectLeak(int bytes)
{
    QTest::expectedLeak = bytes;
}

QT_END_NAMESPACE

