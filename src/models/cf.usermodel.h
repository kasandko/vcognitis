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

#ifndef CFUSERMODEL_H
#define CFUSERMODEL_H

#include "lib-qt-qml-tricks/src/qqmlobjectlistmodel.h"
#include "structs/cf.user.h"
#include "cf.global.h"
#include <QJsonArray>
#include <QTimer>

#define CF_USERMODEL_TYPENAME "CFUserModel"

/*!
 * \brief The CFUserModel class provides user model based on conditional SELECT
 *
 * Caches only portion of result set, more users are loaded
 * on demand (see fetchMore)
 */
class CFUserModel : public QQmlObjectListModel<CFUser>
{
    Q_OBJECT

    // Following properties are WHERE conditions for SELECT statement
    QML_WRITABLE_PROPERTY(int, country)
    QML_WRITABLE_PROPERTY(QJsonArray, city)
    QML_WRITABLE_PROPERTY(int, sex)
    QML_WRITABLE_PROPERTY(int, fromAge)
    QML_WRITABLE_PROPERTY(int, toAge)
    QML_WRITABLE_PROPERTY(bool, photo)
    QML_WRITABLE_PROPERTY(QList<int>, political)
    QML_WRITABLE_PROPERTY(QList<int>, religion)
    QML_WRITABLE_PROPERTY(QList<int>, lifeMain)
    QML_WRITABLE_PROPERTY(QList<int>, peopleMain)
    QML_WRITABLE_PROPERTY(QList<int>, smoking)
    QML_WRITABLE_PROPERTY(QList<int>, alcohol)
    QML_WRITABLE_PROPERTY(QList<int>, relation)
    QML_WRITABLE_PROPERTY(QList<int>, groups)
    QML_WRITABLE_PROPERTY(int, limit)
    QML_WRITABLE_PROPERTY(bool, searchAge)
    QML_WRITABLE_PROPERTY(bool, exact)

    QML_WRITABLE_PROPERTY(int, lastViewedIndex)
    QML_WRITABLE_PROPERTY(bool, showViewed)
    QML_READONLY_PROPERTY(int, usersDownloaded)
    QML_READONLY_PROPERTY(int, viewedCount)
    Q_PROPERTY(int viewedPercent READ viewedPercent NOTIFY lastViewedIndexChanged)
public:
    explicit CFUserModel(QObject *parent = nullptr,
                         const QByteArray &displayRole = QByteArray(),
                         const QByteArray &uidRole = QByteArray());
    int viewedPercent() const;

    /*!
     * \brief will update count of users already downloaded
     */
    void updateUsersDownloaded();
    /*!
     * \brief Returns complete SELECT statement with all available WHERE conditions set
     * \param order If it is true then adds 'ORDER BY groupScore DESC' to the statement
     * \param limit If it is not an INVALID_VALUE then adds 'LIMIT limit' to the statement
     * \param offset IF it is not and INVALID_VALUE then add 'OFFSET offset' to the statement
     */
    QString selectStatement(bool order = false,
                            int limit = INVALID_VALUE,
                            int offset = INVALID_VALUE) const;

    /*!
     * \brief Returns statement which return count of rows based on current select statement
     */
    QString countStatement() const;

    QString countViewedStatement() const;

    /*!
     * \brief Builds and returns WHERE clause with all
     * conditions set accordingly to current filter properties
     */
    QString whereClause(bool withViewed = false, bool viewedOnly = false, bool withAge = true) const;

    /*!
     * \brief True if more users can be fetched from DB
     */
    Q_INVOKABLE bool canFetchMore(const QModelIndex &parent) const override;

    /*!
     * \brief Fetches more users from DB based on current offset value
     */
    Q_INVOKABLE void fetchMore(const QModelIndex &parent) override;

public slots:
    /*!
     * \brief Resets model and fetches initial count of users from DB based on current filtering
     * \return true on success; on false model will not be resetted
     */
    bool select();

    /*!
     * \brief Inits filters from settings values of current user
     */
    void initFilters();

    /*!
     * \brief Updates 'favorite' property of user with given ID with given value
     */
    void refreshFavorite(int userId, bool value);

    /*!
     * \brief Given the list of personal filter values (e.g. 'lifeMain')
     * checks how given value relates to it
     * \return 1 if value is present in the list, 0 if list is empty,
     * -1 if value is missing from the list
     */
    static int checkPersonal(const QList<int> &personal, int value);

    int usersCountByFilterSettings(int countryId, int sex, QJsonArray cityList, bool photo,
                                          int fromAge, int toAge, QVariantList relationList,
                                          QVariantList groupList, bool exact,
                                   QVariantList politicalList, QVariantList religionList,
                                   QVariantList lifeMainList, QVariantList peopleMainList,
                                   QVariantList smokingList, QVariantList alcoholList);

    int usersCountByFilterSettingsScored(int countryId, int sex, QJsonArray cityList, bool photo,
                                          int fromAge, int toAge, QVariantList relationList,
                                          QVariantList groupList, bool exact,
                                   QVariantList politicalList, QVariantList religionList,
                                   QVariantList lifeMainList, QVariantList peopleMainList,
                                   QVariantList smokingList, QVariantList alcoholList);
signals:
    void selectFinished();

private:
    void updateViewedProfiles();

    /*!
     * \brief Step by which model fetches more user data
     */
    int m_step = 50;

    /*!
     * \brief Current offset value
     *
     * Needed to fetch more data on demand
     */
    int m_offset = 0;

    /*!
     * \brief Total count rows returned by SELECT
     */
    int m_queryResultCount = 0;

    /*!
     * \brief Timer for update viewed profiles in DB
     */
    QTimer m_viewedTimer;

    QList<int> m_viewedIndexList;
    int m_lastUpdatedViewedIndex;
};

#endif // CFUSERMODEL_H
