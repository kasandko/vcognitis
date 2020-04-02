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

#ifndef CFSPECIFYAGEHANDLER_H
#define CFSPECIFYAGEHANDLER_H

#include <QObject>
#include <QNetworkReply>
#include <cmath>

#include "QQmlHelpers"
#include "QQmlObjectListModel"

#include "cf.global.h"
#include "structs/cf.user.h"
#include "structs/cf.operror.h"

#define CF_SPECIFYAGEHANDLER_TYPENAME "CFSpecifyAgeHandler"

/*!
 * \brief The CFAgeSearchHandler class is responsible for age specification for concrete user based on group search
 */
class CFSpecifyAgeHandler : public QObject
{
    Q_OBJECT
public:
    explicit CFSpecifyAgeHandler(CFUser *user, QObject *parent = nullptr);

signals:
    /*!
     * \brief Emitted when age specification search finished
     */
    void finished(bool success = true);

    void error(CFOpError *error);

private:
    void startInaccurateCheck();
    void startMaxCheck();
    void startMinCheck();
    void startNormalCheck();
    void next(bool success = true);
    void makeSearchRequest();
    void parseUsers(QSharedPointer<QIODevice> replyData,
                            const QMap<QByteArray, QByteArray> &rawHeaders);
    void updateUserAge(int age);

    /*!
     * \brief Appends a new error with given parameters, increments attempt count
     * and ends iteration without incrementing offset
     */
    void handleError(const QString &technicalMessage,
                     const QString &userfriendlyMessage,
                     int code = INVALID_VALUE);

    void handleCriticalError(const QString &technicalMessage,
                     const QString &userfriendlyMessage);

    /*!
     * \brief Handles network error
     */
    void errorWithServerOrConnection(int  error,
                                     const  QString  &errorMessage);


    CFUser* m_user;
    int m_ageFrom;
    int m_ageTo;
    int m_middleFrom;
    int m_middleTo;
    int m_tmp;

    static const int k_minAge;
    static const int k_maxAge;
};

#endif // CFSPECIFYAGEHANDLER_H
