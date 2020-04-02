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

#ifndef CFVKAUTHMANAGER_H
#define CFVKAUTHMANAGER_H

#include <QObject>
#include <QDateTime>

#include "QQmlHelpers"

#include "cf.global.h"
#include "cf.requestsender.h"

/**
 * @brief Singleton object responsible for authentication process with VK API
 */
class CFVkAuthManager : public QObject
{
    Q_OBJECT
    SINGLETON(CFVkAuthManager)

    /**
     * @brief VK API access token
     */
    QML_READONLY_PROPERTY(QString, token)

    /**
     * @brief App user's VK id
     */
    QML_READONLY_PROPERTY(QString, userVkId)
public:
    /**
     * @brief Permissions that can be requested from user
     */
    enum Permission
    {
        Notify = 1,
        Friends = 2,
        Photos = 4,
        Audio = 8,
        Video = 16,
        Stories = 64,
        Pages = 128,
        LinkToApp = 256,
        Status = 1024,
        Notes = 2048,
        Messages = 4096,
        Wall = 8192,
        Ads = 32768,
        Offline = 65536,
        Docs = 131072,
        Groups = 262144,
        Notifications = 524288,
        Stats = 1048576,
        Email = 4194304,
        Market = 134217728
    };

    explicit CFVkAuthManager(QObject *parent = nullptr);

signals:
    /**
     * @brief Emitted when new access token is required
     */
    void authRequired();

    /**
     * @brief Emitted when authetication results in error
     */
    void authError(const QString &error, const QString &errorMessage);

    /**
     * @brief Emitted when authetication is successful
     */
    void authSuccess();

    /**
     * @brief loggedOut Emitted when app is logged out
     */
    void loggedOut();

public slots:
    /**
     * @brief loginOut Logout from api
     */
    void loginOut();

    /**
     * @brief Return authentication URL with all parameters set
     */
    static QString authUrl();

    /**
     * @brief Returns authentication redirect URL
     * OAuth system will rederict browser there with provided access token
     */
    static QString authRedirectUrl();

    /**
     * @brief Parses auth redirect URL which will contain token as fragment parameter (or error)
     */
    void parseAuthResult(const QString &redirectUrl);

private slots:
    void makeTrackVisitorRequest();
};

#endif // CFVKAUTHMANAGER_H
