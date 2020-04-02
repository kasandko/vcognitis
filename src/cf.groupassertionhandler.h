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

#ifndef CFGROUPASSERTIONHANDLER_H
#define CFGROUPASSERTIONHANDLER_H

#include <QObject>
#include <QNetworkReply>
#include <cmath>

#include "cf.global.h"

class CFGroupModel;
class CFGroup;
class CFRequest;

/*!
 * \brief The CFGroupAssertionHandler class is responsible for asserting edited groups
 */

class CFGroupAssertionHandler : public QObject
{
    Q_OBJECT
public:
    explicit CFGroupAssertionHandler(QObject *parent = nullptr);
    static bool addGroupToDatabase(CFGroup *group, bool isInsert = true, int oldUid = INVALID_VALUE);

signals:
    /**
     * @brief Emitted when manager finishes assertion of the model data
     * @param error True if at least one error occured during execution
     */
    void finished(bool error);

    /**
     * @brief Emitted when general error occured
     * @param message Message describing problem
     */
    void error(const QString &message);

    /**
     * @brief Emitted when error occured with a group
     * @param sourceRow Row index inside a source model
     * @param message Message describing problem
     */
    void errorAt(int sourceRow, const QString &message);

private:

    void assertCurrent();
    void incrementAndProceed();

    void makeResolveScreenNameRequest(const QString &screenName);
    void resolveScreenNameSuccess(QSharedPointer<QIODevice> replyData,
                        const QMap<QByteArray,  QByteArray> &rawHeaders);
    void makeGetMembersRequest(const QString &screenName);
    void getMembersSuccess(QSharedPointer<QIODevice> replyData,
                        const QMap<QByteArray,  QByteArray> &rawHeaders);
    void makeGetByIdRequest(const QString &screenName);
    void getByIdSuccess(QSharedPointer<QIODevice> replyData,
                        const QMap<QByteArray,  QByteArray> &rawHeaders);

    void errorWithServerOrConnection(int  error,
                                     const  QString  &errorMessage);

    void resend(CFRequest *req);
    void unknownError();

    CFGroupModel* m_groupModel = nullptr;
    int m_assertedRow;
    bool m_errorOccured;

    /*!
     * \brief Network request attempt number, needed for automatic resend on errors
     */
    int m_attempt = 0;
    QUrl m_lastUrl; // in case we'll need to resend last request

    static const int k_maxAttempts;
};

#endif // CFGROUPASSERTIONHANDLER_H
