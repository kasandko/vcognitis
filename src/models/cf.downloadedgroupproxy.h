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

#ifndef CFDOWNLOADEDGROUPPROXY_H
#define CFDOWNLOADEDGROUPPROXY_H

#include <QSortFilterProxyModel>

#include "QQmlHelpers"

#define CF_DOWNLOADEDGROUPPROXY_TYPENAME "CFDownloadedGroupProxy"

class CFGroupModel;
class CFGroup;

/*!
 * \brief The CFDownloadedGroupProxy class filters group model
 * by 'downloaded' property and search string
 */
class CFDownloadedGroupProxy : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(QString, searchString)
public:
    explicit CFDownloadedGroupProxy(CFGroupModel *sourceModel, QObject *parent = nullptr);

public slots:
    /*!
     * \brief Returns group with given uid or nullptr if not found
     */
    CFGroup *find(int uid);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // CFDOWNLOADEDGROUPPROXY_H
