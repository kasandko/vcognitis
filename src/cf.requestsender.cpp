#include "cf.requestsender.h"

#include <network/cf.request.h>

CFRequestSender::CFRequestSender(const QUrl &host, QObject *parent)
    : QObject(parent),
      m_host(host)
{
    // Queued connection is mandatory
    connect(&m_network, &CFNetwork::error, this,
            &CFRequestSender::onRequestFailed, Qt::QueuedConnection);

    // Set up limit timer and start it
    m_requestsPerSecondTimer.setInterval(k_intervalBetweenRequest);
    m_requestsPerSecondTimer.setSingleShot(false);
    connect(&m_requestsPerSecondTimer, &QTimer::timeout, this, &CFRequestSender::sendNext);
    m_requestsPerSecondTimer.start();

    connect(&m_network, &CFNetwork::needResend, [this](CFRequest *req) {
        m_resendQueue.append(req);
    });
}

void CFRequestSender::modifyRequest(CFRequest *request)
{
    QNetworkRequest &qRequest = request->qRequest();

    qRequest.setUrl(m_host.url() + request->url().url());
    for (auto it = m_sharedHeaders.begin(); it != m_sharedHeaders.end(); it++) {
        request->qRequest().setHeader(it.key(), it.value());
    }
}

void CFRequestSender::sendNext()
{
    if (!m_queue.isEmpty() || !m_resendQueue.isEmpty()) {
        if (!m_resendQueue.isEmpty()) {
            // if we have any request that should be resend - resend them immediately
            return m_network.send(m_resendQueue.takeFirst());
        }
        // Or send next Request
        m_network.send(m_queue.takeFirst());
    }

    // Do nothing if we have not any request to send.
    // TODO: Make some way to reduce blank shoots
}

QUrl CFRequestSender::host() const
{
    return m_host;
}

void CFRequestSender::setHost(const QUrl &host)
{
    if (m_host != host) {
        m_host = host;
    }
}

void CFRequestSender::pause()
{
    m_requestsPerSecondTimer.stop();
}

void CFRequestSender::resume()
{
    if (!m_requestsPerSecondTimer.isActive()) {
        m_requestsPerSecondTimer.setInterval(k_intervalBetweenRequest);
        m_requestsPerSecondTimer.setSingleShot(false);
        m_requestsPerSecondTimer.start();
    }
}

void CFRequestSender::addSharedHeader(QNetworkRequest::KnownHeaders header, const QVariant &value)
{
    m_sharedHeaders[header] = value;
}

void CFRequestSender::removeSharedHeader(QNetworkRequest::KnownHeaders header)
{
    m_sharedHeaders.remove(header);
}

QMap<QNetworkRequest::KnownHeaders, QVariant> CFRequestSender::sharedHeaders() const
{
    return m_sharedHeaders;
}

void CFRequestSender::send(CFRequest *request)
{
    // Add any shared data before enqueuing
    modifyRequest(request);
    m_queue.append(request);
}

void CFRequestSender::clearQueue()
{
    qDeleteAll(m_queue);
    m_queue.clear();
    m_network.abortAll();
}

void CFRequestSender::onRequestFailed(QNetworkReply::NetworkError error,
                                    const QString &errorMessage)
{
    Q_UNUSED(error)
    emit this->error(errorMessage);
    // You can do any special processing logic here on request error
}
