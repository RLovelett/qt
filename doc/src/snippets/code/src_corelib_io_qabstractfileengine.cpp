/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [0]
class ZipEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString &fileName) const;
};

QAbstractFileEngine *ZipEngineHandler::create(const QString &fileName) const
{
    // ZipEngineHandler returns a ZipEngine for all .zip files
    return fileName.toLower().endsWith(".zip") ? new ZipEngine(fileName) : 0;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    ZipEngineHandler engine;

    MainWindow window;
    window.show();

    return app.exec();
}
//! [0]


//! [1]
QAbstractSocketEngine *ZipEngineHandler::create(const QString &fileName) const
{
    // ZipEngineHandler returns a ZipEngine for all .zip files
    return fileName.toLower().endsWith(".zip") ? new ZipEngine(fileName) : 0;
}
//! [1]


//! [2]
QAbstractFileEngineIterator *
CustomFileEngine::beginEntryList(QDir::Filters filters, const QStringList &filterNames)
{
    return new CustomFileEngineIterator(filters, filterNames);
}
//! [2]


//! [3]
class CustomIterator : public QAbstractFileEngineIterator
{
public:
    CustomIterator(const QStringList &nameFilters, QDir::Filters filters)
        : QAbstractFileEngineIterator(nameFilters, filters), index(0)
    {
        // In a real iterator, these entries are fetched from the
        // file system based on the value of path().
        entries << "entry1" << "entry2" << "entry3";
    }

    bool hasNext() const
    {
        return index < entries.size() - 1;
    }

    QString next()
    {
       if (!hasNext())
           return QString();
       ++index;
       return currentFilePath();
    }

    QString currentFileName()
    {
        return entries.at(index);
    }

private:
    QStringList entries;
    int index;
};
//! [3]
