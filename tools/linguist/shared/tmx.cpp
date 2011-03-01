/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

/****************************************************************************
** Basic importer and exporter for Translation Memory eXchange format (TMX).
** The specification and DTD can be found here:
**
** http://www.lisa.org/tmx/tmx14.dtd
** http://www.lisa.org/fileadmin/standards/tmx1.4/tmx.htm
****************************************************************************/

#include "translator.h"

#include <QtCore/QByteArray>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include <QtXml/QXmlStreamReader>
#include <QtXml/QXmlStreamAttribute>

#include <stdio.h>

QT_BEGIN_NAMESPACE

class TMXReader : public QXmlStreamReader
{
public:
    TMXReader(QIODevice &dev, ConversionData &cd)
        : QXmlStreamReader(&dev)
        , m_cd(cd)
    {}

    // the "real thing"
    bool read(Translator &translator);

private:
    void readTMXElement(Translator &translator);
    void readBodyElement(Translator &translator);
    void readTuElement(Translator &translator);
    void readTuvElement();
    void readSegElement();
    bool getNextElement();
    void skipUnknownElement();

    enum DataField { NoField, SourceField, TargetField, DefinitionField };
    enum LangSet { NothingSet, SourceSet, BothSet };
    DataField m_currentField;
    LangSet m_languageState;
    QString m_currentSource;
    QString m_currentTarget;
    QString m_currentDefinition;
    ConversionData &m_cd;
};

bool TMXReader::read(Translator &translator)
{
    m_currentField = NoField;
    m_languageState = NothingSet;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("tmx"))
                readTMXElement(translator);
            else
                raiseError(QString::fromLatin1("Not a TMX file"));
        }
    }
    if (hasError()) {
        m_cd.appendError(QString::fromLatin1("Failed to parse file: %1").arg(errorString()));
        return false;
    }
    return true;
}

void TMXReader::readTMXElement(Translator &translator)
{
    while (!atEnd()) {
        if (getNextElement())
            break;
        if (isStartElement()) {
            if (name() == "body")
                readBodyElement(translator);
            else
                skipUnknownElement();
        }
    }
}

void TMXReader::readBodyElement(Translator &translator)
{
    while (!atEnd()) {
        if (getNextElement())
            break;
        if (isStartElement()) {
            if (name() == "tu")
                readTuElement(translator);
            else
                skipUnknownElement();
        }
    }
}

void TMXReader::readTuElement(Translator &translator)
{
    while (!atEnd()) {
        if (getNextElement())
            break;
        if (isStartElement()) {
            if (name() == "tuv") {
                QString lang = QLatin1String("xml:lang");
                if (attributes().hasAttribute(lang)) {
                    QString langAttr = attributes().value(lang).toString();
                    /*
                    *  A TMX file can contain more than one translation for
                    *  an expression. To keep it simple this code just parses
                    *  the first language found for the rest of the document.
                    */
                    switch (m_languageState) {
                    case NothingSet:
                        translator.setSourceLanguageCode(langAttr);
                        m_languageState = SourceSet;
                        break;
                    case SourceSet:
                        translator.setLanguageCode(langAttr);
                        m_languageState = BothSet;
                        break;
                    case BothSet:
                        break;
                    }

                    if (!langAttr.compare(translator.sourceLanguageCode())) {
                        m_currentField = SourceField;
                        readTuvElement();
                    } else if (!langAttr.compare(translator.languageCode())) {
                        m_currentField = TargetField;
                        readTuvElement();
                    } else {
                        skipUnknownElement();
                    }
                }
            } else {
                skipUnknownElement();
            }
        }
    }

    if (!m_currentTarget.isEmpty() && !m_currentSource.isEmpty()) {
        TranslatorMessage msg;
        msg.setSourceText(m_currentSource);
        msg.setTranslation(m_currentTarget);
        msg.setComment(m_currentDefinition);
        translator.append(msg);
    } else {
        fprintf(stderr, "The following entry was dropped: ");
        if (m_currentTarget.isEmpty())
            fprintf(stderr, "no translation for: %s\n", m_currentSource.toStdString().c_str());
        else
            fprintf(stderr, "no source language entry for: %s\n", m_currentTarget.toStdString().c_str());
    }

    m_currentSource.clear();
    m_currentTarget.clear();
    m_currentDefinition.clear();
}

void TMXReader::readTuvElement()
{
    while (!atEnd()) {
        if (getNextElement())
            break;
        if (isStartElement()) {
            if (name() == "seg")
                readSegElement();
            else
                skipUnknownElement();
        }
    }
}

void TMXReader::readSegElement()
{
    if ( m_currentField == SourceField )
        m_currentSource = readElementText(IncludeChildElements);
    else if (m_currentField == TargetField)
        m_currentTarget = readElementText(IncludeChildElements);
}

bool TMXReader::getNextElement()
{
    readNext();
    if (isEndElement())
        return true;

    return false;
}

void TMXReader::skipUnknownElement()
{
    while (!atEnd()) {
        if (getNextElement())
            break;
        if (isStartElement())
            skipUnknownElement();
    }
}

static bool loadTMX(Translator &translator, QIODevice &dev, ConversionData &cd)
{
    translator.setLocationsType(Translator::NoLocations);
    TMXReader reader(dev, cd);
    return reader.read(translator);
}

// TODO: this function is duplicated so many times now that it may be worth factoring it out
static QString protect(const QString &str)
{
    QString result;
    result.reserve(str.length() * 12 / 10);
    for (int i = 0; i != str.size(); ++i) {
        uint c = str.at(i).unicode();
        switch (c) {
        case '\"':
            result += QLatin1String("&quot;");
            break;
        case '&':
            result += QLatin1String("&amp;");
            break;
        case '>':
            result += QLatin1String("&gt;");
            break;
        case '<':
            result += QLatin1String("&lt;");
            break;
        case '\'':
            result += QLatin1String("&apos;");
            break;
        default:
            if (c < 0x20 && c != '\r' && c != '\n' && c != '\t')
                result += QString(QLatin1String("&#%1;")).arg(c);
            else // this also covers surrogates
                result += QChar(c);
        }
    }
    return result;
}

static bool saveTMX(const Translator &translator, QIODevice &dev, ConversionData &)
{
    QString languageCode = translator.sourceLanguageCode();
    QTextStream t(&dev);

    t.setCodec(QTextCodec::codecForName("UTF-8"));
    t << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    t << "<!DOCTYPE tmx SYSTEM \"tmx14.dtd\">\n";
    t << "<tmx version=\"1.4\">\n";
    t << "    <header creationtool=\"linguist\"";
    t << " creationtoolversion=\"" QT_VERSION_STR  "\"";
    t << " segtype=\"paragraph\"";
    t << " o-encoding=\"UTF-8\"";
    t << " adminlang=\"" << languageCode << "\"";
    t << " srclang=\"" << languageCode << "\"";
    t << " o-tmf=\"unknown\"";
    t << " datatype=\"plaintext\"/>\n";
    t << "    <body>\n";

    int counter = 1;
    foreach (const TranslatorMessage &msg, translator.messages()) {

        t << "        <tu tuid=\"" << counter << "\">\n";

        languageCode = translator.sourceLanguageCode();
        t << "            <tuv xml:lang=\"" << languageCode << "\">\n";
        t << "                <seg>" << protect(msg.sourceText()) << "</seg>\n";
        t << "            </tuv>\n";

        languageCode = translator.languageCode();
        t << "            <tuv xml:lang=\"" << languageCode << "\">\n";
        QString str = msg.translations().join(QLatin1String("@"));
        str.replace(QChar(Translator::BinaryVariantSeparator),
                    QChar(Translator::TextVariantSeparator));
        t << "                <seg>" << protect(str) << "</seg>\n";
        t << "            </tuv>\n";

        t << "        </tu>\n";
        counter++;
    }

    t << "    </body>\n";
    t << "</tmx>\n";

    return true;
}

int initTMX()
{
    Translator::FileFormat format;

    format.extension = QLatin1String("tmx");
    format.description = QObject::tr("Translation Memory eXchange format (TMX)");
    format.fileType = Translator::FileFormat::TranslationSource;
    format.priority = 0;
    format.loader = &loadTMX;
    format.saver = &saveTMX;
    Translator::registerFileFormat(format);

    return 1;
}

Q_CONSTRUCTOR_FUNCTION(initTMX)

QT_END_NAMESPACE
