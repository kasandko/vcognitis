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

#ifndef CFREQUESTSENDER_H
#define CFREQUESTSENDER_H

#include <QObject>
#include <QLinkedList>
#include <QQueue>
#include <QTimer>

#include "network/cf.network.h"

class CFRequest;

/**
 * @brief CFRequestSender - class that should handle any interactions with
 * network. Internally implementet via queue and
 * timer(that sends request without awaiting previous request)
 */

class CFRequestSender : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs RequestSender object, sets up connections
     * and sets up request per second limit timer
     */
    explicit CFRequestSender(const QUrl &host = QUrl(), QObject *parent = nullptr);

    QUrl host() const;
    void setHost(const QUrl &host);

    void pause();
    void resume();

    void addSharedHeader(QNetworkRequest::KnownHeaders header, const QVariant &value);
    void removeSharedHeader(QNetworkRequest::KnownHeaders header);
    QMap<QNetworkRequest::KnownHeaders, QVariant> sharedHeaders() const;

    /**
     * @brief Sends given request
     * Internally, puts it into request queue
     */
    void send(CFRequest *request);

    void clearQueue();

    static const int constexpr k_intervalBetweenRequest = 334;

signals:
    void error(QString errorMessage);

private slots:

    /**
     * @brief Callback for Network::error
     * @param error Error code
     * @param errorMessage Error message
     */
    void onRequestFailed(QNetworkReply::NetworkError error, const QString &errorMessage);

    /**
     * @brief Tries to send next request in queue
     */
    void sendNext();

private:
    /**
     * @brief Add any shared data set in this RequestSender to given request
     */
    void modifyRequest(CFRequest *request);

    /**
     * @brief Shared host of all requests sent from this RequestSender
     * Will be prepended to Request::qRequest's URL before enqueue
     */
    QUrl m_host;

    /**
     * @brief Shared headers of all requests sent from this RequestSender
     * Will be added to Request::qRequest headers before enqueue
     */
    QMap<QNetworkRequest::KnownHeaders, QVariant> m_sharedHeaders;

    CFNetwork m_network;

    /**
     * @brief True if request is in progress and no request can be sent at moment
     */
    bool m_locked = false;

    /**
     * @brief Request queue
     * This object owns enqueued pointers
     */
    QLinkedList<CFRequest *> m_queue;

    /**
     * @brief Request queue
     * This object owns enqueued pointers for request that should be resend
     */
    QQueue<CFRequest *> m_resendQueue;

    /**
     * @brief Refreshed request per second count
     */
    QTimer m_requestsPerSecondTimer;
};

#endif // CFREQUESTSENDER_H
