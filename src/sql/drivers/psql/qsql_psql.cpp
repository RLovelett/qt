/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#include "qsql_psql.h"

#include <qcoreapplication.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qregexp.h>

#include <qsqlfield.h>
#include <qsqlindex.h>
#include <qsqlrecord.h>
#include <qsqlquery.h>
#include <qsocketnotifier.h>
#include <qstringlist.h>
#include <qmutex.h>

#include <libpq-fe.h>
#include <pg_config.h>

#include <stdlib.h>
#include <math.h>
// below code taken from an example at http://www.gnu.org/software/hello/manual/autoconf/Function-Portability.html
#ifndef isnan
    # define isnan(x) \
        (sizeof (x) == sizeof (long double) ? isnan_ld (x) \
        : sizeof (x) == sizeof (double) ? isnan_d (x) \
        : isnan_f (x))
    static inline int isnan_f  (float       x) { return x != x; }
    static inline int isnan_d  (double      x) { return x != x; }
    static inline int isnan_ld (long double x) { return x != x; }
#endif

#ifndef isinf
    # define isinf(x) \
        (sizeof (x) == sizeof (long double) ? isinf_ld (x) \
        : sizeof (x) == sizeof (double) ? isinf_d (x) \
        : isinf_f (x))
    static inline int isinf_f  (float       x) { return isnan (x - x); }
    static inline int isinf_d  (double      x) { return isnan (x - x); }
    static inline int isinf_ld (long double x) { return isnan (x - x); }
#endif


// workaround for postgres defining their OIDs in a private header file
#define QBOOLOID 16
#define QINT8OID 20
#define QINT2OID 21
#define QINT4OID 23
#define QNUMERICOID 1700
#define QFLOAT4OID 700
#define QFLOAT8OID 701
#define QABSTIMEOID 702
#define QRELTIMEOID 703
#define QDATEOID 1082
#define QTIMEOID 1083
#define QTIMETZOID 1266
#define QTIMESTAMPOID 1114
#define QTIMESTAMPTZOID 1184
#define QOIDOID 2278
#define QBYTEAOID 17
#define QREGPROCOID 24
#define QXIDOID 28
#define QCIDOID 29

/* This is a compile time switch - if PQfreemem is declared, the compiler will use that one,
   otherwise it'll run in this template */
template <typename T>
inline void PQfreemem(T *t, int = 0) { free(t); }

Q_DECLARE_METATYPE(PGconn*)
Q_DECLARE_METATYPE(PGresult*)

QT_BEGIN_NAMESPACE

inline void qPQfreemem(void *buffer)
{
    PQfreemem(buffer);
}


QPSQLResult::QPSQLResult(const QPSQLDriver* db)
    : QSqlResult(db), driver(db), result(0)
{
    preparedQueriesEnabled = driver->hasFeature(QSqlDriver::PreparedQueries);
    driver->resultCheckIn(this);
    cleanup();
}

QPSQLResult::~QPSQLResult()
{
    cleanup();
    if (driver)
        driver->resultCheckOut(this);
}

QVariant QPSQLResult::handle() const
{
    return qVariantFromValue(result);
}

void QPSQLResult::cleanup()
{
    if (result)
        PQclear(result);
    result = 0;
    setAt(QSql::BeforeFirstRow);
    currentSize = -1;
    setActive(false);
}

bool QPSQLResult::fetch(int i)
{
    if (i < 0)
        return false;
    if (i >= currentSize)
        return false;
    setAt(i);
    return true;
}

bool QPSQLResult::fetchFirst()
{
    return fetch(0);
}

bool QPSQLResult::fetchLast()
{
    return fetch(PQntuples(result) - 1);
}

QVariant QPSQLResult::data(int i)
{
    if (i >= PQnfields(result)) {
        qWarning("QPSQLResult::data: column %d out of range", i);
        return QVariant();
    }
    int ptype = PQftype(result, i);
    QVariant::Type type = driver->decodePGType(ptype);
    const char *val = PQgetvalue(result, at(), i);
    if (PQgetisnull(result, at(), i))
        return QVariant(type);
    switch (type) {
    case QVariant::Bool:
        return QVariant((bool)(val[0] == 't'));
    case QVariant::String:
        return driver->isUtf8 ? QString::fromUtf8(val) : QString::fromAscii(val);
    case QVariant::LongLong:
        if (val[0] == '-')
            return QString::fromLatin1(val).toLongLong();
        else
            return QString::fromLatin1(val).toULongLong();
    case QVariant::Int:
        return atoi(val);
    case QVariant::Double:
        if (ptype == QNUMERICOID) {
            if (numericalPrecisionPolicy() != QSql::HighPrecision) {
                QVariant retval;
                bool convert;
                double dbl=QString::fromAscii(val).toDouble(&convert);
                if (numericalPrecisionPolicy() == QSql::LowPrecisionInt64)
                    retval = (qlonglong)dbl;
                else if (numericalPrecisionPolicy() == QSql::LowPrecisionInt32)
                    retval = (int)dbl;
                else if (numericalPrecisionPolicy() == QSql::LowPrecisionDouble)
                    retval = dbl;
                if (!convert)
                    return QVariant();
                return retval;
            }
            return QString::fromAscii(val);
        }
        return QString::fromAscii(val).toDouble();
    case QVariant::Date:
        if (val[0] == '\0') {
            return QVariant(QDate());
        } else {
#ifndef QT_NO_DATESTRING
            return QVariant(QDate::fromString(QString::fromLatin1(val), Qt::ISODate));
#else
            return QVariant(QString::fromLatin1(val));
#endif
        }
    case QVariant::Time: {
        const QString str = QString::fromLatin1(val);
#ifndef QT_NO_DATESTRING
        if (str.isEmpty())
            return QVariant(QTime());
        if (str.at(str.length() - 3) == QLatin1Char('+') || str.at(str.length() - 3) == QLatin1Char('-'))
            // strip the timezone
            // TODO: fix this when timestamp support comes into QDateTime
            return QVariant(QTime::fromString(str.left(str.length() - 3), Qt::ISODate));
        return QVariant(QTime::fromString(str, Qt::ISODate));
#else
        return QVariant(str);
#endif
    }
    case QVariant::DateTime: {
        QString dtval = QString::fromLatin1(val);
#ifndef QT_NO_DATESTRING
        if (dtval.length() < 10)
            return QVariant(QDateTime());
        // remove the timezone
        // TODO: fix this when timestamp support comes into QDateTime
        if (dtval.at(dtval.length() - 3) == QLatin1Char('+') || dtval.at(dtval.length() - 3) == QLatin1Char('-'))
            dtval.chop(3);
        // milliseconds are sometimes returned with 2 digits only
        if (dtval.at(dtval.length() - 3).isPunct())
            dtval += QLatin1Char('0');
        if (dtval.isEmpty())
            return QVariant(QDateTime());
        else
            return QVariant(QDateTime::fromString(dtval, Qt::ISODate));
#else
        return QVariant(dtval);
#endif
    }
    case QVariant::ByteArray: {
        size_t len;
        unsigned char *data = PQunescapeBytea((unsigned char*)val, &len);
        QByteArray ba((const char*)data, len);
        qPQfreemem(data);
        return QVariant(ba);
    }
    default:
    case QVariant::Invalid:
        qWarning("QPSQLResult::data: unknown data type");
    }
    return QVariant();
}

bool QPSQLResult::isNull(int field)
{
    PQgetvalue(result, at(), field);
    return PQgetisnull(result, at(), field);
}

bool QPSQLResult::reset(const QString& query)
{
    cleanup();
    if (!driver)
        return false;

    result = driver->exec(query);
    return processResults();
}

int QPSQLResult::size()
{
    return currentSize;
}

int QPSQLResult::numRowsAffected()
{
    return QString::fromLatin1(PQcmdTuples(result)).toInt();
}

QVariant QPSQLResult::lastInsertId() const
{
    if (isActive()) {
        Oid id = PQoidValue(result);
        if (id != InvalidOid)
            return QVariant(id);
    }
    return QVariant();
}

QSqlRecord QPSQLResult::record() const
{
    QSqlRecord info;
    if (!isActive() || !isSelect())
        return info;

    int count = PQnfields(result);
    for (int i = 0; i < count; ++i) {
        QSqlField f;
        if (driver->isUtf8)
            f.setName(QString::fromUtf8(PQfname(result, i)));
        else
            f.setName(QString::fromLocal8Bit(PQfname(result, i)));
        f.setType(driver->decodePGType(PQftype(result, i)));
        int len = PQfsize(result, i);
        int precision = PQfmod(result, i);
        // swap length and precision if length == -1
        if (len == -1 && precision > -1) {
            len = precision - 4;
            precision = -1;
        }
        f.setLength(len);
        f.setPrecision(precision);
        f.setSqlType(PQftype(result, i));
        info.append(f);
    }
    return info;
}

void QPSQLResult::virtual_hook(int id, void *data)
{
    Q_ASSERT(data);

    switch (id) {
    default:
        QSqlResult::virtual_hook(id, data);
    }
}

bool QPSQLResult::prepare(const QString &query)
{
    if (!driver)
        return false;
    if (!preparedQueriesEnabled)
        return QSqlResult::prepare(query);

    cleanup();
    driver->deallocate(preparedStmtId);

    // save original query, because we use getNamedPlaceholder()
    // and call setQuery() repeatedly
    QString orig = lastQuery();
    QString pq = query; // prepared query
    if (bindingSyntax() == QSqlResult::NamedBinding) {
        // replace all :foo with $n
        int from = 0;
        int found;
        QString holder;
        QStringList allHolder;
        while (true) {
            found = getNamedPlaceholder(from, holder);
            if (-1 == found) break;

            QString substitude;
            substitude = QLatin1Char('$');
            const int exist = allHolder.indexOf(holder);
            if (exist > -1) {
                substitude += QString::number(exist + 1);
            } else {
                allHolder.append(holder);
                substitude += QString::number(allHolder.size());
            }
            pq.replace(found, holder.length(), substitude);
            setQuery(pq);
            from = found + substitude.length();
        }
    } else if (bindingSyntax() == QSqlResult::PositionalBinding) {
          // replace all ? with $n
          int from = 0;
          int found;
          int i = 1;
          while (true) {
              found = getPositionalPlaceHolder(from);
              if (-1 == found) break;

              QString substitude;
              substitude = QLatin1Char('$');
              substitude += QString::number(i);
              pq.replace(found, 1, substitude);
              setQuery(pq);
              from = found + substitude.length();
              ++i;
          }
    }

    setQuery(orig);

    const QString stmtId = driver->newStmtId();
    QString stmt = QString::fromLatin1("PREPARE %1 AS ").arg(stmtId).append(pq);
    PGresult *result = driver->exec(stmt);

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        setLastError(driver->makeError(QCoreApplication::translate("QPSQLResult",
                                "Unable to prepare statement"), QSqlError::StatementError));
        PQclear(result);
        preparedStmtId.clear();
        return false;
    }

    PQclear(result);
    preparedStmtId = stmtId;
    return true;
}

bool QPSQLResult::exec()
{
    if (!preparedQueriesEnabled)
        return QSqlResult::exec();

    cleanup();

    QString stmt;
    if (bindingSyntax() != QSqlResult::NotUsed) {
        const QString params = driver->createParamString(boundValues());
        stmt = QString::fromLatin1("EXECUTE %1 (%2)").arg(preparedStmtId).arg(params);
    } else {
        stmt = QString::fromLatin1("EXECUTE %1").arg(preparedStmtId);
    }

    result = driver->exec(stmt);

    return processResults();
}

bool QPSQLResult::processResults()
{
    if (!result)
        return false;

    int status = PQresultStatus(result);
    if (status == PGRES_TUPLES_OK) {
        setSelect(true);
        setActive(true);
        currentSize = PQntuples(result);
        return true;
    } else if (status == PGRES_COMMAND_OK) {
        setSelect(false);
        setActive(true);
        currentSize = -1;
        return true;
    }
    setLastError(driver->makeError(QCoreApplication::translate("QPSQLResult",
                    "Unable to create query"), QSqlError::StatementError));
    return false;
}

void QPSQLResult::driverIsGone()
{
    driver = 0;
}

///////////////////////////////////////////////////////////////////

static bool setEncodingUtf8(PGconn* connection)
{
    PGresult* result = PQexec(connection, "SET CLIENT_ENCODING TO 'UNICODE'");
    int status = PQresultStatus(result);
    PQclear(result);
    return status == PGRES_COMMAND_OK;
}

static void setDatestyle(PGconn* connection)
{
    PGresult* result = PQexec(connection, "SET DATESTYLE TO 'ISO'");
    int status =  PQresultStatus(result);
    if (status != PGRES_COMMAND_OK)
        qWarning("%s", PQerrorMessage(connection));
    PQclear(result);
}

static QPSQLDriver::Protocol getPSQLVersion(PGconn* connection)
{
    QPSQLDriver::Protocol serverVersion = QPSQLDriver::Version6;
    PGresult* result = PQexec(connection, "select version()");
    int status = PQresultStatus(result);
    if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
        QString val = QString::fromAscii(PQgetvalue(result, 0, 0));
        QRegExp rx(QLatin1String("(\\d+)\\.(\\d+)"));
        rx.setMinimal(true); // enforce non-greedy RegExp
        if (rx.indexIn(val) != -1) {
            int vMaj = rx.cap(1).toInt();
            int vMin = rx.cap(2).toInt();

            switch (vMaj) {
            case 7:
                switch (vMin) {
                case 0:
                    serverVersion = QPSQLDriver::Version7;
                    break;
                case 1:
                case 2:
                    serverVersion = QPSQLDriver::Version71;
                    break;
                default:
                    serverVersion = QPSQLDriver::Version73;
                    break;
                }
                break;
            case 8:
                switch (vMin) {
                case 0:
                    serverVersion = QPSQLDriver::Version8;
                    break;
                case 1:
                    serverVersion = QPSQLDriver::Version81;
                    break;
                case 2:
                default:
                    serverVersion = QPSQLDriver::Version82;
                    break;
                }
                break;
            default:
                break;
            }
        }
    }
    PQclear(result);

    if (serverVersion < QPSQLDriver::Version71)
        qWarning("This version of PostgreSQL is not supported and may not work.");

    return serverVersion;
}

QPSQLDriver::QPSQLDriver(QObject *parent)
    : QSqlDriver(parent), connection(0), socketNotifier(0), mutex(new QMutex)
{
}

QPSQLDriver::~QPSQLDriver()
{
    close();
    delete mutex;
}

QVariant QPSQLDriver::handle() const
{
    return qVariantFromValue(connection);
}

bool QPSQLDriver::hasFeature(DriverFeature f) const
{
    switch (f) {
    case Transactions:
    case QuerySize:
    case LastInsertId:
    case LowPrecisionNumbers:
    case EventNotifications:
        return true;
    case PreparedQueries:
    case PositionalPlaceholders:
    case NamedPlaceholders:
        return protocol >= QPSQLDriver::Version82;
    case BatchOperations:
    case SimpleLocking:
    case FinishQuery:
    case MultipleResultSets:
        return false;
    case BLOB:
        return protocol >= QPSQLDriver::Version71;
    case Unicode:
        return isUtf8;
    }
    return false;
}

/*
   Quote a string for inclusion into the connection string
   \ -> \\
   ' -> \'
   surround string by single quotes
 */
static QString qQuote(QString s)
{
    s.replace(QLatin1Char('\\'), QLatin1String("\\\\"));
    s.replace(QLatin1Char('\''), QLatin1String("\\'"));
    s.append(QLatin1Char('\'')).prepend(QLatin1Char('\''));
    return s;
}

bool QPSQLDriver::open(const QString & db,
                       const QString & user,
                       const QString & password,
                       const QString & host,
                       int port,
                       const QString& connOpts)
{
    if (isOpen())
        close();
    QString connectString;
    if (!host.isEmpty())
        connectString.append(QLatin1String("host=")).append(qQuote(host));
    if (!db.isEmpty())
        connectString.append(QLatin1String(" dbname=")).append(qQuote(db));
    if (!user.isEmpty())
        connectString.append(QLatin1String(" user=")).append(qQuote(user));
    if (!password.isEmpty())
        connectString.append(QLatin1String(" password=")).append(qQuote(password));
    if (port != -1)
        connectString.append(QLatin1String(" port=")).append(qQuote(QString::number(port)));

    // add any connect options - the server will handle error detection
    if (!connOpts.isEmpty()) {
        QString opt = connOpts;
        opt.replace(QLatin1Char(';'), QLatin1Char(' '), Qt::CaseInsensitive);
        connectString.append(QLatin1Char(' ')).append(opt);
    }

    connection = PQconnectdb(connectString.toLocal8Bit().constData());
    if (PQstatus(connection) == CONNECTION_BAD) {
        setLastError(makeError(tr("Unable to connect"), QSqlError::ConnectionError));
        setOpenError(true);
        PQfinish(connection);
        connection = 0;
        return false;
    }

    protocol = getPSQLVersion(connection);
    isUtf8 = setEncodingUtf8(connection);
    setDatestyle(connection);

    setOpen(true);
    setOpenError(false);
    return true;
}

void QPSQLDriver::close()
{
    seid.clear();
    if (socketNotifier) {
        disconnect(socketNotifier, SIGNAL(activated(int)), this, SLOT(_q_handleNotification(int)));
        delete socketNotifier;
        socketNotifier = 0;
    }

    if (connection) {
        foreach (QPSQLResult *result, activeResults) {
            deallocate(result->preparedStmtId);
            result->driverIsGone();
        }
        PQfinish(connection);
    }
    connection = 0;
    setOpen(false);
    setOpenError(false);
}

QSqlResult *QPSQLDriver::createResult() const
{
    return new QPSQLResult(this);
}

bool QPSQLDriver::beginTransaction()
{
    if (!isOpen()) {
        qWarning("QPSQLDriver::beginTransaction: Database not open");
        return false;
    }
    PGresult* res = PQexec(connection, "BEGIN");
    if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        setLastError(makeError(tr("Could not begin transaction"),
                               QSqlError::TransactionError));
        return false;
    }
    PQclear(res);
    return true;
}

bool QPSQLDriver::commitTransaction()
{
    if (!isOpen()) {
        qWarning("QPSQLDriver::commitTransaction: Database not open");
        return false;
    }
    PGresult* res = PQexec(connection, "COMMIT");

    bool transaction_failed = false;

    // XXX
    // This hack is used to tell if the transaction has succeeded for the protocol versions of
    // PostgreSQL below. For 7.x and other protocol versions we are left in the dark.
    // This hack can dissapear once there is an API to query this sort of information.
    if (protocol == QPSQLDriver::Version8 ||
        protocol == QPSQLDriver::Version81 ||
        protocol == QPSQLDriver::Version82) {
        transaction_failed = qstrcmp(PQcmdStatus(res), "ROLLBACK") == 0;
    }

    if (!res || PQresultStatus(res) != PGRES_COMMAND_OK || transaction_failed) {
        PQclear(res);
        setLastError(makeError(tr("Could not commit transaction"),
                               QSqlError::TransactionError));
        return false;
    }
    PQclear(res);
    return true;
}

bool QPSQLDriver::rollbackTransaction()
{
    if (!isOpen()) {
        qWarning("QPSQLDriver::rollbackTransaction: Database not open");
        return false;
    }
    PGresult* res = PQexec(connection, "ROLLBACK");
    if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
        setLastError(makeError(tr("Could not rollback transaction"),
                               QSqlError::TransactionError));
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

QStringList QPSQLDriver::tables(QSql::TableType type) const
{
    QStringList tl;
    if (!isOpen())
        return tl;
    QSqlQuery t(createResult());
    t.setForwardOnly(true);

    if (type & QSql::Tables)
        appendTables(tl, t, QLatin1Char('r'));
    if (type & QSql::Views)
        appendTables(tl, t, QLatin1Char('v'));
    if (type & QSql::SystemTables) {
        t.exec(QLatin1String("select relname from pg_class where (relkind = 'r') "
                "and (relname like 'pg_%') "));
        while (t.next())
            tl.append(t.value(0).toString());
    }

    return tl;
}

static void qSplitTableName(QString &tablename, QString &schema)
{
    int dot = tablename.indexOf(QLatin1Char('.'));
    if (dot == -1)
        return;
    schema = tablename.left(dot);
    tablename = tablename.mid(dot + 1);
}

void QPSQLDriver::resultCheckIn(QPSQLResult *result) const
{
    activeResults.insert(result);
}

void QPSQLDriver::resultCheckOut(QPSQLResult *result) const
{
    deallocate(result->preparedStmtId);
    activeResults.remove(result);
}

PGresult *QPSQLDriver::exec(const QString &query) const
{
    return PQexec(connection, isUtf8 ? query.toUtf8().constData()
                                     : query.toLocal8Bit().constData());
}

void QPSQLDriver::appendTables(QStringList &tl, QSqlQuery &t, QChar type) const
{
    QString query;
    if (protocol >= QPSQLDriver::Version73) {
        query = QString::fromLatin1("select pg_class.relname, pg_namespace.nspname from pg_class "
                  "left join pg_namespace on (pg_class.relnamespace = pg_namespace.oid) "
                  "where (pg_class.relkind = '%1') and (pg_class.relname !~ '^Inv') "
                  "and (pg_class.relname !~ '^pg_') "
                  "and (pg_namespace.nspname != 'information_schema') ").arg(type);
    } else {
        query = QString::fromLatin1("select relname, null from pg_class where (relkind = '%1') "
                  "and (relname !~ '^Inv') "
                  "and (relname !~ '^pg_') ").arg(type);
    }
    t.exec(query);
    while (t.next()) {
        QString schema = t.value(1).toString();
        if (schema.isEmpty() || schema == QLatin1String("public"))
            tl.append(t.value(0).toString());
        else
            tl.append(t.value(0).toString().prepend(QLatin1Char('.')).prepend(schema));
    }
}

QSqlError QPSQLDriver::makeError(const QString& err, QSqlError::ErrorType type) const
{
    const char *s = PQerrorMessage(connection);
    QString msg = isUtf8 ? QString::fromUtf8(s) : QString::fromLocal8Bit(s);
    return QSqlError(QLatin1String("QPSQL: ") + err, msg, type);
}

QSqlIndex QPSQLDriver::primaryIndex(const QString& tablename) const
{
    QSqlIndex idx(tablename);
    if (!isOpen())
        return idx;
    QSqlQuery i(createResult());
    QString stmt;

    QString tbl = tablename;
    QString schema;
    qSplitTableName(tbl, schema);

    if (isIdentifierEscaped(tbl, QSqlDriver::TableName))
        tbl = stripDelimiters(tbl, QSqlDriver::TableName);
    else
        tbl = tbl.toLower();

    if (isIdentifierEscaped(schema, QSqlDriver::TableName))
        schema = stripDelimiters(schema, QSqlDriver::TableName);
    else
        schema = schema.toLower();

    switch(protocol) {
    case QPSQLDriver::Version6:
        stmt = QLatin1String("select pg_att1.attname, int(pg_att1.atttypid), pg_cl.relname "
                "from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
                "where pg_cl.relname = '%1_pkey' "
                "and pg_cl.oid = pg_ind.indexrelid "
                "and pg_att2.attrelid = pg_ind.indexrelid "
                "and pg_att1.attrelid = pg_ind.indrelid "
                "and pg_att1.attnum = pg_ind.indkey[pg_att2.attnum-1] "
                "order by pg_att2.attnum");
        break;
    case QPSQLDriver::Version7:
    case QPSQLDriver::Version71:
        stmt = QLatin1String("select pg_att1.attname, pg_att1.atttypid::int, pg_cl.relname "
                "from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
                "where pg_cl.relname = '%1_pkey' "
                "and pg_cl.oid = pg_ind.indexrelid "
                "and pg_att2.attrelid = pg_ind.indexrelid "
                "and pg_att1.attrelid = pg_ind.indrelid "
                "and pg_att1.attnum = pg_ind.indkey[pg_att2.attnum-1] "
                "order by pg_att2.attnum");
        break;
    case QPSQLDriver::Version73:
    case QPSQLDriver::Version74:
    case QPSQLDriver::Version8:
    case QPSQLDriver::Version81:
    case QPSQLDriver::Version82:
        stmt = QLatin1String("SELECT pg_attribute.attname, pg_attribute.atttypid::int, "
                "pg_class.relname "
                "FROM pg_attribute, pg_class "
                "WHERE %1 pg_class.oid IN "
                "(SELECT indexrelid FROM pg_index WHERE indisprimary = true AND indrelid IN "
                " (SELECT oid FROM pg_class WHERE relname = '%2')) "
                "AND pg_attribute.attrelid = pg_class.oid "
                "AND pg_attribute.attisdropped = false "
                "ORDER BY pg_attribute.attnum");
        if (schema.isEmpty())
            stmt = stmt.arg(QLatin1String("pg_table_is_visible(pg_class.oid) AND"));
        else
            stmt = stmt.arg(QString::fromLatin1("pg_class.relnamespace = (select oid from "
                   "pg_namespace where pg_namespace.nspname = '%1') AND ").arg(schema));
        break;
    }

    i.exec(stmt.arg(tbl));
    while (i.isActive() && i.next()) {
        QSqlField f(i.value(0).toString(), decodePGType(i.value(1).toInt()));
        idx.append(f);
        idx.setName(i.value(2).toString());
    }
    return idx;
}

QSqlRecord QPSQLDriver::record(const QString& tablename) const
{
    QSqlRecord info;
    if (!isOpen())
        return info;

    QString tbl = tablename;
    QString schema;
    qSplitTableName(tbl, schema);

    if (isIdentifierEscaped(tbl, QSqlDriver::TableName))
        tbl = stripDelimiters(tbl, QSqlDriver::TableName);
    else
        tbl = tbl.toLower();

    if (isIdentifierEscaped(schema, QSqlDriver::TableName))
        schema = stripDelimiters(schema, QSqlDriver::TableName);
    else
        schema = schema.toLower();

    QString stmt;
    switch(protocol) {
    case QPSQLDriver::Version6:
        stmt = QLatin1String("select pg_attribute.attname, int(pg_attribute.atttypid), "
                "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                "int(pg_attribute.attrelid), pg_attribute.attnum "
                "from pg_class, pg_attribute "
                "where pg_class.relname = '%1' "
                "and pg_attribute.attnum > 0 "
                "and pg_attribute.attrelid = pg_class.oid ");
        break;
    case QPSQLDriver::Version7:
        stmt = QLatin1String("select pg_attribute.attname, pg_attribute.atttypid::int, "
                "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                "pg_attribute.attrelid::int, pg_attribute.attnum "
                "from pg_class, pg_attribute "
                "where pg_class.relname = '%1' "
                "and pg_attribute.attnum > 0 "
                "and pg_attribute.attrelid = pg_class.oid ");
        break;
    case QPSQLDriver::Version71:
        stmt = QLatin1String("select pg_attribute.attname, pg_attribute.atttypid::int, "
                "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                "pg_attrdef.adsrc "
                "from pg_class, pg_attribute "
                "left join pg_attrdef on (pg_attrdef.adrelid = "
                "pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
                "where pg_class.relname = '%1' "
                "and pg_attribute.attnum > 0 "
                "and pg_attribute.attrelid = pg_class.oid "
                "order by pg_attribute.attnum ");
        break;
    case QPSQLDriver::Version73:
    case QPSQLDriver::Version74:
    case QPSQLDriver::Version8:
    case QPSQLDriver::Version81:
    case QPSQLDriver::Version82:
        stmt = QLatin1String("select pg_attribute.attname, pg_attribute.atttypid::int, "
                "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                "pg_attrdef.adsrc "
                "from pg_class, pg_attribute "
                "left join pg_attrdef on (pg_attrdef.adrelid = "
                "pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
                "where %1 "
                "and pg_class.relname = '%2' "
                "and pg_attribute.attnum > 0 "
                "and pg_attribute.attrelid = pg_class.oid "
                "and pg_attribute.attisdropped = false "
                "order by pg_attribute.attnum ");
        if (schema.isEmpty())
            stmt = stmt.arg(QLatin1String("pg_table_is_visible(pg_class.oid)"));
        else
            stmt = stmt.arg(QString::fromLatin1("pg_class.relnamespace = (select oid from "
                   "pg_namespace where pg_namespace.nspname = '%1')").arg(schema));
        break;
    }

    QSqlQuery query(createResult());
    query.exec(stmt.arg(tbl));
    if (protocol >= QPSQLDriver::Version71) {
        while (query.next()) {
            int len = query.value(3).toInt();
            int precision = query.value(4).toInt();
            // swap length and precision if length == -1
            if (len == -1 && precision > -1) {
                len = precision - 4;
                precision = -1;
            }
            QString defVal = query.value(5).toString();
            if (!defVal.isEmpty() && defVal.at(0) == QLatin1Char('\''))
                defVal = defVal.mid(1, defVal.length() - 2);
            QSqlField f(query.value(0).toString(), decodePGType(query.value(1).toInt()));
            f.setRequired(query.value(2).toBool());
            f.setLength(len);
            f.setPrecision(precision);
            f.setDefaultValue(defVal);
            f.setSqlType(query.value(1).toInt());
            info.append(f);
        }
    } else {
        // Postgres < 7.1 cannot handle outer joins
        while (query.next()) {
            QString defVal;
            QString stmt2 = QLatin1String("select pg_attrdef.adsrc from pg_attrdef where "
                            "pg_attrdef.adrelid = %1 and pg_attrdef.adnum = %2 ");
            QSqlQuery query2(createResult());
            query2.exec(stmt2.arg(query.value(5).toInt()).arg(query.value(6).toInt()));
            if (query2.isActive() && query2.next())
                defVal = query2.value(0).toString();
            if (!defVal.isEmpty() && defVal.at(0) == QLatin1Char('\''))
                defVal = defVal.mid(1, defVal.length() - 2);
            int len = query.value(3).toInt();
            int precision = query.value(4).toInt();
            // swap length and precision if length == -1
            if (len == -1 && precision > -1) {
                len = precision - 4;
                precision = -1;
            }
            QSqlField f(query.value(0).toString(), decodePGType(query.value(1).toInt()));
            f.setRequired(query.value(2).toBool());
            f.setLength(len);
            f.setPrecision(precision);
            f.setDefaultValue(defVal);
            f.setSqlType(query.value(1).toInt());
            info.append(f);
        }
    }

    return info;
}

QString QPSQLDriver::formatValue(const QSqlField &field, bool trimStrings) const
{
    QString r;
    if (field.isNull()) {
        r = QLatin1String("NULL");
    } else {
        switch (field.type()) {
        case QVariant::DateTime:
#ifndef QT_NO_DATESTRING
            if (field.value().toDateTime().isValid()) {
                QDate dt = field.value().toDateTime().date();
                QTime tm = field.value().toDateTime().time();
                // msecs need to be right aligned otherwise psql
                // interpretes them wrong
                r = QLatin1Char('\'') + QString::number(dt.year()) + QLatin1Char('-')
                          + QString::number(dt.month()) + QLatin1Char('-')
                          + QString::number(dt.day()) + QLatin1Char(' ')
                          + tm.toString() + QLatin1Char('.')
                          + QString::number(tm.msec()).rightJustified(3, QLatin1Char('0'))
                          + QLatin1Char('\'');
            } else {
                r = QLatin1String("NULL");
            }
#else
            r = QLatin1String("NULL");
#endif // QT_NO_DATESTRING
            break;
        case QVariant::Time:
#ifndef QT_NO_DATESTRING
            if (field.value().toTime().isValid()) {
                r = QLatin1Char('\'') + field.value().toTime().toString(QLatin1String("hh:mm:ss.zzz")) + QLatin1Char('\'');
            } else
#endif
            {
                r = QLatin1String("NULL");
            }
            break;
        case QVariant::String:
        {
            // Escape '\' characters
            r = QSqlDriver::formatValue(field, trimStrings);
            r.replace(QLatin1String("\\"), QLatin1String("\\\\"));
            break;
        }
        case QVariant::Bool:
            if (field.value().toBool())
                r = QLatin1String("TRUE");
            else
                r = QLatin1String("FALSE");
            break;
        case QVariant::ByteArray: {
            QByteArray ba(field.value().toByteArray());
            size_t len;
#if defined PG_VERSION_NUM && PG_VERSION_NUM-0 >= 80200
            unsigned char *data = PQescapeByteaConn(connection, (unsigned char*)ba.constData(), ba.size(), &len);
#else
            unsigned char *data = PQescapeBytea((unsigned char*)ba.constData(), ba.size(), &len);
#endif
            r += QLatin1Char('\'');
            r += QLatin1String((const char*)data);
            r += QLatin1Char('\'');
            qPQfreemem(data);
            break;
        }
        case QVariant::Double: {
            double val = field.value().toDouble();
            if (isnan(val))
                r = QLatin1String("'NaN'");
            else {
                int res = isinf(val);
                if (res == 1)
                    r = QLatin1String("'Infinity'");
                else if (res == -1)
                    r = QLatin1String("'-Infinity'");
                else
                    r = QSqlDriver::formatValue(field, trimStrings);
            }
            break;
        }
        default:
            r = QSqlDriver::formatValue(field, trimStrings);
            break;
        }
    }
    return r;
}

QString QPSQLDriver::escapeIdentifier(const QString &identifier, IdentifierType) const
{
    QString res = identifier;
    if (!identifier.isEmpty() && !identifier.startsWith(QLatin1Char('"')) && !identifier.endsWith(QLatin1Char('"')) ) {
        res.replace(QLatin1Char('"'), QLatin1String("\"\""));
        res.prepend(QLatin1Char('"')).append(QLatin1Char('"'));
        res.replace(QLatin1Char('.'), QLatin1String("\".\""));
    }
    return res;
}

QString QPSQLDriver::createParamString(const QVector<QVariant> boundValues) const
{
    if (boundValues.isEmpty())
        return QString();

    QString params;
    QSqlField f;
    for (int i = 0; i < boundValues.count(); ++i) {
        const QVariant &val = boundValues.at(i);

        f.setType(val.type());
        if (val.isNull())
            f.clear();
        else
            f.setValue(val);

        if (!params.isNull())
            params.append(QLatin1String(", "));

        params.append(formatValue(f));
    }
    return params;
}

QString QPSQLDriver::newStmtId() const
{
    mutex->lock();
    static unsigned int preparedStmtCount = 0;
    QString id = QLatin1String("qpsqlpstmt_") + QString::number(++preparedStmtCount, 16);
    mutex->unlock();
    return id;
}

void QPSQLDriver::deallocate(QString &stmtId) const
{
    if (stmtId.isEmpty())
        return;

    const QString stmt = QLatin1String("DEALLOCATE ") + stmtId;
    PGresult *result = exec(stmt);

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
        qWarning("Unable to free statement: %s", PQerrorMessage(connection));

    PQclear(result);
    stmtId.clear();
}

QVariant::Type QPSQLDriver::decodePGType(const int pgType) const
{
    QVariant::Type type = QVariant::Invalid;
    switch (pgType) {
    case QBOOLOID:
        type = QVariant::Bool;
        break;
    case QINT8OID:
        type = QVariant::LongLong;
        break;
    case QINT2OID:
    case QINT4OID:
    case QOIDOID:
    case QREGPROCOID:
    case QXIDOID:
    case QCIDOID:
        type = QVariant::Int;
        break;
    case QNUMERICOID:
    case QFLOAT4OID:
    case QFLOAT8OID:
        type = QVariant::Double;
        break;
    case QABSTIMEOID:
    case QRELTIMEOID:
    case QDATEOID:
        type = QVariant::Date;
        break;
    case QTIMEOID:
    case QTIMETZOID:
        type = QVariant::Time;
        break;
    case QTIMESTAMPOID:
    case QTIMESTAMPTZOID:
        type = QVariant::DateTime;
        break;
    case QBYTEAOID:
        type = QVariant::ByteArray;
        break;
    default:
        type = QVariant::String;
        break;
    }
    return type;
}

bool QPSQLDriver::isOpen() const
{
    if(!connection) return false;
    return PQstatus(connection) == CONNECTION_OK;
}

bool QPSQLDriver::subscribeToNotificationImplementation(const QString &name)
{
    if (!isOpen()) {
        qWarning("QPSQLDriver::subscribeToNotificationImplementation: database not open.");
        return false;
    }

    if (seid.contains(name)) {
        qWarning("QPSQLDriver::subscribeToNotificationImplementation: already subscribing to '%s'.",
            qPrintable(name));
        return false;
    }

    int socket = PQsocket(connection);
    if (socket) {
        QString query = QLatin1String("LISTEN ") + escapeIdentifier(name, QSqlDriver::TableName);
        if (PQresultStatus(PQexec(connection,
                                  isUtf8 ? query.toUtf8().constData()
                                            : query.toLocal8Bit().constData())
                          ) != PGRES_COMMAND_OK) {
            setLastError(makeError(tr("Unable to subscribe"), QSqlError::StatementError));
            return false;
        }

        if (!socketNotifier) {
            socketNotifier = new QSocketNotifier(socket, QSocketNotifier::Read);
            connect(socketNotifier, SIGNAL(activated(int)), this, SLOT(_q_handleNotification(int)));
        }
    }

    seid << name;
    return true;
}

bool QPSQLDriver::unsubscribeFromNotificationImplementation(const QString &name)
{
    if (!isOpen()) {
        qWarning("QPSQLDriver::unsubscribeFromNotificationImplementation: database not open.");
        return false;
    }

    if (!seid.contains(name)) {
        qWarning("QPSQLDriver::unsubscribeFromNotificationImplementation: not subscribed to '%s'.",
            qPrintable(name));
        return false;
    }

    QString query = QLatin1String("UNLISTEN ") + escapeIdentifier(name, QSqlDriver::TableName);
    if (PQresultStatus(PQexec(connection,
                              isUtf8 ? query.toUtf8().constData()
                                     : query.toLocal8Bit().constData())
                      ) != PGRES_COMMAND_OK) {
        setLastError(makeError(tr("Unable to unsubscribe"), QSqlError::StatementError));
        return false;
    }

    seid.removeAll(name);

    if (seid.isEmpty()) {
        disconnect(socketNotifier, SIGNAL(activated(int)), this, SLOT(_q_handleNotification(int)));
        delete socketNotifier;
        socketNotifier = 0;
    }

    return true;
}

QStringList QPSQLDriver::subscribedToNotificationsImplementation() const
{
    return seid;
}

void QPSQLDriver::_q_handleNotification(int)
{
    PQconsumeInput(connection);

    PGnotify *notify = 0;
    while ((notify = PQnotifies(connection)) != 0) {
        QString name(QLatin1String(notify->relname));
        if (seid.contains(name))
            emit notification(name);
        else
            qWarning("QPSQLDriver: received notification for '%s' which isn't subscribed to.",
                    qPrintable(name));

        qPQfreemem(notify);
    }
}

QT_END_NAMESPACE
