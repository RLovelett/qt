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

#include "qgconfbackend_p.h"

#include <QDebug>

#ifdef Q_WS_HILDON

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QGConfBackend, gconfBackend);

QGConfBackend::QGConfBackend()
{
    qgconf_loadLibGConf();
    client = q_gconf_client_get_default();
}

QGConfBackend::~QGConfBackend()
{
    g_object_unref(client);
    qgconf_unloadLibGConf();
}

QVariant QGConfBackend::getValue(const QString &key)
{
    QVariant retValue;
    if (q_gconf_client_get_default != 0) {
        GConfClient* client = q_gconf_client_get_default();
        GError *err = 0;
        //Getting the Gconf value type
        GConfValue* value = 0;

        value = q_gconf_client_get(client, qPrintable(key), &err);
        if (err) {
            //qDebug() << "ERROR: Unable to get the Value";
            g_error_free (err);
            return QVariant();
        }
        if (!value){
            //qDebug() << "VALUE is 0";
            return QVariant();
        }
        switch(value->type){
        case GCONF_VALUE_STRING: {
            char *str = q_gconf_client_get_string(client, qPrintable(key), &err);
            if (!err) {
                retValue.setValue(QString::fromUtf8(str));
                g_free(str);
            }
        } break;
        case GCONF_VALUE_BOOL: {
            gboolean b = q_gconf_client_get_bool(client, qPrintable(key), &err);
            if (!err) {
                retValue.setValue((bool)b);
            }
        } break;
        case GCONF_VALUE_INT: {
            gint i = q_gconf_client_get_int(client, qPrintable(key), &err);
            if (!err) {
                retValue.setValue((int)i);
            }
        } break;
        default:
            //qDebug("QGConfBackend::getGConfValue: Sorry, the type %d has not been ported ", value->type);
            return QVariant();
        }

        if (err)
            g_error_free (err);

        return retValue;
    }

    return QVariant();
}

bool QGConfBackend::connect(const QString& dir, slot funcToCall){
    //TODO Dir Check

    //Check if the dir exists already in the Map
    /*
    QMap<QString, slot>::const_iterator i = map.find(dir);
    if (i != map.end()){
        //qDebug() << "The key exists already in the Map.";
        return false;
    }*/

    if ( map.value(dir))
        return false;

    gconfBackend()->map.insert(dir, funcToCall);
    watch(dir, callback);
    return true;
}

QGConfBackend* QGConfBackend::self(){
    return gconfBackend();
}

void QGConfBackend::watch(const QString& dir, callbackFunc cPtr){
    GError *err = 0;

    q_gconf_client_add_dir(client, qPrintable(dir), GCONF_CLIENT_PRELOAD_NONE, &err);

    if (err) {
        //qDebug() <<  "Failed to add a watch to GCClient:" << err->message;
        g_error_free (err);
    }

    q_gconf_client_notify_add(client, qPrintable(dir), cPtr, 0, 0, &err);

    if (err) {
        //qDebug() << "Failed to add register the callback" << err->message;
        g_error_free (err);
    }
}

void QGConfBackend::callback(GConfClient* a, guint b, GConfEntry* c, void* d){
    Q_UNUSED(a);
    Q_UNUSED(b);
    Q_UNUSED(d);
    //qDebug() << "KEY" << c->key << "VALUE TYPE" << c->value->type;

    QVariant value;
    const QString fullKey = c->key;
    QString key, dir;

    key = fullKey.split("/").last();
    dir = fullKey;
    dir.remove("/" + key);

    slot slotToCall = gconfBackend()->map.value(dir);
    value = gconfBackend()->getValue(fullKey);
    (*slotToCall)(key, value);
}

QT_END_NAMESPACE

#endif //Q_WS_HILDON
