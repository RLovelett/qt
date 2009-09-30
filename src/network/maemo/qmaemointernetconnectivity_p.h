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
    QMaemoICPrivate();
    ~QMaemoICPrivate();
	
    bool isConnected();
	bool isAutoConnect();
    
    //Show connection dialog.
    void connectionRequest();

    bool isHttpProxyUsed();
    QString currentAPName();
    QString lastAPName();

    QString lastAPid();

    //Called by the QGConfBackend instance when the proxy parameters change.
    static void proxySettingsChanged(QString& key, QVariant value);

private:
    void checkConnectionStatus();
    void readErrorDBusErrorMsg(const QDBusMessage& msg);

private Q_SLOTS:
    //Update the internal vars with the status_changed signals sent by the daemon via D-Bus
    void statusChangedSlot(QString IAPname, QString networkType,
			QString state, QString errorCode);

private:
   int connected;
   QDBusInterface *icdInterface;
   QDBusInterface *icdUiInterface;
   QString iap; //Internet Access Point

};

QT_END_NAMESPACE

#endif //Q_WS_HILDON

#endif //QMAEMOINTERNETCONNECTIVITY_P_H
