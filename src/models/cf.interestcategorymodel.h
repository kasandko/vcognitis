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

#ifndef CFINTERESTCATEGORYMODEL_H
#define CFINTERESTCATEGORYMODEL_H

#include "lib-qt-qml-tricks/src/qqmlobjectlistmodel.h"
#include "structs/cf.interestcategory.h"

#define CF_INTERESTCATEGORYMODEL_TYPENAME "CFInterestCategoryModel"

/**
 * @brief Model that handles 'InterestCategory' SQL table
 * Include most logic required when dealing with IterestCategory entities
 * (like searches, conditional selects, etc.)
 */
class CFInterestCategoryModel: public QQmlObjectListModel<CFInterestCategory>
{
    Q_OBJECT
public:
    explicit CFInterestCategoryModel(QObject *parent = nullptr,
                             const QByteArray &displayRole = QByteArray(),
                             const QByteArray &uidRole = "uid");
    /**
     * @brief Returns the index of group with given screen name
     * or INVALID_INDEX if not found
     */
    int find(const QString &name) const;

    bool select();
private:
};




#endif // CFINTERESTCATEGORYMODEL_H
