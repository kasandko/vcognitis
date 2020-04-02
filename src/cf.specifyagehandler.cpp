#include "cf.specifyagehandler.h"

#include <QUrlQuery>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "network/cf.request.h"
#include "managers/cf.vkauthmanager.h"

const int CFSpecifyAgeHandler::k_minAge = 14;
const int CFSpecifyAgeHandler::k_maxAge = 80;

CFSpecifyAgeHandler::CFSpecifyAgeHandler(CFUser *user, QObject *parent)
    : QObject(parent)
{
    m_user = user;

    m_ageFrom = INVALID_VALUE;
    m_ageTo = INVALID_VALUE;
    m_middleFrom = INVALID_VALUE;
    m_middleTo = INVALID_VALUE;

    QTimer::singleShot(0, [this]{
        if (m_user->get_inaccurateAge()) {
            startInaccurateCheck();
        } else {
            startMaxCheck();
        }
    });
}

void CFSpecifyAgeHandler::startInaccurateCheck()
{
    int inaccurateFrom = m_user->property("age").toInt() - 10;
    int inaccurateTo = m_user->property("age").toInt() + 10;

    if (inaccurateFrom >= k_minAge && inaccurateFrom < k_maxAge &&
            inaccurateTo > k_minAge && inaccurateTo <= k_maxAge) {
        m_ageFrom = inaccurateFrom;
        m_ageTo = inaccurateTo;
        next(true);
    } else {
        startMaxCheck();
    }
}

void CFSpecifyAgeHandler::startMaxCheck()
{
    m_ageFrom = k_maxAge;
    makeSearchRequest();
}

void CFSpecifyAgeHandler::startMinCheck()
{
    m_ageFrom = INVALID_VALUE;
    m_ageTo = k_minAge;
    makeSearchRequest();
}

void CFSpecifyAgeHandler::startNormalCheck()
{
    m_ageFrom = k_minAge + 1;
    m_ageTo = k_maxAge - 1;
    next(true);
}

void CFSpecifyAgeHandler::next(bool success)
{
    if (success) {
        if (m_ageFrom == m_ageTo || m_ageTo == k_minAge) {
            updateUserAge(m_ageTo);
            return;
        }

        if (m_ageFrom == k_maxAge) {
            updateUserAge(m_ageFrom);
            return;
        }

        m_tmp = m_ageTo;
        m_middleTo = m_ageFrom + (m_ageTo - m_ageFrom) / 2;
        m_middleFrom = m_middleTo + 1;
        m_ageTo = m_middleTo;
    } else {
        if (m_ageFrom == k_maxAge) {
            startMinCheck();
            return;
        }

        if (m_ageTo == k_minAge) {
            startNormalCheck();
            return;
        }

        if (m_ageFrom > m_ageTo) {
            m_ageTo = INVALID_VALUE;
            m_middleTo = INVALID_VALUE;
            m_middleFrom = INVALID_VALUE;
            startMaxCheck();
            return;
        }

        m_ageTo = m_tmp;
        m_ageFrom = m_middleFrom;
        m_middleTo = m_ageFrom + (m_ageTo - m_ageFrom) / 2;
        m_middleFrom = m_middleTo + 1;
    }

    makeSearchRequest();
}

void CFSpecifyAgeHandler::makeSearchRequest()
{
    QUrl url(QStringLiteral("/method/users.search"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("group_id"), QString::number(m_user->get_groupId()));
    query.addQueryItem(QStringLiteral("q"), m_user->get_firstName() + " " + m_user->get_lastName());
    query.addQueryItem(QStringLiteral("count"), "1000");
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());

    if (m_ageFrom != INVALID_VALUE)
        query.addQueryItem(QStringLiteral("age_from"), QString::number(m_ageFrom));

    if (m_ageTo != INVALID_VALUE)
        query.addQueryItem(QStringLiteral("age_to"), QString::number(m_ageTo));

    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req, &CFRequest::success, this, &CFSpecifyAgeHandler::parseUsers);
    connect(req, &CFRequest::error, this, &CFSpecifyAgeHandler::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFSpecifyAgeHandler::parseUsers(QSharedPointer<QIODevice> replyData, const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        handleCriticalError("[CFAgeSearchHandler::parseSubscriptions] Could not open reply data",
                    tr("Не получилось обработать ответ сервера"));
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    QJsonObject jsResponse = replyAsObject.value(QLatin1String("response")).toObject();

    if (jsResponse.value(QLatin1String("count")).toInt() == 0) {
        next(false);
        return;
    }

    QJsonArray jsUsers = jsResponse.value(QLatin1String("items")).toArray();

    for (const QJsonValue user : jsUsers) {
        if (m_user->get_vkId() == user[QLatin1String("id")].toInt()) {
            next(true);
            return;
        }
    }

    next(false);
}

void CFSpecifyAgeHandler::updateUserAge(int age)
{
    QDate bdate = QDate::currentDate().addYears(-age);
    bdate.setDate(bdate.year(), 1, 1);

    m_user->set_bdate(bdate.toJulianDay());
    m_user->set_inaccurateAge(false);

    QSqlQuery query(QStringLiteral("UPDATE Users SET bdate=%1, inaccurateAge=0 WHERE id=%2")
                    .arg(m_user->get_bdate()).arg(m_user->get_vkId()));
    if (!query.exec()) {
        handleCriticalError("CFSpecifyAgeHandler::updateUserAge() sql query failed:", query.lastError().text());
    }

    emit finished();
}

void CFSpecifyAgeHandler::handleError(const QString &technicalMessage, const QString &userfriendlyMessage, int code)
{
    qWarning() << technicalMessage;
    emit error(new CFOpError(code, userfriendlyMessage, technicalMessage));
}

void CFSpecifyAgeHandler::handleCriticalError(const QString &technicalMessage, const QString &userfriendlyMessage)
{
    qWarning() << technicalMessage;
    emit error(new CFOpError(INVALID_VALUE, userfriendlyMessage, technicalMessage));
    emit finished(false);
}

void CFSpecifyAgeHandler::errorWithServerOrConnection(int error, const QString &errorMessage)
{
    if (error == QNetworkReply::OperationCanceledError) {
        // Expected error when we cancel operation; we don't need to handle it
        return;
    }

    bool safe = true;
    QString message;
    QString debugMessage;
    if (error > QNetworkReply::UnknownServerError) {
        int vkError = error - QNetworkReply::UnknownServerError;

        debugMessage = "[CFSpecifyAgeHandler::errorWithServerOrConnection] VK API error: "
                + QString::number(vkError) + "; message: " + errorMessage;

        switch (vkError) {
        case CFAPIError::ACCESS_DENIED:
            message = tr("Ошибка доступа");
            safe = false;
            break;
        case CFAPIError::TOO_MANY_REQUESTS:
            message = tr("Слишком много запросов в секунду");
            break;
        default:
            message = tr("Неизвестная ошибка VK API");
        }
    } else {
        debugMessage = "[CFSpecifyAgeHandler::errorWithServerOrConnection] Network error: "
                + QString::number(error) + "; message: " + errorMessage;
        message = tr("Сетевая ошибка");
    }

    if (safe) {
        handleError(debugMessage, message, error);
    } else {
        handleCriticalError(debugMessage, message);
    }
}
