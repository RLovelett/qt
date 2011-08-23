/****************************************************************************
**
** Copyright (C) 2011 Blit Software S.L.
** All rights reserved.
** Contact: Sergi Díaz (sdiaz@blitsoftware.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Blit Software gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
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

static bool qBuildNtlmv2Response(const QByteArray *bufferIn, QByteArray *bufferOut);
static bool qGetNtlmResponseType1Session(QByteArray *response);



QByteArray QAuthenticatorPrivate::ntlmPhase1()
{
    QByteArray rc;
	qGetNtlmResponseType1Session(&rc);
    return rc;
}

QByteArray QAuthenticatorPrivate::ntlmPhase3(QAuthenticatorPrivate *ctx, const QByteArray &phase2data)
{
	QByteArray rc;
	qBuildNtlmv2Response(&phase2data, &rc);
    return rc;
}


static void resolveLibs()
{
    static bool triedResolve = false;

#ifndef QT_NO_THREAD
    // protect initialization
    QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
#endif
    if (!triedResolve) {
        // need to resolve the security functions

        HINSTANCE securHnd = QSystemLibrary::load(L"secur32");
        if(securHnd) {
            ptrAcquireCredentialsHandleA = (PtrAcquireCredentialsHandleA)GetProcAddress(securHnd, "AcquireCredentialsHandleA");
            ptrInitializeSecurityContextA = (PtrInitializeSecurityContextA)GetProcAddress(securHnd, "InitializeSecurityContextA");
            ptrCompleteAuthToken = (PtrCompleteAuthToken)GetProcAddress(securHnd, "CompleteAuthToken");
        }
        triedResolve = true;
    }
}

static bool qBuildNtlmv2Response(const QByteArray *bufferIn, QByteArray *bufferOut)
{
    if(!ptrInitializeSecurityContextA || !ptrCompleteAuthToken)
        return false;

    SecBufferDesc sbdOut, sbdIn;
    SecBuffer sbOut, sbIn;
    DWORD ctxAttr;
    SECURITY_STATUS isc;

    bufferOut->clear();

    if (bufferIn) {
        sbdIn.ulVersion = SECBUFFER_VERSION;
        sbdIn.cBuffers = 1;
        sbdIn.pBuffers = &sbIn; // just one buffer
        sbIn.BufferType = SECBUFFER_TOKEN;
        sbIn.cbBuffer = bufferIn->length();
        sbIn.pvBuffer = LocalAlloc(0, bufferIn->length());

        memcpy((char*)sbIn.pvBuffer, bufferIn->constData(), bufferIn->length());
    } else {
        sbIn.pvBuffer = NULL;
    }

    sbdOut.ulVersion = SECBUFFER_VERSION;
    sbdOut.cBuffers = 1;
    sbdOut.pBuffers = &sbOut; // just one buffer
    sbOut.BufferType = SECBUFFER_TOKEN; // preparing a token here
    sbOut.cbBuffer = 8000;
    sbOut.pvBuffer = LocalAlloc(0, sbOut.cbBuffer);

    isc = ptrInitializeSecurityContextA(&cred, bufferIn ? &cliCtx : NULL,
        NULL, ISC_REQ_CONNECTION, 0, SECURITY_NETWORK_DREP,
        bufferIn ? &sbdIn : NULL, 0, &cliCtx, &sbdOut, &ctxAttr, &useBefore);

    switch(isc) {
    case SEC_I_COMPLETE_AND_CONTINUE:
        //fall through
    case SEC_I_COMPLETE_NEEDED:
        ptrCompleteAuthToken(&cliCtx, &sbdOut);
        break;
    }

    if (sbIn.pvBuffer) {
        LocalFree(sbIn.pvBuffer);
    }

    if (sbOut.pvBuffer) {
        bufferOut->append((const char*)sbOut.pvBuffer, sbOut.cbBuffer);
        LocalFree(sbOut.pvBuffer);
        return true;
    }
    else {
        return false;
    }
}

static bool qGetNtlmResponseType1Session(QByteArray *response)
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