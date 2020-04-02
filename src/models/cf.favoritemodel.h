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

#ifndef CFFAVORITEMODEL_H
#define CFFAVORITEMODEL_H

#include "lib-qt-qml-tricks/src/qqmlobjectlistmodel.h"
#include "structs/cf.user.h"

#define CF_FAVORITEMODEL_TYPENAME "CFFavoriteModel"

/*!
 * \brief The CFFavoriteModel class provides user model filtered by 'favorite' property
 */
class CFFavoriteModel : public QQmlObjectListModel<CFUser>
{
    Q_OBJECT
public:
    explicit CFFavoriteModel(QObject *parent = nullptr,
                             const QByteArray &displayRole = QByteArray(),
                             const QByteArray &uidRole = QByteArray());

public slots:
    /*!
     * \brief Resets model and fetches all favorite users from DB
     * \return true on success; on false model will not be reseted
     */
    bool select();

signals:

};

#endif // CFFAVORITEMODEL_H
