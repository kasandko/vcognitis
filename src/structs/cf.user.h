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

#ifndef CFUSER_H
#define CFUSER_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "QQmlHelpers"

class QSqlQuery;

/*!
 * \brief The CFUser class encapsulates user entity
 */
class CFUser : public QObject
{
    Q_OBJECT
    // Following properties mirror DB columns
    QML_WRITABLE_PROPERTY(int, vkId)
    QML_WRITABLE_PROPERTY(QString, screenName)
    QML_WRITABLE_PROPERTY(QString, firstName)
    QML_WRITABLE_PROPERTY(QString, lastName)
    QML_WRITABLE_PROPERTY(int, city)
    QML_WRITABLE_PROPERTY(int, country)
    QML_WRITABLE_PROPERTY(int, sex)
    QML_WRITABLE_PROPERTY(qint64, bdate)
    QML_WRITABLE_PROPERTY(QString, photoMax)
    QML_WRITABLE_PROPERTY(int, relation)
    QML_WRITABLE_PROPERTY(int, political)
    QML_WRITABLE_PROPERTY(int, religion)
    QML_WRITABLE_PROPERTY(int, peopleMain)
    QML_WRITABLE_PROPERTY(int, lifeMain)
    QML_WRITABLE_PROPERTY(int, smoking)
    QML_WRITABLE_PROPERTY(int, alcohol)
    QML_WRITABLE_PROPERTY(int, groupScore)
    QML_WRITABLE_PROPERTY(int, interestScore)
    QML_WRITABLE_PROPERTY(bool, scored)
    QML_WRITABLE_PROPERTY(bool, favorite)
    QML_WRITABLE_PROPERTY(bool, inaccurateAge)
    QML_WRITABLE_PROPERTY(int, groupId)
    QML_WRITABLE_PROPERTY(bool, openMessages)
    QML_WRITABLE_PROPERTY(bool, viewed)

    // Proxies that map this user's groups to app user's groups
    QML_CONSTANT_PROPERTY(QSortFilterProxyModel *, positiveGroups)
    QML_CONSTANT_PROPERTY(QSortFilterProxyModel *, negativeGroups)
    QML_CONSTANT_PROPERTY(QSortFilterProxyModel *, interests)

    // Age based on birthday date
    Q_PROPERTY(int age READ calculateAge NOTIFY bdateChanged)


public:
    explicit CFUser (QObject *parent = nullptr);
    explicit CFUser(const QSqlQuery &query, QObject *parent = nullptr);

private slots:
    void persistFavorite();

private:
    QList<int> topInterests();

    int calculateAge() const;

    void initProxy();

    /*!
     * \brief Fetches this user's groups from DB, build RegEx id string from them
     * and sets this string to the proxy models
     */
    void refreshFilterStrings();
};


#endif // CFUSER_H
