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

#ifndef CF_REQUEST_H
#define CF_REQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

/**
 * @brief Encapsulates data needed to put request into queue of request sender
 */
class CFRequest : public QObject
{
    Q_OBJECT

public:

    explicit CFRequest(const QUrl &url,
            QNetworkAccessManager::Operation op = QNetworkAccessManager::GetOperation,
            QObject *parent = nullptr);

    QUrl url() const;
    void setUrl(const QUrl &url);

    QNetworkAccessManager::Operation operation() const;

    void setLargeData(QIODevice *largeData);
    QIODevice *largeData();

    void setData(const QByteArray &data);
    QByteArray data() const;

    void setMultiPartData(QHttpMultiPart *multiPartData);
    QHttpMultiPart *multiPartData();

    /**
     * @brief Return mutable reference to underlying QNetworkRequest object
     */
    QNetworkRequest &qRequest();

signals:
    /**
     * @brief Emitted if reply was succesful (NoError)
     * @param replyData Pointer to received data, ownership is transferred to receiver
     * @param rawHeaders Reply's raw headers map
     */
    void success(QSharedPointer<QIODevice> replyData,
                 const QMap<QByteArray, QByteArray> &rawHeaders);

    /**
     * @brief Emitted if reply was unsuccesful(any error or timeout)
     * @param error Error code
     * @param errorMessage Error message
     */
    void error(int error, const QString &errorMessage);

    /**
     * @brief See QNetworkReply::uploadProgress doc
     */
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

    /**
     * @brief See QNetworkReply::downloadProgress doc
     */
    void downloadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    /**
     * @brief Destination URL
     * Actual, final URL (of QNetworkRequest object) may be changed by sender
     * but this url will remain unchanged
     */
    QUrl m_url;

    QNetworkAccessManager::Operation m_op;

    /**
     * @brief Pointer to data that needs to be uploaded, used for big sized of data
     * This object owns the pointer
     * If it is not null then multipart data and byte array data must be null
     */
    QIODevice *m_largeData = nullptr;

    /**
     * @brief Data that needs to be uploaded, used for small sized data
     * If it is valid then multipart data and large data must be null
     */
    QByteArray m_data;

    /**
     * @brief Multipart data that needs to be uploaded
     * This object owns the pointer
     * If it is not null then large data and data must be null
     */
    QHttpMultiPart  *m_multiPartData = nullptr;

    /**
     * @brief Underlying QNetworkRequest object
     * As it has many properties required here it is easier to store QNetworkRequest
     * instead of mirroring them
     */
    QNetworkRequest m_request;
};

#endif // CF_REQUEST_H
