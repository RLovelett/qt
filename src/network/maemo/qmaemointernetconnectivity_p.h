/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Maemo Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMAEMOINTERNETCONNECTIVITY_P_H
#define QMAEMOINTERNETCONNECTIVITY_P_H

#include "QtCore/qobject.h"
#include "QtDBus/qdbusinterface.h"
#include <QDebug>
#include "qgconfbackend_p.h"

#include <icd/osso-ic-dbus.h>
#include <icd/osso-ic-ui-dbus.h>

#ifdef Q_WS_HILDON

QT_BEGIN_NAMESPACE

class QMaemoICPrivate : public QObject
{
   Q_OBJECT

   enum connectStatus {
        UNKNOWN = -1,
        DISCONNECTED = 0,
        CONNECTED = 1,
        DISCONNECTING = 2,
        CONNECTING = 3
        //SCAN_START
        //SCAN_STOP
   };

public:
    QMaemoICPrivate():
        connected(UNKNOWN),
        icdInterface(0),
        icdUiInterface(0)
    {
        if (!QDBusConnection::systemBus().isConnected()) {
            qFatal("Cannot connect to the D-BUS session bus.");
        }

        //Creating D-Bus interfeces
        icdInterface = new QDBusInterface(ICD_DBUS_SERVICE, ICD_DBUS_PATH, ICD_DBUS_INTERFACE, QDBusConnection::systemBus(), this);
        icdUiInterface = new QDBusInterface(ICD_UI_DBUS_SERVICE, ICD_UI_DBUS_PATH, ICD_UI_DBUS_INTERFACE, QDBusConnection::systemBus(), this);

        QDBusConnection::systemBus().connect(ICD_DBUS_SERVICE, ICD_DBUS_PATH, ICD_DBUS_INTERFACE, "status_changed",
                                             this, SLOT(statusChangedSlot(QString, QString, QString, QString) ));

        //Watching for changes in the proxy settings
        QString httpProxyDir = QString("/system/http_proxy");
        if (!QGConfBackend::self()->connect(httpProxyDir, proxySettingsChanged))
            qWarning() << "QMaemoICPrivate fails to watch for GConf changes in " << httpProxyDir; 
    }

    ~QMaemoICPrivate(){
    }

    bool isConnected(){
        if (connected == UNKNOWN)
            checkConnectionStatus();
        return connected;
    }
    
    //Show connection dialog.
    void connectionRequest()
    {
        if (isConnected()){
            qDebug() << "The device is already connected";
            return;
        }

        QDBusMessage reply;
        reply = icdUiInterface->call(ICD_UI_SHOW_CONNDLG_REQ, false); //### REMOVE ME

#if 0 //TODO: We need a new data class to store IAP.
        if (iap.isEmpty()){
            //Show the the Access point list to the user
            reply = icdUiInterface->call(ICD_UI_SHOW_CONNDLG_REQ, false);
        }else{
            //Connect to the last Access Point used
            qDebug() << "Connecting to" << iap;
            reply = icdInterface->call(ICD_CONNECT_REQ, iap, 0);
        }
#endif
        if (reply.type() == QDBusMessage::ErrorMessage){
            readErrorDBusErrorMsg(reply);
            return;
        }
    }

    bool isHttpProxyUsed()
    {
        if (connected != CONNECTED)
            return false;
        return QGConfBackend::self()->getValue("/system/http_proxy/use_http_proxy").toBool();
    }


    //Called by the QGConfBackend instance when the proxy parameters change.
    static void proxySettingsChanged(QString& key, QVariant value){
        qDebug() << "PROXY SETTINGS CHANGED" << key << value;
        //emit qmic->proxySettingsChangedSig(key, value);
    }

private:
    void checkConnectionStatus(){
        QDBusMessage reply = icdInterface->call(ICD_GET_STATE_REQ);

        if (reply.type() == QDBusMessage::ErrorMessage){
            readErrorDBusErrorMsg(reply);
            connected = UNKNOWN;
            return;
        }

        QList<QVariant> values;
        values = reply.arguments();
        if (values.takeFirst().toInt()){
            connected = CONNECTED;
        }else{
            connected = DISCONNECTED;
        }
    }

    void readErrorDBusErrorMsg(const QDBusMessage& msg)
    {
        qWarning() << "QMaemoInternetConnectivity has received an error message." << endl
                   << "Name: " << msg.errorName() << endl
                   << "Message: " << msg.errorMessage();
    }

private Q_SLOTS:
    //Update the internal vars with the status_changed signals sent by the daemon via D-Bus
    void statusChangedSlot(QString IAPname, QString networkType, QString state, QString errorCode){
        qDebug() << "STATUS CHANGED" << IAPname << networkType << state << errorCode;

        Q_UNUSED(networkType); //TODO

        iap = IAPname;

        if (!errorCode.isEmpty()){
            connected= UNKNOWN;
            qWarning() << errorCode;
        }else if (state == "CONNECTED"){
            connected = CONNECTED;
            //emit icStatusChanged();
        }else if (state == "CONNECTING"){
            connected = CONNECTING;
        }else if (state == "IDLE" && errorCode.isEmpty()){
            connected = DISCONNECTED;
            //emit icStatusChanged();
        }else if (state == "DISCONNECTING"){
            connected = DISCONNECTING;
        }
        qDebug() << "CHECK connected=" << connected;
    }

private:
   int connected;
   QDBusInterface *icdInterface;
   QDBusInterface *icdUiInterface;
   QString iap; //Internet Access Point

};

QT_END_NAMESPACE

#endif //Q_WS_HILDON

#endif //QMAEMOINTERNETCONNECTIVITY_P_H
