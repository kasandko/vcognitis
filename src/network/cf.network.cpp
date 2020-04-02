#include "cf.network.h"
#include <QHttpMultiPart>
#include <QJsonObject>
#include <QJsonDocument>

#include "cf.global.h"
#include "network/cf.request.h"
#include "cf.networkutils.h"

QString timeoutProperty = "timeoutProperty";

CFNetwork::CFNetwork(QObject *parent)
    : QObject(parent)
{
    // TODO handle redirects manualy if needed (will be done on RequestSender level)
    m_networkManager.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    connect(&m_networkManager, &QNetworkAccessManager::finished, this, &CFNetwork::processReply);
}

CFNetwork::~CFNetwork()
{
    /* Abort emits QNetworkReply::finished so Network::processReply
     * will follow and cleanup will be done there */

    for (QNetworkReply *reply : m_replyRequestMap.keys()) {
        reply->abort();
        QTimer *timer = m_replyTimerMap[reply];

        if (timer != nullptr) {
            timer->stop();
        }
    }
}

void CFNetwork::cleanup()
{
    m_buffer.reset();
}

void CFNetwork::setTimeoutInterval(int timeoutInterval)
{
    if (m_timeoutInterval != timeoutInterval) {
        m_timeoutInterval = timeoutInterval;
    }
}

int CFNetwork::timeoutInterval() const
{
    return m_timeoutInterval;
}

void CFNetwork::send(CFRequest *request)
{
    QNetworkReply *reply = nullptr;
    switch (request->operation()) {
    case QNetworkAccessManager::GetOperation: reply = get(request); break;
    case QNetworkAccessManager::PostOperation: reply = post(request); break;
    default:
        emit error(QNetworkReply::OperationCanceledError, tr("Failed to send request"));
    }

    if (reply == nullptr) {
        // If we are here then one and only error signal was already sent
        request->error(QNetworkReply::UnknownNetworkError, "Can't send request");
        return;
    }

    reply->setProperty(timeoutProperty.toUtf8(), false);

    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [timer, this]() {
       handleTimeout(timer);
    });
    timer->start(timeoutInterval());

    connect(reply, &QNetworkReply::downloadProgress, request, &CFRequest::downloadProgress);
    connect(reply, &QNetworkReply::uploadProgress, request, &CFRequest::uploadProgress);

    m_replyRequestMap.insert(reply, request);
    m_replyTimerMap.insert(reply, timer);
}

void CFNetwork::abortAll()
{
    for (QNetworkReply *reply : m_replyRequestMap.keys()) {
        reply->abort();
    }
}

void CFNetwork::processReply(QNetworkReply *reply)
{
    QTimer *timer = m_replyTimerMap.value(reply, nullptr);

    if (timer != nullptr) {
        timer->stop();
    }
    CFRequest *request = m_replyRequestMap.value(reply, nullptr);

    if (request == nullptr)  {
        cleanup();
        emit error(QNetworkReply::UnknownNetworkError, "Can't find request by given reply");
        return;
    }

    bool isNeedResend = true;

    if (reply->error() != QNetworkReply::NoError || reply->property(timeoutProperty.toUtf8()).toBool()) {
        emit request->error(reply->error(), reply->errorString());

        cleanup();
        // Will be resend
    } else {
        QByteArray data =reply->readAll();

        QBuffer *buffer = new QBuffer();
        buffer->open(QIODevice::ReadWrite);
        buffer->write(data);

        QJsonObject replyAsObject = QJsonDocument::fromJson(data).object();

        if (!replyAsObject.contains(QLatin1String("error"))) {
            isNeedResend = false;
            emit request->success(QSharedPointer<QIODevice>(buffer),
                                  NetworkUtils::headerPairsList2Map(reply->rawHeaderPairs()));
        } else {
            QJsonObject error = replyAsObject.value(QLatin1String("error")).toObject();
            int errorCode = error.value(QLatin1String("error_code")).toInt();

            switch (errorCode) {
            case CFAPIError::TOO_MANY_REQUESTS:
                emit request->error(QNetworkReply::UnknownServerError
                                    + CFAPIError::TOO_MANY_REQUESTS,
                                    "Too many request per second");

                // Will be resend
                break;
            case CFAPIError::ACCESS_DENIED:
                isNeedResend = false;
                emit request->error(QNetworkReply::UnknownServerError
                                    + CFAPIError::ACCESS_DENIED,
                                    "Access denied");
                // Do not resend
                break;
            case CFAPIError::DELETED_OR_BANNED:
                isNeedResend = false;
                emit request->error(QNetworkReply::UnknownServerError
                                    + CFAPIError::DELETED_OR_BANNED,
                                    "User is deleted or banned");
                // Do not resend
                break;
            case CFAPIError::PRIVATE_PROFILE:
                isNeedResend = false;
                emit request->error(QNetworkReply::UnknownServerError
                                    + CFAPIError::ACCESS_DENIED,
                                    "Profile is private");
                // Do not resend
                break;
            case CFAPIError::INVALID_GROUP_ID:
                isNeedResend = false;
                emit request->error(QNetworkReply::UnknownServerError
                                    + CFAPIError::INVALID_GROUP_ID,
                                    "Invalid group id");
                // Do not resend
                break;
            case CFAPIError::ACCESS_TO_GROUP_DENIED:
                isNeedResend = false;
                emit request->error(QNetworkReply::UnknownServerError
                                    + CFAPIError::ACCESS_TO_GROUP_DENIED,
                                    "Access to group denied");
                // Do not resend
                break;
            default:
                emit request->error(QNetworkReply::UnknownServerError + errorCode,
                                    "Unknown error");
                // Error that we don't know - just resend
                break;
            }
        }
    }

    m_replyTimerMap.take(reply)->deleteLater();
    m_replyRequestMap.remove(reply);
    reply->deleteLater();


    if (isNeedResend)
        emit needResend(request);
    else {
        request->deleteLater();
    }
}


void CFNetwork::handleTimeout(QTimer *timer)
{
    // TODO: Test and some impovements.
    /* Abort emits QNetworkReply::finished so Network::processReply
       will follow and we handle timeout uniformely with other finish
       cases but to distinguish timeout we have to mark it via m_timeout */

    QNetworkReply *reply = nullptr;

    for (QNetworkReply *item : m_replyTimerMap.keys()) {
        if (m_replyTimerMap[item] == timer){
            reply = item;
            break;
        }
    }

    if (reply == nullptr) {
        qDebug() << "Can't find reply from timeout";
        return;
    }

    reply->setProperty(timeoutProperty.toUtf8(), true);
    qDebug() << "timeout";
    reply->abort();
}

QNetworkReply *CFNetwork::get(CFRequest *request)
{
    return m_networkManager.get(request->qRequest());
}

QNetworkReply *CFNetwork::post(CFRequest *request)
{
    QNetworkReply *reply = nullptr;
    // Only one Request data object should be valid
    // so we look through all of them until find it
    if (QHttpMultiPart *multiPartData = request->multiPartData()) {
        reply = m_networkManager.post(request->qRequest(), multiPartData);
    } else if (QIODevice *largeData = request->largeData()) {
        if (!largeData->open(QIODevice::ReadOnly)) {
            emit error(QNetworkReply::OperationCanceledError, tr("Cannot open upload data for reading"));
            return nullptr;
        }
        reply = m_networkManager.post(request->qRequest(), largeData);
    } else {
        reply = m_networkManager.post(request->qRequest(), request->data());
    }

    return reply;
}
