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
#include <qhash.h>
#include <qbytearray.h>
#include <qcryptographichash.h>
#include <qhttp.h>
#include <qiodevice.h>
#include <qdatastream.h>
#include <qendian.h>
#include <qstring.h>
#include <qdatetime.h>



QT_BEGIN_NAMESPACE


/*!
  \class QAuthenticator
  \brief The QAuthenticator class provides an authentication object.
  \since 4.3

  \reentrant
  \ingroup network
  \inmodule QtNetwork

  The QAuthenticator class is usually used in the
  \l{QNetworkAccessManager::}{authenticationRequired()} and
  \l{QNetworkAccessManager::}{proxyAuthenticationRequired()} signals of QNetworkAccessManager and
  QAbstractSocket. The class provides a way to pass back the required
  authentication information to the socket when accessing services that
  require authentication.

  QAuthenticator supports the following authentication methods:
  \list
    \o Basic
    \o NTLM version 1
    \o Digest-MD5
  \endlist

  Note that, in particular, NTLM version 2 is not supported.

  \section1 Options

  In addition to the username and password required for authentication, a
  QAuthenticator object can also contain additional options. The
  options() function can be used to query incoming options sent by
  the server; the setOption() function can
  be used to set outgoing options, to be processed by the authenticator
  calculation. The options accepted and provided depend on the authentication
  type (see method()).

  The following tables list known incoming options as well as accepted
  outgoing options. The list of incoming options is not exhaustive, since
  servers may include additional information at any time. The list of
  outgoing options is exhaustive, however, and no unknown options will be
  treated or sent back to the server.

  \section2 Basic

  \table
    \header \o Option \o Direction \o Description
    \row \o \tt{realm} \o Incoming \o Contains the realm of the authentication, the same as realm()
  \endtable

  The Basic authentication mechanism supports no outgoing options.

  \section2 NTLM version 1

  The NTLM authentication mechanism currently supports no incoming or outgoing options.

  \section2 Digest-MD5

  \table
    \header \o Option \o Direction \o Description
    \row \o \tt{realm} \o Incoming \o Contains the realm of the authentication, the same as realm()
  \endtable

  The Digest-MD5 authentication mechanism supports no outgoing options.

  \sa QSslSocket
*/


/*!
  Constructs an empty authentication object
*/
QAuthenticator::QAuthenticator()
    : d(0)
{
}

/*!
  Destructs the object
*/
QAuthenticator::~QAuthenticator()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Constructs a copy of \a other.
*/
QAuthenticator::QAuthenticator(const QAuthenticator &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

/*!
    Assigns the contents of \a other to this authenticator.
*/
QAuthenticator &QAuthenticator::operator=(const QAuthenticator &other)
{
    if (d == other.d)
        return *this;

    if (d && !d->ref.deref())
        delete d;

    d = other.d;
    if (d)
        d->ref.ref();
    return *this;
}

/*!
    Returns true if this authenticator is identical to \a other; otherwise
    returns false.
*/
bool QAuthenticator::operator==(const QAuthenticator &other) const
{
    if (d == other.d)
        return true;
    return d->user == other.d->user
        && d->password == other.d->password
        && d->realm == other.d->realm
        && d->method == other.d->method
        && d->options == other.d->options;
}

/*!
    \fn bool QAuthenticator::operator!=(const QAuthenticator &other) const

    Returns true if this authenticator is different from \a other; otherwise
    returns false.
*/

/*!
  returns the user used for authentication.
*/
QString QAuthenticator::user() const
{
    return d ? d->user : QString();
}

/*!
  Sets the \a user used for authentication.
*/
void QAuthenticator::setUser(const QString &user)
{
    detach();
    int separatorPosn = 0;

    switch(d->method) {
    case QAuthenticatorPrivate::Ntlm:
        if((separatorPosn = user.indexOf(QLatin1String("\\"))) != -1) {
            //domain name is present
            d->realm.clear();
            d->userDomain = user.left(separatorPosn);
            d->extractedUser = user.mid(separatorPosn + 1);
            d->user = user;
        } else if((separatorPosn = user.indexOf(QLatin1String("@"))) != -1) {
            //domain name is present
            d->realm.clear();
            d->userDomain = user.left(separatorPosn);
            d->extractedUser = user.left(separatorPosn);
            d->user = user;
        } else {
            d->extractedUser = user;
            d->user = user;
            d->realm.clear();
            d->userDomain.clear();
        }
        break;
    default:
        d->user = user;
        d->userDomain.clear();
        break;
    }
}

/*!
  returns the password used for authentication.
*/
QString QAuthenticator::password() const
{
    return d ? d->password : QString();
}

/*!
  Sets the \a password used for authentication.
*/
void QAuthenticator::setPassword(const QString &password)
{
    detach();
    d->password = password;
}

/*!
  \internal
*/
void QAuthenticator::detach()
{
    if (!d) {
        d = new QAuthenticatorPrivate;
        d->ref = 1;
        return;
    }

    qAtomicDetach(d);
    d->phase = QAuthenticatorPrivate::Start;
}

/*!
  returns the realm requiring authentication.
*/
QString QAuthenticator::realm() const
{
    return d ? d->realm : QString();
}

/*!
    \since 4.7
    Returns the value related to option \a opt if it was set by the server.
    See \l{QAuthenticator#Options} for more information on incoming options.
    If option \a opt isn't found, an invalid QVariant will be returned.

    \sa options(), QAuthenticator#Options
*/
QVariant QAuthenticator::option(const QString &opt) const
{
    return d ? d->options.value(opt) : QVariant();
}

/*!
    \since 4.7
    Returns all incoming options set in this QAuthenticator object by parsing
    the server reply. See \l{QAuthenticator#Options} for more information
    on incoming options.

    \sa option(), QAuthenticator#Options
*/
QVariantHash QAuthenticator::options() const
{
    return d ? d->options : QVariantHash();
}

/*!
    \since 4.7

    Sets the outgoing option \a opt to value \a value.
    See \l{QAuthenticator#Options} for more information on outgoing options.

    \sa options(), option(), QAuthenticator#Options
*/
void QAuthenticator::setOption(const QString &opt, const QVariant &value)
{
    detach();
    d->options.insert(opt, value);
}


/*!
    Returns true if the authenticator is null.
*/
bool QAuthenticator::isNull() const
{
    return !d;
}

QAuthenticatorPrivate::QAuthenticatorPrivate()
    : ref(0)
    , method(None)
    , phase(Start)
    , nonceCount(0)
{
    cnonce = QCryptographicHash::hash(QByteArray::number(qrand(), 16) + QByteArray::number(qrand(), 16),
                                      QCryptographicHash::Md5).toHex();
    nonceCount = 0;
}

#ifndef QT_NO_HTTP
void QAuthenticatorPrivate::parseHttpResponse(const QHttpResponseHeader &header, bool isProxy)
{
    const QList<QPair<QString, QString> > values = header.values();
    QList<QPair<QByteArray, QByteArray> > rawValues;

    QList<QPair<QString, QString> >::const_iterator it, end;
    for (it = values.constBegin(), end = values.constEnd(); it != end; ++it)
        rawValues.append(qMakePair(it->first.toLatin1(), it->second.toUtf8()));

    // continue in byte array form
    parseHttpResponse(rawValues, isProxy);
}
#endif

void QAuthenticatorPrivate::parseHttpResponse(const QList<QPair<QByteArray, QByteArray> > &values, bool isProxy)
{
    const char *search = isProxy ? "proxy-authenticate" : "www-authenticate";

    method = None;
    /*
      Fun from the HTTP 1.1 specs, that we currently ignore:

      User agents are advised to take special care in parsing the WWW-
      Authenticate field value as it might contain more than one challenge,
      or if more than one WWW-Authenticate header field is provided, the
      contents of a challenge itself can contain a comma-separated list of
      authentication parameters.
    */

    QByteArray headerVal;
    for (int i = 0; i < values.size(); ++i) {
        const QPair<QByteArray, QByteArray> &current = values.at(i);
        if (current.first.toLower() != search)
            continue;
        QByteArray str = current.second.toLower();
        if (method < Basic && str.startsWith("basic")) {
            method = Basic;
            headerVal = current.second.mid(6);
        } else if (method < Ntlm && str.startsWith("ntlm")) {
            method = Ntlm;
            headerVal = current.second.mid(5);
        } else if (method < DigestMd5 && str.startsWith("digest")) {
            method = DigestMd5;
            headerVal = current.second.mid(7);
        }
    }

    challenge = headerVal.trimmed();
    QHash<QByteArray, QByteArray> options = parseDigestAuthenticationChallenge(challenge);

    switch(method) {
    case Basic:
        if(realm.isEmpty())
            this->options[QLatin1String("realm")] = realm = QString::fromLatin1(options.value("realm"));
        if (user.isEmpty() && password.isEmpty())
            phase = Done;
        break;
    case Ntlm:
        // #### extract from header
        break;
    case DigestMd5: {
        if(realm.isEmpty())
            this->options[QLatin1String("realm")] = realm = QString::fromLatin1(options.value("realm"));
        if (options.value("stale").toLower() == "true")
            phase = Start;
        if (user.isEmpty() && password.isEmpty())
            phase = Done;
        break;
    }
    default:
        realm.clear();
        challenge = QByteArray();
        phase = Invalid;
    }
}

QByteArray QAuthenticatorPrivate::calculateResponse(const QByteArray &requestMethod, const QByteArray &path)
{
    QByteArray response;
    const char *methodString = 0;
    switch(method) {
    case QAuthenticatorPrivate::None:
        methodString = "";
        phase = Done;
        break;
    case QAuthenticatorPrivate::Plain:
        response = '\0' + user.toUtf8() + '\0' + password.toUtf8();
        phase = Done;
        break;
    case QAuthenticatorPrivate::Basic:
        methodString = "Basic ";
        response = user.toLatin1() + ':' + password.toLatin1();
        response = response.toBase64();
        phase = Done;
        break;
    case QAuthenticatorPrivate::Login:
        if (challenge.contains("VXNlciBOYW1lAA==")) {
            response = user.toUtf8().toBase64();
            phase = Phase2;
        } else if (challenge.contains("UGFzc3dvcmQA")) {
            response = password.toUtf8().toBase64();
            phase = Done;
        }
        break;
    case QAuthenticatorPrivate::CramMd5:
        break;
    case QAuthenticatorPrivate::DigestMd5:
        methodString = "Digest ";
        response = digestMd5Response(challenge, requestMethod, path);
        phase = Done;
        break;
    case QAuthenticatorPrivate::Ntlm:
        methodString = "NTLM ";
        if (challenge.isEmpty()) {
            response = this->ntlmPhase1().toBase64();
			//BUGFIX NTLMv2 (QTBUG-17322)
            /*if (user.isEmpty())
                phase = Done;
            else*/
                phase = Phase2;
        } else {
            response = this->ntlmPhase3(this, QByteArray::fromBase64(challenge)).toBase64();
            phase = Done;
        }

        break;
    }
    return QByteArray(methodString) + response;
}


// ---------------------------- Digest Md5 code ----------------------------------------

QHash<QByteArray, QByteArray> QAuthenticatorPrivate::parseDigestAuthenticationChallenge(const QByteArray &challenge)
{
    QHash<QByteArray, QByteArray> options;
    // parse the challenge
    const char *d = challenge.constData();
    const char *end = d + challenge.length();
    while (d < end) {
        while (d < end && (*d == ' ' || *d == '\n' || *d == '\r'))
            ++d;
        const char *start = d;
        while (d < end && *d != '=')
            ++d;
        QByteArray key = QByteArray(start, d - start);
        ++d;
        if (d >= end)
            break;
        bool quote = (*d == '"');
        if (quote)
            ++d;
        if (d >= end)
            break;
        start = d;
        QByteArray value;
        while (d < end) {
            bool backslash = false;
            if (*d == '\\' && d < end - 1) {
                ++d;
                backslash = true;
            }
            if (!backslash) {
                if (quote) {
                    if (*d == '"')
                        break;
                } else {
                    if (*d == ',')
                        break;
                }
            }
            value += *d;
            ++d;
        }
        while (d < end && *d != ',')
            ++d;
        ++d;
        options[key] = value;
    }

    QByteArray qop = options.value("qop");
    if (!qop.isEmpty()) {
        QList<QByteArray> qopoptions = qop.split(',');
        if (!qopoptions.contains("auth"))
            return QHash<QByteArray, QByteArray>();
        // #### can't do auth-int currently
//         if (qop.contains("auth-int"))
//             qop = "auth-int";
//         else if (qop.contains("auth"))
//             qop = "auth";
//         else
//             qop = QByteArray();
        options["qop"] = "auth";
    }

    return options;
}

/*
  Digest MD5 implementation

  Code taken from RFC 2617

  Currently we don't support the full SASL authentication mechanism (which includes cyphers)
*/


/* calculate request-digest/response-digest as per HTTP Digest spec */
static QByteArray digestMd5ResponseHelper(
    const QByteArray &alg,
    const QByteArray &userName,
    const QByteArray &realm,
    const QByteArray &password,
    const QByteArray &nonce,       /* nonce from server */
    const QByteArray &nonceCount,  /* 8 hex digits */
    const QByteArray &cNonce,      /* client nonce */
    const QByteArray &qop,         /* qop-value: "", "auth", "auth-int" */
    const QByteArray &method,      /* method from the request */
    const QByteArray &digestUri,   /* requested URL */
    const QByteArray &hEntity       /* H(entity body) if qop="auth-int" */
    )
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(userName);
    hash.addData(":", 1);
    hash.addData(realm);
    hash.addData(":", 1);
    hash.addData(password);
    QByteArray ha1 = hash.result();
    if (alg.toLower() == "md5-sess") {
        hash.reset();
        // RFC 2617 contains an error, it was:
        // hash.addData(ha1);
        // but according to the errata page at http://www.rfc-editor.org/errata_list.php, ID 1649, it
        // must be the following line:
        hash.addData(ha1.toHex());
        hash.addData(":", 1);
        hash.addData(nonce);
        hash.addData(":", 1);
        hash.addData(cNonce);
        ha1 = hash.result();
    };
    ha1 = ha1.toHex();

    // calculate H(A2)
    hash.reset();
    hash.addData(method);
    hash.addData(":", 1);
    hash.addData(digestUri);
    if (qop.toLower() == "auth-int") {
        hash.addData(":", 1);
        hash.addData(hEntity);
    }
    QByteArray ha2hex = hash.result().toHex();

    // calculate response
    hash.reset();
    hash.addData(ha1);
    hash.addData(":", 1);
    hash.addData(nonce);
    hash.addData(":", 1);
    if (!qop.isNull()) {
        hash.addData(nonceCount);
        hash.addData(":", 1);
        hash.addData(cNonce);
        hash.addData(":", 1);
        hash.addData(qop);
        hash.addData(":", 1);
    }
    hash.addData(ha2hex);
    return hash.result().toHex();
}

QByteArray QAuthenticatorPrivate::digestMd5Response(const QByteArray &challenge, const QByteArray &method, const QByteArray &path)
{
    QHash<QByteArray,QByteArray> options = parseDigestAuthenticationChallenge(challenge);

    ++nonceCount;
    QByteArray nonceCountString = QByteArray::number(nonceCount, 16);
    while (nonceCountString.length() < 8)
        nonceCountString.prepend('0');

    QByteArray nonce = options.value("nonce");
    QByteArray opaque = options.value("opaque");
    QByteArray qop = options.value("qop");

//    qDebug() << "calculating digest: method=" << method << "path=" << path;
    QByteArray response = digestMd5ResponseHelper(options.value("algorithm"), user.toLatin1(),
                                              realm.toLatin1(), password.toLatin1(),
                                              nonce, nonceCountString,
                                              cnonce, qop, method,
                                              path, QByteArray());


    QByteArray credentials;
    credentials += "username=\"" + user.toLatin1() + "\", ";
    credentials += "realm=\"" + realm.toLatin1() + "\", ";
    credentials += "nonce=\"" + nonce + "\", ";
    credentials += "uri=\"" + path + "\", ";
    if (!opaque.isEmpty())
        credentials += "opaque=\"" + opaque + "\", ";
    credentials += "response=\"" + response + '\"';
    if (!options.value("algorithm").isEmpty())
        credentials += ", algorithm=" + options.value("algorithm");
    if (!options.value("qop").isEmpty()) {
        credentials += ", qop=" + qop + ", ";
        credentials += "nc=" + nonceCountString + ", ";
        credentials += "cnonce=\"" + cnonce + '\"';
    }

    return credentials;
}

// ---------------------------- Digest Md5 code ----------------------------------------






QT_END_NAMESPACE
