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

#ifndef CFSCOREHANDLER_H
#define CFSCOREHANDLER_H

#include <QObject>
#include <QNetworkReply>
#include <QSqlQuery>
#include <QTimer>

#include "QQmlHelpers"
#include "QQmlObjectListModel"

#include "cf.global.h"
#include "structs/cf.operror.h"

class CFUserModel;

#define CF_SCOREHANDLER_TYPENAME "CFScoreHandler"
#define CF_GROUPS_COUNT 100

/*!
 * \brief The CFScoreHandler class is responsible for user scoring operation
 *
 * User data is selected directly from DB, using current filter params of a user model
 *
 * One instance of this class handles one pass over query result set
 *
 * It is possible to pause and resume scoring via setting inProgress
 * to false and true respectively
 *
 * finished will be emitted upon completion; on error, respective signal
 * will be emitted and scoring paused
 */
class CFScoreHandler : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(bool, paused)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged) // [0.0, 1.0]
    Q_PROPERTY(double estimatedDuration READ estimatedDuration NOTIFY estimatedDurationChanged) // In seconds
public:
    /*!
     * \param index Index from which to resume scoring
     * \param userModel User model needed to get SQL SELECT statement used for fetching users
     */
    explicit CFScoreHandler(CFUserModel *userModel, int score = INVALID_VALUE, QObject *parent = nullptr);

    double progress() const;
    int estimatedDuration() const;
    void start();
    void togglePaused();

signals:
    void progressChanged();
    void estimatedDurationChanged();

    /*!
     * \brief started - Emitted when download is started
     */
    void started();
    /*!
     * \brief Emitted when download finished
     */
    void finished(bool success = true);

    /*!
     * \brief chunkFinished - Emitted when downloaded only chunk of data
     */
    void chunkFinished();

    void error(CFOpError *error);

private:
    /*!
     * \brief Initializes user select query using statement got from user model
     */
    bool initQuery(CFUserModel *userModel);

    bool removePreviousData(int userId);

    void endIteration(int id);

    void makeGetSubscriptionsRequest(int id);
    void parseSubscriptions(QSharedPointer<QIODevice> replyData,
                            const QMap<QByteArray, QByteArray> &rawHeaders);

    /*!
     * \brief Appends a new error with given parameters, increments attempt count
     * and ends iteration without incrementing index
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

    QString selectStatement(CFUserModel *userModel);

    int m_score;

    /**
     * @brief User select query
     */
    QSqlQuery m_query;

    /*!
     * \brief Query result count
     */
    int m_count = 0;

    QVector<int> m_userIds;
    /*!
     * \brief m_userGroupIds - uses for store for each user their base groups
     */
    QMap<int, int> m_userGroupIds;
};

#endif // CFSCOREHANDLER_H
