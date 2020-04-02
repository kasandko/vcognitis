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

#ifndef CFAGESEARCHHANDLER_H
#define CFAGESEARCHHANDLER_H

#include <QObject>
#include <QNetworkReply>
#include <QSqlQuery>
#include <QSet>
#include <cmath>

#include "QQmlHelpers"
#include "QQmlObjectListModel"

#include "cf.global.h"
#include "structs/cf.operror.h"

#define CF_AGESEARCHHANDLER_TYPENAME "CFAgeSearchHandler"

class CFGroup;

/*!
 * \brief The CFAgeSearchHandler class is responsible for age detection based on group search
 */
class CFAgeSearchHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged) // [0.0, 1.0]
    Q_PROPERTY(double estimatedDuration READ estimatedDuration NOTIFY progressChanged) // In seconds
public:
    explicit CFAgeSearchHandler(QObject *parent = nullptr);

    double progress() const;
    int estimatedDuration() const;

signals:
    void progressChanged();

    /*!
     * \brief Emitted when search successfully finished
     */
    void finished(bool success = true);

    void error(CFOpError *error);

private:
    void next();
    void makeSearchRequest(int sort = 0);
    void parseUsers(QSharedPointer<QIODevice> replyData,
                            const QMap<QByteArray, QByteArray> &rawHeaders);
    void initUserSet();
    void updateAges();

    /*!
     * \brief Appends a new error with given parameters, increments attempt count
     * and ends iteration without incrementing index
     */
    void handleError(bool safe,
                     const QString &technicalMessage,
                     const QString &userfriendlyMessage,
                     int code = INVALID_VALUE);

    void handleSqlError(const QString &technicalMessage,
                     const QString &userfriendlyMessage);

    /*!
     * \brief Handles network error
     */
    void errorWithServerOrConnection(int  error,
                                     const  QString  &errorMessage);

    int m_allRequestCount;
    int m_processedRequestCount;

    QSet<int> m_userIdSet;
    QSet<int> m_matchedIdSet;
    QList<CFGroup*> m_groupList;
    int m_groupIndex = 0;

    // Filter fields for group search
    int m_country;
    QList<int> m_cityList;
    int m_cityIndex = 0;
    QList<int> m_relationList;
    int m_relationIndex = -1;
    int m_sex;
    int m_fromAge;
    int m_toAge;
    bool m_photo;
};

#endif // CFAGESEARCHHANDLER_H
