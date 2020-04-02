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

#ifndef CFCOUNTRY_H
#define CFCOUNTRY_H

#include "QQmlHelpers"

class QSqlQuery;

/*!
 * \brief The CFCountry class encapsulates country entity
 */
class CFCountry : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(int, uid);
    QML_WRITABLE_PROPERTY(QString, name);

public:
    explicit CFCountry (QObject *parent = nullptr);
    explicit CFCountry(const QSqlQuery &query, QObject *parent = nullptr);
};

#endif // CFCOUNTRY_H
