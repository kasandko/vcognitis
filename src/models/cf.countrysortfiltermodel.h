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

#ifndef CFCOUNTRYSORTFILTERMODEL_H
#define CFCOUNTRYSORTFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "QQmlHelpers"

#define CF_COUNTRYSORTFILTERPROXY_TYPENAME "CFCountrySortFilterProxy"

class CFCountryModel;

class CFCountrySortFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(QString, filterString)

public:
    explicit CFCountrySortFilterModel(CFCountryModel *sourceModel, QObject *parent = nullptr);

public slots:
    /**
     * @brief indexByRow
     * @param row
     * @return return model index by requested row
     */
    QModelIndex indexByRow(int row) const;
    /**
     * @brief Returns the index of country with given id
     * or INVALID_INDEX if not found
     */
    int find(int id) const;


protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

};

#endif // CFCOUNTRYSORTFILTERMODEL_H
