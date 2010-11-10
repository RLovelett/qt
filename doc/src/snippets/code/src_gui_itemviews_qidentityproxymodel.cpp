/****************************************************************************
**
** Copyright (C) 2010 Klarälvdalens Datakonsult AB,
**        a KDAB Group company, info@kdab.com,
**        author Stephen Kelly <stephen.kelly@kdab.com>
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

//! [0]
class DateFormatProxyModel : public QIdentityProxyModel
{
  // ...

  void setDateFormatString(const QString &formatString)
  {
    m_formatString = formatString;
  }

  QVariant data(const QModelIndex &index, int role)
  {
    if (role != Qt::DisplayRole)
      return QIdentityProxyModel::data(index, role);

    const QDateTime dateTime = sourceModel()->data(SourceClass::DateRole).toDateTime();

    return dateTime.toString(m_formatString);
  }

private:
  QString m_formatString;
};
//! [0]
