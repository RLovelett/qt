/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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


#include <qauthenticator.h>
#include <qauthenticator_p.h>
#include <qdebug.h>
#include <qbytearray.h>
#include <qstring.h>
#include "private/qmutexpool_p.h"
#include <private/qsystemlibrary_p.h>

#define SECURITY_WIN32
#include <Windows.h>
#include <Sspi.h>


QT_BEGIN_NAMESPACE

QT_BEGIN_INCLUDE_NAMESPACE
typedef SECURITY_STATUS (SEC_ENTRY *PtrAcquireCredentialsHandleA)(SEC_CHAR*, SEC_CHAR*, ULONG, PLUID, PVOID, SEC_GET_KEY_FN, PVOID, PCredHandle, PTimeStamp);
static PtrAcquireCredentialsHandleA ptrAcquireCredentialsHandleA = 0;
typedef SECURITY_STATUS (SEC_ENTRY *PtrInitializeSecurityContextA)(PCredHandle, PCtxtHandle, SEC_CHAR*, ULONG, ULONG, ULONG, PSecBufferDesc, ULONG, PCtxtHandle, PSecBufferDesc, PULONG, PTimeStamp);
static PtrInitializeSecurityContextA ptrInitializeSecurityContextA = 0;
typedef SECURITY_STATUS (SEC_ENTRY *PtrCompleteAuthToken)(PCtxtHandle, PSecBufferDesc);
static PtrCompleteAuthToken ptrCompleteAuthToken = 0;
QT_END_INCLUDE_NAMESPACE

static CredHandle cred;  //Handle to the credentials.
static TimeStamp useBefore;
static CtxtHandle cliCtx;

static bool qBuildNtlmv2Response(const QByteArray* sbin, QByteArray* sbout);
static bool qGetNtlmResponseType1Session(QByteArray* response);



QByteArray QAuthenticatorPrivate::ntlmPhase1()
{
    QByteArray rc;
	qGetNtlmResponseType1Session(&rc);
    return rc;
}

QByteArray QAuthenticatorPrivate::ntlmPhase3(QAuthenticatorPrivate *ctx, const QByteArray& phase2data)
{
	QByteArray rc;
	qBuildNtlmv2Response(&phase2data, &rc);
    return rc;
}


static void resolveLibs()
{
    static bool triedResolve = false;
    if (!triedResolve) {
        // need to resolve the security info functions

        // protect initialization
#ifndef QT_NO_THREAD
        QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
        // check triedResolve again, since another thread may have already
        // done the initialization
        if (triedResolve) {
            // another thread did initialize the security function pointers,
            // so we shouldn't do it again.
            return;
        }
#endif

        triedResolve = true;
        HINSTANCE securHnd = QSystemLibrary::load(L"secur32");
        if(securHnd) {
            ptrAcquireCredentialsHandleA = (PtrAcquireCredentialsHandleA)GetProcAddress(securHnd, "AcquireCredentialsHandleA");
            ptrInitializeSecurityContextA = (PtrInitializeSecurityContextA)GetProcAddress(securHnd, "InitializeSecurityContextA");
            ptrCompleteAuthToken = (PtrCompleteAuthToken)GetProcAddress(securHnd, "CompleteAuthToken");
        }
    }
}

static bool qBuildNtlmv2Response(const QByteArray* sbin, QByteArray* sbout)
{
    if(!ptrInitializeSecurityContextA || !ptrCompleteAuthToken)
        return false;

    SecBufferDesc obd, ibd;
    SecBuffer ob, ib;
    DWORD ctxAttr;
    SECURITY_STATUS isc;

    sbout->clear();

    if (sbin) {
        ibd.ulVersion = SECBUFFER_VERSION;
        ibd.cBuffers = 1;
        ibd.pBuffers = &ib; // just one buffer
        ib.BufferType = SECBUFFER_TOKEN;
        ib.cbBuffer = sbin->length();
        ib.pvBuffer = LocalAlloc(0, sbin->length());

        memcpy((char*)ib.pvBuffer, sbin->constData(), sbin->length());
    } else
        ib.pvBuffer = NULL;

    obd.ulVersion = SECBUFFER_VERSION;
    obd.cBuffers = 1;
    obd.pBuffers = &ob; // just one buffer
    ob.BufferType = SECBUFFER_TOKEN; // preparing a token here
    ob.cbBuffer = 8000;
    ob.pvBuffer = LocalAlloc(0, ob.cbBuffer);

    isc = ptrInitializeSecurityContextA(&cred, sbin?&cliCtx:NULL,
        NULL, ISC_REQ_CONNECTION, 0, SECURITY_NETWORK_DREP,
        sbin?&ibd:NULL, 0, &cliCtx, &obd, &ctxAttr, &useBefore);

    switch(isc)
    {
    case SEC_I_COMPLETE_AND_CONTINUE:
    case SEC_I_COMPLETE_NEEDED:
        ptrCompleteAuthToken(&cliCtx, &obd);
        break;
    }

    if (ib.pvBuffer) {
        LocalFree(ib.pvBuffer);
        //ib.pvBuffer = NULL;
    }

    if (ob.pvBuffer) {
        sbout->append((const char*)ob.pvBuffer, ob.cbBuffer);

        LocalFree(ob.pvBuffer);
        //ob.pvBuffer = NULL;

        return true;
    }
    else
        return false;
}

static bool qGetNtlmResponseType1Session(QByteArray* response)
{
    resolveLibs();
    if(ptrAcquireCredentialsHandleA && ptrInitializeSecurityContextA && ptrCompleteAuthToken) {
        if (ptrAcquireCredentialsHandleA(NULL, "NTLM", SECPKG_CRED_OUTBOUND, NULL, /*name&pwd*/NULL, NULL, NULL, &cred, &useBefore) == SEC_E_OK) {
            //We have acquired the credentials from Windows, we are in a domain and can connect to a NTLMv2 proxy.
            return qBuildNtlmv2Response(NULL, response);
        }
    }

    return false;
}

QT_END_NAMESPACE