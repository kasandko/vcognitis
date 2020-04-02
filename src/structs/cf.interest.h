/****************************************************************************
**
** Copyright © 2020 Barbatum™.
** All rights reserved.
** Contact: support@barbatum.ru
**
**
** This file is part of VCognitis project.
**
** VCognitis is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** VCognitis is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with VCognitis.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CF_INTEREST_H
#define CF_INTEREST_H

#include <QObject>

#include "QQmlHelpers"

class QSqlQuery;

/*!
 * \brief The CFInterest class encapsulates interest entity
 */
class CFInterest : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(int, uid);
    QML_WRITABLE_PROPERTY(QString, name);
    QML_WRITABLE_PROPERTY(QString, tags);
    QML_WRITABLE_PROPERTY(int, parentId);

public:
    explicit CFInterest (QObject *parent = nullptr);
    explicit CFInterest(const QSqlQuery &query, QObject *parent = nullptr);

};

#endif // CF_INTEREST_H
