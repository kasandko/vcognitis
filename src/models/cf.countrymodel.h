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

#ifndef CFCOUNTRYMODEL_H
#define CFCOUNTRYMODEL_H

#include "lib-qt-qml-tricks/src/qqmlobjectlistmodel.h"
#include "structs/cf.country.h"

#define CF_COUNTRYMODEL_TYPENAME "CFCountryModel"

class CFCountryModel : public QQmlObjectListModel<CFCountry>
{
    Q_OBJECT
public:
    explicit CFCountryModel(QObject *parent = nullptr,
                            const QByteArray &displayRole = QByteArray(),
                            const QByteArray &uidRole = "uid");

    /**
     * @brief select will fetch data from sql database
     */
    bool select();
public slots:
    /**
     * @brief Returns the index of country with given id
     * or INVALID_INDEX if not found
     */
    int find(int id) const;

};

#endif // CFCOUNTRYMODEL_H
