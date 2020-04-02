#include "network/cf.request.h"

#include <QHttpMultiPart>

CFRequest::CFRequest(const QUrl &url,
                 QNetworkAccessManager::Operation op,
                 QObject *parent)
    : QObject(parent),
      m_url(url),
      m_op(op)
{

}

QUrl CFRequest::url() const
{
    return m_url;
}

void CFRequest::setUrl(const QUrl &url)
{
    if (m_url != url) {
        m_url = url;
    }
}

QNetworkAccessManager::Operation CFRequest::operation() const
{
    return m_op;
}

void CFRequest::setLargeData(QIODevice *largeData)
{
    if (largeData != m_largeData) {
        m_largeData = largeData;
        // Take ownership
        m_largeData->setParent(this);
    }
}

QIODevice *CFRequest::largeData()
{
    return m_largeData;
}

void CFRequest::setData(const QByteArray &data)
{
    if (data != m_largeData) {
        m_data = data;
    }
}

QByteArray CFRequest::data() const
{
    return m_data;
}

void CFRequest::setMultiPartData(QHttpMultiPart *multiPartData)
{
    if (multiPartData != m_multiPartData) {
        m_multiPartData = multiPartData;
        // Take ownership
        m_multiPartData->setParent(this);
    }
}

QHttpMultiPart *CFRequest::multiPartData()
{
    return m_multiPartData;
}

QNetworkRequest &CFRequest::qRequest()
{
    return m_request;
}
