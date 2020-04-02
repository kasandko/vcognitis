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

#ifndef CFGROUPMANAGER_H
#define CFGROUPMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QIODevice>
#include <QNetworkReply>
#include <QSortFilterProxyModel>

#include "QQmlHelpers"
#include "QQmlObjectListModel"

#include "cf.global.h"
#include "cf.groupassertionhandler.h"
#include "cf.loopedchunkdownloader.h"
#include "models/cf.groupmodel.h"
#include "models/cf.downloadedgroupproxy.h"

#include "models/cf.interestmodel.h"
#include "models/cf.interestcategorymodel.h"
#include "models/cf.interestproxymodel.h"
#include "models/cf.interestfilterproxymodel.h"


/**
 * @brief Singleton object responsible for managing groups
 * It makes requests and processes replies via VK API and persists
 * results in SQL model
 */
class CFGroupManager : public QObject
{
    Q_OBJECT
    SINGLETON(CFGroupManager)
    QML_CONSTANT_PROPERTY(CFGroupModel *, groupModel)
    QML_CONSTANT_PROPERTY(QQmlObjectListProxyModel *, positiveGroups)
    QML_CONSTANT_PROPERTY(QQmlObjectListProxyModel *, negativeGroups)
    QML_CONSTANT_PROPERTY(CFDownloadedGroupProxy *, downloadedGroupProxy)
    QML_CONSTANT_PROPERTY(CFInterestModel *, interestModel)
    QML_CONSTANT_PROPERTY(CFInterestCategoryModel *, interestCategoryModel)
    QML_CONSTANT_PROPERTY(CFInterestFilterProxyModel *, interestSearchModel)
    QML_WRITABLE_PROPERTY(QList<int>, interests)
    QML_READONLY_PROPERTY(int, userGroupsCount)
    QML_WRITABLE_PROPERTY(bool, interestsDirty)
    QML_READONLY_PROPERTY(CFLoopedChunkDownloader *, loopedChunkDownloader)
    QML_READONLY_PROPERTY(int, maxTimeToDownloadAllMembers) // In secs
    Q_PROPERTY(int categoriesCount READ categoriesCount NOTIFY categoriesCountChanged)

    /*!
        \brief Pointer to current assertion handler, nullptr otherwise
    */
    QML_READONLY_PROPERTY(CFGroupAssertionHandler *, assert)

public:
    explicit CFGroupManager(QObject *parent = nullptr);
    int categoriesCount() const;

    static const int k_groupCountForAutomatic;
    static const int k_maxSubInGroup = 1000000;

public slots:
    int allottedTimeForTopAnalysys();

    /*!
     * \brief Starts automatic mode execution
     */
    void startAutomatic(int allottedTimeInSecs);

    /**
     * @brief Fetches groups of current user, compares with
     * currently stored and persists new ones
     */
    void refreshUserGroups(int count);

    /*!
     * \brief Gets groups for analysys for automatic modes
     *
     * At present, only computes max time needed to download all memebers of all groups
     */
    void getGroupsForAnalysys();

    /**
     * @brief Asserts data validity of each dirty row of the model
     */
    void assertModelValidity();

    CFInterestProxyModel *interestByCategoryModel(int index);

    /*!
     * \brief Calls select all models, gets interests from settings
     * and does other user dependent tasks
     */
    void init();

signals:
    /*!
     * \brief userInterestDownloadFinished - Emitted when user interest downloaded
     * by refreshUserGroups method
     * \param items - list of interest ids
     */
    void userInterestDownloadFinished(QList<int> items);


    void userGroupsRefreshed();

    /**
     * @brief Emitted when manager finishes assertion of the model data
     * @param error True if at least one error occured during execution
     */
    void modelValidityAssertionFinished(bool error);

    /**
     * @brief Emitted when general error occured
     * @param message Message describing problem
     */
    void error(const QString &message);

    void opError(CFOpError *error);

    /**
     * @brief Emitted when error occured with a group
     * @param sourceRow Row index inside a source model
     * @param message Message describing problem
     */
    void errorAt(int sourceRow, const QString &message);

    void categoriesCountChanged();

    void automaticFinished();

    void initialized();

private:
    void refreshUserGroupsCount();

    void initInterestProxies();

    void initPositiveGroups();
    void initNegativeGroups();

    void refreshUserGroupsSuccess(QSharedPointer<QIODevice> replyData,
                          const QMap<QByteArray,  QByteArray> &rawHeaders);

    void getGroupsForAnalysysSuccess(QSharedPointer<QIODevice> replyData,
                              const QMap<QByteArray,  QByteArray> &rawHeaders);

    void errorWithServerOrConnection(int  error,
                                     const  QString  &errorMessage);

    /*!
     * \brief Given JSON group object returns photo_200 value if it is defined
     * otherwise returns placeholder photo
     */
    static QString parsePhoto(const QJsonObject &jsGroup);


    /**
     * @brief m_interestProxyModels list of proxy with diffrent categories
     */
    QList<CFInterestProxyModel *> m_interestProxyModels;

    /*!
     * \brief True if automatic mode was started and not yet ended
     */
    bool m_automaticMode = false;
    /*!
     * \brief Time allotted for automatic mode
     */
    int m_allottedTimeInSecs = 0;
    int m_allottedTimeInSecsForTopAnalyze = 0;
};

#endif // CFGROUPMANAGER_H
