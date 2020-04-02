#include "cf.agesearchhandler.h"

#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlError>
#include <QDebug>

#include "network/cf.request.h"
#include "managers/cf.vkauthmanager.h"
#include "managers/cf.groupmanager.h"
#include "managers/cf.usermanager.h"

CFAgeSearchHandler::CFAgeSearchHandler(QObject *parent)
    : QObject(parent),
      m_allRequestCount(0),
      m_processedRequestCount(0)
{
    CFGroupModel *groupModel = CFGroupManager::instance().get_groupModel();

    initUserSet();

    for (int i = 0; i < groupModel->count(); i++){
        if (groupModel->at(i)->downloaded())
            m_groupList.append(groupModel->at(i));
    }


    m_groupIndex = 0;

    QTimer::singleShot(0,[this](){
        m_fromAge != 0 ? next() : emit finished();
    });
}

double CFAgeSearchHandler::progress() const
{
    return m_allRequestCount ? static_cast<double>(m_processedRequestCount) / m_allRequestCount : 0;
}

int CFAgeSearchHandler::estimatedDuration() const
{
    int remainingCount = std::max(m_allRequestCount - m_processedRequestCount, 0);
    double requestsPerSecond = 1000. / CFRequestSender::k_intervalBetweenRequest;
    return std::round(remainingCount / requestsPerSecond);
}

void CFAgeSearchHandler::next()
{
    makeSearchRequest(0);
    if (m_groupList.at(m_groupIndex)->get_subCount() > 1000) {
        makeSearchRequest(1);
    }

    if (m_relationIndex < m_relationList.count() - 1) {
        m_relationIndex++;
    } else if (m_cityIndex < m_cityList.count() - 1) {
        m_relationIndex = 0;
        m_cityIndex++;
    } else {
        m_relationIndex = 0;
        m_cityIndex = 0;
        m_groupIndex++;
    }

    if (m_groupIndex >= m_groupList.count()) {
        emit progressChanged();
        return;
    }

    next();
}

void CFAgeSearchHandler::makeSearchRequest(int sort)
{
    QUrl url(QStringLiteral("/method/users.search"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("group_id"), QString::number(m_groupList.at(m_groupIndex)->get_uid()));
    query.addQueryItem(QStringLiteral("age_from"), QString::number(m_fromAge));
    query.addQueryItem(QStringLiteral("age_to"), QString::number(m_toAge));
    query.addQueryItem(QStringLiteral("count"), "1000");
    query.addQueryItem(QStringLiteral("sort"), QString::number(sort));
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());

    if (m_country != INVALID_VALUE && m_country) {
        query.addQueryItem(QStringLiteral("country"), QString::number(m_country));
    }

    if (!m_cityList.isEmpty()) {
        query.addQueryItem(QStringLiteral("city"), QString::number(m_cityList.at(m_cityIndex)));
    }

    if (!m_relationList.isEmpty()) {
        query.addQueryItem(QStringLiteral("status"), QString::number(m_relationList.at(m_relationIndex)));
    }

    if (m_sex != Personal::Sex::Both && m_sex != Personal::Sex::Undefined) {
        query.addQueryItem(QStringLiteral("sex"), QString::number(m_sex));
    }

    if (m_photo) {
        query.addQueryItem(QStringLiteral("has_photo"), "1");
    }

    url.setQuery(query);

    m_allRequestCount++;

    CFRequest *req = new CFRequest(url);
    connect(req, &CFRequest::success, this, &CFAgeSearchHandler::parseUsers);
    connect(req, &CFRequest::error, this, &CFAgeSearchHandler::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFAgeSearchHandler::parseUsers(QSharedPointer<QIODevice> replyData, const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        handleError(false, "[CFAgeSearchHandler::parseSubscriptions] Could not open reply data",
                    tr("Не получилось обработать ответ сервера"));
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    QJsonObject jsResponse = replyAsObject.value(QLatin1String("response")).toObject();

    QJsonArray jsUsers = jsResponse.value(QLatin1String("items")).toArray();

    for (const QJsonValue user : jsUsers) {
        int id = user[QLatin1String("id")].toInt();
        if (m_userIdSet.contains(id))
            m_matchedIdSet.insert(id);
    }

    m_processedRequestCount++;
    emit progressChanged();

    if (m_processedRequestCount == m_allRequestCount) {
        updateAges();
        emit finished();
    }
}

void CFAgeSearchHandler::initUserSet()
{
    CFUserModel *userModel = CFUserManager::instance().get_model();
    m_country = userModel->get_country();
    m_sex = userModel->get_sex();
    m_fromAge = userModel->get_fromAge();
    m_toAge = userModel->get_toAge();
    for (const QJsonValue val : userModel->get_city())
        m_cityList << val["id"].toInt();
    m_cityIndex = 0;
    m_relationList = userModel->get_relation();
    m_relationIndex = -1;
    m_photo = userModel->get_photo();

    QString whereClause = userModel->whereClause(false, false, false);
    QString queryString(QStringLiteral("SELECT DISTINCT id FROM Users %1 (bdate IS NULL OR inaccurateAge=true);"));
    queryString = queryString.arg(whereClause.isEmpty() ? "WHERE" : whereClause + " AND");

    QSqlQuery query(queryString);
    if (!query.exec()) {
        handleSqlError("CFAgeSearchHandler::initUserSet() sql query failed", query.lastError().text());
        return;
    }

    while (query.next()) {
        m_userIdSet.insert(query.value(QLatin1String("id")).toInt());
    }
}

void CFAgeSearchHandler::updateAges()
{
    QDate bdate = QDate::currentDate().addYears(-((m_fromAge + m_toAge) / 2));
    bdate.setDate(bdate.year(), 1, 1);

    QList<int> idList = m_matchedIdSet.toList();
    QString idString;
    for (int &id : idList) {
        idString += QString("%1,").arg(id);
    }
    idString.chop(1);

    QString queryString = QStringLiteral("UPDATE Users SET bdate=%1, inaccurateAge=0 WHERE id IN (%2)").arg(bdate.toJulianDay()).arg(idString);
    QSqlQuery query(queryString);
    if (!query.exec()) {
        handleSqlError("CFAgeSearchHandler::updateAges() sql query failed:", query.lastError().text());
    }
}

void CFAgeSearchHandler::handleError(bool safe, const QString &technicalMessage, const QString &userfriendlyMessage, int code)
{
    qWarning() << technicalMessage;
    emit error(new CFOpError(code, userfriendlyMessage, technicalMessage));

    if (!safe) {
        emit finished(false);
    }
}

void CFAgeSearchHandler::errorWithServerOrConnection(int error, const QString &errorMessage)
{
    if (error == QNetworkReply::OperationCanceledError) {
        // usual error when we cancel operation; we don't need to handle it
        return;
    }

    bool safe = true;
    QString message;
    QString debugMessage;
    if (error > QNetworkReply::UnknownServerError) {
        int vkError = error - QNetworkReply::UnknownServerError;

        debugMessage = "[CFAgeSearchHandler::errorWithServerOrConnection] VK API error: "
                + QString::number(vkError) + "; message: " + errorMessage;

        switch (vkError) {
        case CFAPIError::ACCESS_DENIED:
            message = tr("Ошибка доступа");
            break;
        case CFAPIError::TOO_MANY_REQUESTS:
            message = tr("Слишком много запросов в секунду");
            break;
        default:
            message = tr("Неизвестная ошибка VK API");
        }
    } else {
        debugMessage = "[CFAgeSearchHandler::errorWithServerOrConnection] Network error: "
                + QString::number(error) + "; message: " + errorMessage;
        message = tr("Сетевая ошибка");
    }

    handleError(safe, debugMessage, message, error);
}

void CFAgeSearchHandler::handleSqlError(const QString &technicalMessage, const QString &userfriendlyMessage)
{
    qWarning() << technicalMessage;
    emit error(new CFOpError(INVALID_VALUE, userfriendlyMessage, technicalMessage));
    QTimer::singleShot(0, [this]{ emit finished(false); });
}
