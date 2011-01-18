/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
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

#include "project.h"
#include "option.h"

#include <QSet>
#include <QStringList>
#include <QByteArray>
#include <QDir>
#include <QFileInfo>


#include "filelist.h"



QT_BEGIN_NAMESPACE

namespace {
    const char * const indent = "        ";

    void dumpFilename (QString name,
                       QSet<QString> &skip,
                       QString (*transform)(QString) = 0,
                       bool isDirectoryList = false
    ) {
        if (Option::mkspecial::filelist_relate_filenames) {
            name = QDir().absoluteFilePath(name);
            QDir relateTo = Option::mkspecial::filelist_relate_to != "" ?
                            QDir(Option::mkspecial::filelist_relate_to) :
                            QDir::current();

            name = relateTo.relativeFilePath(name);
        }

        name = name.trimmed();

        if (!isDirectoryList) {
            if (name.startsWith("./"))
                name = name.remove(0, 2);

            if (name == "")
                return;
        } else {
            // Our relation logic might have stripped away a naked "./"
            // or ".", but we want it explicitly in case of -I.
            if (name == "")
                name = ".";
        }

        if (transform)
            name = transform (name);

        if (skip.contains(name)) return;
        fprintf (stdout, " \\\n%s%s", indent, name.toLocal8Bit().data());
        skip.insert (name);
    }


    void dumpFilelist (QString name, QStringList list,
                       QString (*transform)(QString) = 0,
                       bool isDirectoryList = false
    ) {
        const char *varname = name.toLocal8Bit().data();
        fprintf (stdout, "%s = ", varname);

        QSet<QString> skip;

        list.sort();
        foreach (QString name, list) {
            dumpFilename (name, skip, transform, isDirectoryList);
        }
        fprintf (stdout, "\n\n");
    }


    QString filename_pattern_replace (QString name, QString pattern) {
        if (pattern.trimmed() == "")
            return name;
        QFileInfo fi(name);
        return fi.path() + "/" +
               pattern.replace("#", fi.baseName());
    }

    QString ui_pattern_replace (QString name) {
        return filename_pattern_replace(name,
            Option::mkspecial::filelist_ui_pattern);
    }

    QString moc_pattern_replace (QString name) {
        return filename_pattern_replace(name,
            Option::mkspecial::filelist_moc_pattern);
    }

    QString qrc_pattern_replace (QString name) {
        return filename_pattern_replace(name,
            Option::mkspecial::filelist_qrc_pattern);
    }

    QString include_path_replace (QString name) {
        return "-I" + name;
    }
}



void FilelistGenerator::run(QMakeProject project)
{
    FilelistGenerator f;
    f.setProjectFile(&project);
    QTextStream t(&Option::output);
    f.writeMakefile(t);
}



FilelistGenerator::FilelistGenerator()
{
}



bool FilelistGenerator::writeMakefile(QTextStream &str)
{
    const QString oldPwd = qmake_getpwd();
    qmake_setpwd(Option::output_dir); // Because the project file contains
                                      // names relative to the folder qmake is
                                      // run in (at least that's my guess)
    verifyCompilers();
    QString prefix = Option::mkspecial::filelist_prefix + "_";

    QString manual_sourcefiles = prefix + "MANUAL_SOURCEFILES";
    QString include_path       = prefix + "INCLUDEPATH";
    QString all_sourcefiles    = prefix + "SOURCES";
    QString ui_files           = prefix + "UIFILES";
    QString moc_files          = prefix + "MOCFILES";
    QString qrc_files          = prefix + "RESOURCEFILES";
    QString built_sources      = prefix + "BUILT_SOURCES";
    QString cppflags           = prefix + "CPPFLAGS";

    dumpFilelist(manual_sourcefiles, project->values("SOURCES"));
    dumpFilelist(ui_files, project->values("FORMS"), ui_pattern_replace);
    dumpFilelist(moc_files, project->values("MOCABLES"),moc_pattern_replace);
    dumpFilelist(qrc_files, project->values("RESOURCES"), qrc_pattern_replace);
    dumpFilelist(include_path, project->values("INCLUDEPATH"),
                 include_path_replace, true);

    fprintf(stdout, "%s = \\\n"
                    "%s$(%s) \\\n"
                    "%s$(%s) \\\n"
                    "%s$(%s)\n\n",
                    built_sources.toLocal8Bit().data(),
                    indent, ui_files.toLocal8Bit().data(),
                    indent, moc_files.toLocal8Bit().data(),
                    indent, qrc_files.toLocal8Bit().data()
    );
    fprintf(stdout, "%s = \\\n"
                    "%s$(%s) \\\n"
                    "%s$(%s)\n\n",
                    all_sourcefiles.toLocal8Bit().data(),
                    indent, manual_sourcefiles.toLocal8Bit().data(),
                    indent, built_sources.toLocal8Bit().data()
    );
    fprintf(stdout, "%s = \\\n"
                    "%s$(%s)\n\n",
                    cppflags.toLocal8Bit().data(),
                    indent, include_path.toLocal8Bit().data()
    );

    qmake_setpwd(oldPwd);
    return true;
}

QT_END_NAMESPACE
