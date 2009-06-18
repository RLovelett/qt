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
/*!
    \class QMaemoInternetConnectivity

    \since 4.5maemo

    \brief The QMaemoInternetConnectivity TODO

    \reentrant
    \ingroup maemo
    \inmodule QtNetwork

    QMaemoInternetConnectivity TODO
*/
#include "qmaemointernetconnectivity.h"
#include "qmaemointernetconnectivity_p.h"

#ifdef Q_WS_HILDON


QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QMaemoICPrivate, maemoICInstance);

bool QMaemoInternetConnectivity::isConnected(){
    return maemoICInstance()->isConnected();
}

static QString currentAPName() {
    return maemoICInstance()->currentAPName();
}

void QMaemoInternetConnectivity::connectionRequest(){
    return maemoICInstance()->connectionRequest();
}

bool QMaemoInternetConnectivity::isHttpProxyUsed(){
    return maemoICInstance()->isHttpProxyUsed();
}

QMaemoICPrivate::QMaemoICPrivate()
	: connected(UNKNOWN)
	, icdInterface(0)
	, icdUiInterface(0)
{
	if (!QDBusConnection::systemBus().isConnected()) {
		qFatal("Cannot connect to the D-BUS session bus.");
	}

	//Creating D-Bus interfeces
	icdInterface = new QDBusInterface(ICD_DBUS_SERVICE, ICD_DBUS_PATH,
			ICD_DBUS_INTERFACE, QDBusConnection::systemBus(), this);
	icdUiInterface = new QDBusInterface(ICD_UI_DBUS_SERVICE, ICD_UI_DBUS_PATH,
			ICD_UI_DBUS_INTERFACE, QDBusConnection::systemBus(), this);

	QDBusConnection::systemBus().connect(ICD_DBUS_SERVICE, ICD_DBUS_PATH,
			ICD_DBUS_INTERFACE, "status_changed",
			this, SLOT(statusChangedSlot(QString, QString, QString, QString) ));

	//Watching for changes in the proxy settings
	QString httpProxyDir = QString("/system/http_proxy");
	if (!QGConfBackend::self()->connect(httpProxyDir, proxySettingsChanged))
		qWarning() << "QMaemoICPrivate fails to watch for GConf changes in " << httpProxyDir; 

}

QMaemoICPrivate::~QMaemoICPrivate()
{
}

bool QMaemoICPrivate::isAutoConnect()
{
	QString auto_connect = QGConfBackend::self()->getValue(
			"/system/osso/connectivity/network_type/auto_connect").toString();
	qDebug() << "auto_connect:" << auto_connect;

	int search_interval = QGConfBackend::self()->getValue(
			"/system/osso/connectivity/network_type/search_interval").toInt();
	qDebug() << "search_interval:" << search_interval;

	// Connect automatically:
	// Always ask -> empty
	// WLAN -> WLAN_INFRA
	// Phone -> DUM_... etc.
	// Any Connection -> *
	if (auto_connect.isEmpty())
		return false;

	if (search_interval == 0)
		return false;

	return true;
}

bool QMaemoICPrivate::isConnected()
{
	qDebug() << "isConnected() :" << connected;
	if (connected == UNKNOWN)
		checkConnectionStatus();
	return connected;
}

void QMaemoICPrivate::connectionRequest()
{
	if (isConnected()) {
		qDebug() << "The device is already connected";
		return;
	}

	if (!isAutoConnect()) {
		qDebug() << "The device should not auto connect";
		return;
	}

	QDBusMessage reply;

	//Check last used network
	//NOTE: Chinook needs lastApName
	const QString lastNetwork = lastAPid();
	qDebug() << "Last used network is " << lastNetwork;
	
	if (lastNetwork.isEmpty()) {
		//Show the the Access point list to the user
		reply = icdUiInterface->call(ICD_UI_SHOW_CONNDLG_REQ, false);
	} else {
		//Connect to the last Access Point used
		qDebug() << "Connecting to" << lastNetwork;
		quint32 i = 0;
		reply = icdInterface->call(ICD_CONNECT_REQ, lastNetwork, i);
	}
	
	
	if (reply.type() == QDBusMessage::ErrorMessage) {
		readErrorDBusErrorMsg(reply);
		if (!lastNetwork.isEmpty())
			icdUiInterface->call(ICD_UI_SHOW_CONNDLG_REQ, false);
		return;
	}
}

bool QMaemoICPrivate::isHttpProxyUsed()
{
	if (connected != CONNECTED)
		return false;
	return QGConfBackend::self()->getValue("/system/http_proxy/use_http_proxy").toBool();
}

QString QMaemoICPrivate::currentAPName()
{
	if (connected != CONNECTED)
		return QString();
	QString gconfKey = iap;
	gconfKey.prepend("/system/osso/connectivity/IAP/");
	gconfKey.append("/name");
	return QGConfBackend::self()->getValue(gconfKey).toString();
}

QString QMaemoICPrivate::lastAPName()
{
	QString gconfKey = lastAPid();
	if (gconfKey.isEmpty())
		return QString();
	gconfKey.prepend("/system/osso/connectivity/IAP/");
	gconfKey.append("/name");
	return QGConfBackend::self()->getValue(gconfKey).toString();
}

QString QMaemoICPrivate::lastAPid()
{
	return QGConfBackend::self()->getValue("/system/osso/connectivity/IAP/last_used_network").toString();
}

//Called by the QGConfBackend instance when the proxy parameters change.
void QMaemoICPrivate::proxySettingsChanged(QString& key, QVariant value)
{
	qDebug() << "PROXY SETTINGS CHANGED" << key << value;
	//emit qmic->proxySettingsChangedSig(key, value);
}

void QMaemoICPrivate::checkConnectionStatus()
{
	qDebug() << "echeckConnectionStatus()";
	QDBusMessage reply = icdInterface->call(ICD_GET_STATE_REQ);

	if (reply.type() == QDBusMessage::ErrorMessage) {
		readErrorDBusErrorMsg(reply);
		connected = UNKNOWN;
		return;
	}

	QList<QVariant> values;
	values = reply.arguments();
	qDebug() << "reply arg size:" << values.size();
#if 0
	if (values.takeFirst().toInt()) {
		connected = CONNECTED;
	} else {
		connected = DISCONNECTED;
	}
#else
	connected = DISCONNECTED;
	while (!values.isEmpty()) {
		int n = values.takeFirst().toInt();
		qDebug() << "status:" << n;
		if (n) {
		//if (values.takeFirst().toInt()) {
			connected = CONNECTED;
			break;
		}
	}
#endif
	qDebug() << "echeckConnectionStatus() done";
}

void QMaemoICPrivate::readErrorDBusErrorMsg(const QDBusMessage& msg)
{
	qWarning() << "QMaemoInternetConnectivity has received an error message." << endl
			   << "Name: " << msg.errorName() << endl
			   << "Message: " << msg.errorMessage();
}

void QMaemoICPrivate::statusChangedSlot(QString IAPname, QString networkType, QString state, QString errorCode)
{
	qDebug() << "STATUS CHANGED" << IAPname << networkType << state << errorCode;

	Q_UNUSED(networkType); //TODO

	iap = IAPname;

	if (!errorCode.isEmpty()) {
		connected= UNKNOWN;
		qWarning() << errorCode;
	} else if (state == "CONNECTED") {
		connected = CONNECTED;
		//emit icStatusChanged();
	} else if (state == "CONNECTING") {
		connected = CONNECTING;
	} else if (state == "IDLE" && errorCode.isEmpty()) {
		connected = DISCONNECTED;
		//emit icStatusChanged();
	} else if (state == "DISCONNECTING") {
		connected = DISCONNECTING;
	}
	qDebug() << "CHECK connected=" << connected;
}

QT_END_NAMESPACE

#endif //Q_WS_HILDON

