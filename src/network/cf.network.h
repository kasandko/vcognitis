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

#ifndef CFNETWORK_H
#define CFNETWORK_H

#include <QObject>
#include <QNetworkReply>
#include <QTimer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QBuffer>
#include <QTemporaryFile>

class CFRequest;


/**
 * @brief Lowest layer of network wrapper
 * Responsible for sending requests and handling replies
 * via direct communication with QNetworkAccessManager object
 */
class CFNetwork : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs Network objects, connects to NAM::finished
     * and sets up timeout timer
     */
    explicit CFNetwork(QObject *parent = nullptr);

    /**
     * @brief Aborts current replies
     */
    ~CFNetwork();

    void setTimeoutInterval(int timeoutInterval);
    int timeoutInterval() const;

    /**
     * @brief Sends given request over network
     */
    void send(CFRequest *request);

    void abortAll();

signals:
    /**
     * @brief Emitted on unsuccesful replies
     * @param error Error code
     * @param errorMessage Error message
     */
    void error(QNetworkReply::NetworkError error, const QString &errorMessage);

    void needResend(CFRequest *req);

private slots:
    /**
     * @brief Processes finished reply
     */
    void processReply(QNetworkReply *reply);

    /**
     * @brief Aborts current reply and marks this object that timeout
     * handler was called
     * @param timer Timer that triggered that handler
     */
    void handleTimeout(QTimer *timer);

private:
    /**
     * @brief Resets data devices
     */
    void cleanup();

    /**
     * @brief Wrapper around NAM::get
     */
    QNetworkReply *get(CFRequest *request);

    /**
     * @brief Wrapper around NAM::post
     */
    QNetworkReply *post(CFRequest *request);


    QNetworkAccessManager m_networkManager;

    /**
     * @brief Buffer for received data if data is small
     */
    QScopedPointer<QBuffer> m_buffer;


    QMap<QNetworkReply*, CFRequest*> m_replyRequestMap;


    QMap<QNetworkReply*, QTimer*> m_replyTimerMap;

    int m_timeoutInterval = 15000;
};

#endif // CFNETWORK_H
