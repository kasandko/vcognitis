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

#ifndef CFUSERMANAGER_H
#define CFUSERMANAGER_H

#include <QObject>
#include <QIODevice>
#include <QNetworkReply>
#include <QTimer>
#include <QMediaPlayer>
#include <QtConcurrent>

#include "QQmlHelpers"

#include "cf.global.h"
#include "models/cf.countrymodel.h"
#include "models/cf.countrysortfiltermodel.h"
#include "models/cf.usermodel.h"
#include "models/cf.favoritemodel.h"
#include "cf.downloadqueue.h"
#include "cf.scorehandler.h"
#include "cf.agesearchhandler.h"
#include "cf.specifyagehandler.h"

#define CF_OPERRORLIST_TYPENAME "CFOpErrorList"

/**
 * @brief Singleton object responsible for managing users (profiles)
 * It makes request and processes replies via VK API, persists
 * results in SQL model, stores objects that handle various user related operations
 */
class CFUserManager : public QObject
{
    Q_OBJECT
    SINGLETON(CFUserManager)
    QML_CONSTANT_PROPERTY(CFUserModel *, model)
    QML_READONLY_PROPERTY(CFUser *, currentUser)
    QML_CONSTANT_PROPERTY(CFFavoriteModel *, favoriteModel)
    QML_CONSTANT_PROPERTY(CFCountryModel *, countryModel)
    QML_CONSTANT_PROPERTY(CFCountrySortFilterModel *, countryProxyModel)
    QML_READONLY_PROPERTY(CFDownloadQueue *, downloadQueue)

    /*!
        \brief Pointer to current scoring handler, nullptr otherwise
    */
    QML_READONLY_PROPERTY(CFScoreHandler *, score)

    /*!
        \brief Pointer to current age search handler, nullptr otherwise
    */
    QML_READONLY_PROPERTY(CFAgeSearchHandler *, ageSearch)

    /*!
        \brief Pointer to current specify age handler, nullptr otherwise
    */
    QML_READONLY_PROPERTY(CFSpecifyAgeHandler *, ageSpecifier)

    /* Rescoring progress in range [0.0, 1.0]*/
    QML_READONLY_PROPERTY(double, rescoringPercent);
    /* Rescoring status*/
    QML_READONLY_PROPERTY(bool, rescoringActive);

    QML_WRITABLE_PROPERTY(int, lastViewedIndex)

    /* user for whom we are currently specifying age */
    QML_READONLY_PROPERTY(int, currentAgeSpecifyVkId)

public:
    explicit CFUserManager(QObject *parent = nullptr);

    QSet<int> friendsAndMyself() const;
    /*!
     * \brief ASYNC topAnalyzeTimeScore Calculate nearest to timeInSec score
     * \param timeInSec
     * \param wait if it true method will retunr result otherwise it will emit signal topAnalyzeTimeScoreFinished
     * \return if wait true then return pair where first - time, second - score, otherwise return pair()
     */
    QPair<int, int> topAnalyzeTimeScore(int timeInSec, bool wait = false);

signals:
    void topAnalyzeTimeFinished(int time);
    void topAnalyzeTimeScoreFinished(int time, int score);

    void scoringSuccessful();

    /**
     * @brief citySearchFinished Emitted when city search successfully executed
     * @param json list of cities with region.
     */
    void citySearchFinished(QString json);

    /**
     * @brief noCityFound Emitted when no city found from citySearch
     */
    void noCitiesFound();

    /**
     * @brief error Emitted when error occurs in get operation
     * @param message Message describing problem
     */
    void error(const QString &message);

    bool rescoreFinished(bool success);

    void opError(CFOpError *error);

    void initialized();

public slots:
    /*!
     * \brief topAnalyzeTimeScore qml wrapper for topAnalyzeTimeScore
     * \param time
     */
    void topAnalyzeTimeScoreQml(int time);
    /*!
     * \brief ASYNC topAnalyzeTime will calculate approximate time in seconds
     * for analyze top users with score more than param and return that time
     * \param score

     */
    void topAnalyzeTime(int score);

    /*!
     * \brief topAnalyze Will start scoring for all users that have a score
     * greater than param inScore
     * \param inScore
     */
    void topAnalyze(int inScore, int allotteredTime = INVALID_VALUE);

    /*!
     * \brief Starts scoring operation of user proxy data
     * Returns immediately if another scoring is not yet finished
     */
    void startScoring();
    void cancelScoring();

    /*!
     * \brief Calls select on user filter model and user favorite model
     */
    void select();

    /*!
     * \brief Recomputes score of all users with available groups
     * \return true on success
     */
    void rescore(QList<int> interests = {});

    /**
     * @brief citySearch search cities by current country
     * @param query
     */
    void citySearch(QString inQuery, int countryId);

    void specifyAge(CFUser *user);

    /*!
     * \brief Handles network error
     */
    void errorWithServerOrConnection(int  error,
                                     const  QString  &errorMessage);

    /*!
     * \brief Inits filters, selects data, resumes scoring/download
     * and does other user dependent tasks
     */
    void init();

    void playNotification();
private:
    /*!
     * \brief Will return where statement for topAnalyze methods
     * \return
     */
    QString topAnalyzeWhereStatement();


    void citySearchSuccess(QSharedPointer<QIODevice> replyData,
                           const QMap<QByteArray,  QByteArray> &rawHeaders);

    void initScore();
    void removeScore();

    void initFriendsAndMyself();

    void makeGetFriendsRequest();
    void parseFriends(QSharedPointer<QIODevice> replyData,
                      const QMap<QByteArray, QByteArray> &rawHeaders);

    void makeMeRequest();
    void parseMe(QSharedPointer<QIODevice> replyData,
                 const QMap<QByteArray, QByteArray> &rawHeaders);

    QMediaPlayer m_notificationPlayer;

    /*!
     * \brief Set of friend IDs and app user ID
     * Needed for scoring to ignore respective people
     */
    QSet<int> m_friendsAndMyself;

    // rescore runs via QtConcurrent and we need to know when is is finished
    QFutureWatcher<bool> m_rescoreFutureWatcher;

    // TopAnalyzeTime runs via QtConcurrent and we need to know when is is finished
    QFutureWatcher<int> m_topAnalyzeTimeWatcher;
};

#endif // CFUSERMANAGER_H
