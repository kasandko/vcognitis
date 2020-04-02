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

#ifndef CFDOWNLOADHANDLER_H
#define CFDOWNLOADHANDLER_H

#include <QObject>
#include <QNetworkReply>
#include <QTimer>
#include <QQueue>
#include <QFutureWatcher>

#include "QQmlHelpers"
#include "QQmlObjectListModel"

#include "cf.global.h"
#include "structs/cf.operror.h"
#include "structs/cf.group.h"

#define CF_DOWNLOADHANDLER_TYPENAME "CFDownloadHandler"

/*!
 * \brief The CFDownloadHandler class is responsible for group member
 * download operation
 *
 * One instance of this class handles one group, which must be
 * provided in constructor
 *
 * Received group members are immediately saved directly to db
 *
 * finished will be emitted upon completion; partlyDownloadFinished will be emitted upon partly
 * completion; on error, respective signal will be emitted
 */
class CFDownloadHandler : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(CFGroup *, group)
    QML_READONLY_PROPERTY(int, allRequestCount)
    QML_READONLY_PROPERTY(int, requestCount)

public:
    explicit CFDownloadHandler(CFGroup *group,
                               QObject *parent = nullptr);

    ~CFDownloadHandler();
    /*!
     * \brief Size of user batch received in one reply
     */
    static const int k_offsetStep;

    /*!
     * \brief Starts downloading of whole group from current offset to sub count
     */
    void start();
    /*!
     * \brief startPartly Starts downloading only part of group (whole if subcount - offset < subCount)
     * \param subCount - count of subs which we should download
     */
    void startPartly(int subCount);

    /*!
     * \brief Stops immediately downloading also flush requestSender queue.
     */
    void stop();

signals:
    /*!
     * \brief started - Emitted when download is started
     */
    void started();
    /*!
     * \brief Emitted when download finished
     */
    void finished(bool success = true);

    /*!
     * \brief partlyDownloadFinished Emitted when partly DownloadFinished(not whole group)
     */
    void partlyDownloadFinished(bool success = true);

    /*!
     * \brief chunkFinished - Emitted when downloaded only chunk of data which <= k_offsetStep
     * \param processedUserCount - count of users which was downloaded
     */
    void chunkFinished(int processedUserCount);

    void error(CFOpError *error);

private:
    // Basically this struct is needed only to test if there was error or not
    struct ParseMembersResult
    {
        enum Error {
            NoError,
            CountriesInsert,
            UsersInsert,
            UserGroupsInsert,
            Commit,
            Open
        };

        int error = NoError;
        QString debugErrorMessage;
        QString errorMessage;
    };

    void resetAllRequestCount();

    /*!
     * \param processedUserCount Number of users that were proccessed in the last request
     * \param offset - current offset
     */
    void endIteration(int processedUserCount, int offset = -1);

    // Following 2 methods are part of the download iteration
    void makeGetMembersRequest(int offset);
    void parseMembers(QSharedPointer<QIODevice> replyData,
                      const QMap<QByteArray, QByteArray> &rawHeaders);

    /*!
     * \brief Appends a new error with given parameters and stop download if error is not safe
     */
    void handleError(bool safe,
                     const QString &technicalMessage,
                     const QString &userfriendlyMessage,
                     int code = INVALID_VALUE);

    /*!
     * \brief Handles network error
     */
    void errorWithServerOrConnection(int  error,
                                     const  QString  &errorMessage);

    /*!
     * \brief Given VK relgion ID, returns respective Personal::Religion enum value
     */
    int parseReligion(int religionId) const;

    /*!
     * \brief Calculates birth year based on school and university info
     */
    int guessBirthYear(const QJsonObject &jsUser);

    bool m_inProgress = false;
    bool m_partlyDownload = false;
    qint64 m_partlyLastOffset = -1;

    QVector<int> m_offsetDoneVec;

    QSet<int> m_friendsAndMyself;

    /*!
     * \brief m_watcherArray array for storing all working watchers
     */
    QVector<QFutureWatcher<ParseMembersResult>*> m_watcherArray;
};

#endif // CFDOWNLOADHANDLER_H
