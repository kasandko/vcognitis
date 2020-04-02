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

#ifndef CFGROUPMODEL_H
#define CFGROUPMODEL_H

#include <QUrl>

#include "lib-qt-qml-tricks/src/qqmlobjectlistmodel.h"
#include "lib-qt-qml-tricks/src/qqmlhelpers.h"
#include "structs/cf.group.h"


#define CF_GROUPMODEL_TYPENAME "CFGroupModel"

/**
 * @brief Model that handles 'Groups' SQL table
 * Include most logic required when dealing with Group entities
 */
class CFGroupModel : public QQmlObjectListModel<CFGroup>
{
    Q_OBJECT
    // Shows status of db(sets true if there are any changes to sync)
    QML_READONLY_PROPERTY(bool, dirtyStatus)
    QML_WRITABLE_PROPERTY(bool, haveEmpty)
    QML_WRITABLE_PROPERTY(QList<int>, emptyIndexes)

public:
    explicit CFGroupModel(QObject *parent = nullptr,
                         const QByteArray &displayRole = QByteArray(),
                         const QByteArray &uidRole = "uid");

    // Need to watch dirty status(if user change something we mark model as dirty)
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;

public slots:
    /**
     * @brief submitAll Save all changes from model to DB
     * @return status of opeation true on success
     */
    bool submitAll();
    /**
     * @brief Add one empty group to model without submitting
     * @param desired True if weight should be positive
     */
    void addEmpty(bool desired = true);

    /**
     * @brief Returns the index of group with given ID
     * or INVALID_INDEX if not found
     */
    int find(int id) const;

    /**
     * @brief Returns true if row with given screen name already exists
     * (rows that are marked for deletion do not count)
     */
    bool isUnique(const QString &screenName) const;

    /*!
     * \brief Resets model and fetches grops from DB
     * \return true on success; on false model will not be reseted
     */
    bool select();

signals:
    void cantAddEmpty();

private:
    /*!
     * \brief Returns the group with hishest priority
     */
    CFGroup *highestPriorityGroup() const;
};

#endif // CFGROUPMODEL_H
