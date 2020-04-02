#include "cf.scorehandler.h"

#include <cmath>

#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

#include "network/cf.request.h"
#include "cf.utils.h"
#include "models/cf.usermodel.h"
#include "managers/cf.vkauthmanager.h"
#include "managers/cf.groupmanager.h"
#include "managers/cf.usermanager.h"

CFScoreHandler::CFScoreHandler(CFUserModel *userModel, int score, QObject *parent)
    : QObject(parent),
      m_paused(false),
      m_score(score)
{
    connect(this, &CFScoreHandler::progressChanged,
            this, &CFScoreHandler::estimatedDurationChanged);

    initQuery(userModel);
}

double CFScoreHandler::progress() const
{
    return m_count ? std::min(static_cast<double>(m_count - m_userIds.length()) / m_count, 1.) : 0;
}

int CFScoreHandler::estimatedDuration() const
{
    double requestsPerSecond = 1000. / CFRequestSender::k_intervalBetweenRequest;
    return std::round(m_userIds.length() / requestsPerSecond);
}

bool CFScoreHandler::initQuery(CFUserModel *userModel)
{
    m_query.prepare(selectStatement(userModel));

    if (!m_query.exec()) {
        return false;
    }

    m_count = 0;
    while(m_query.next()) {
        m_count++;
        m_userIds.append(m_query.value(QStringLiteral("id")).toInt());
        m_userGroupIds[m_userIds.last()] = m_query.value(QStringLiteral("groupId")).toInt();
    }

    return true;
}

void CFScoreHandler::endIteration(int id)
{
    m_userIds.removeAll(id);

    emit progressChanged();
    if (m_userIds.empty())
        return emit finished();

    emit chunkFinished();
}

void CFScoreHandler::start()
{
    for (int id : m_userIds)
        makeGetSubscriptionsRequest(id);
    emit started();

    if (!m_count) {
        emit finished();
    }
}

void CFScoreHandler::togglePaused()
{
    if (m_paused)
        CFGlobal::vkApiRequestSender().resume();
    else
        CFGlobal::vkApiRequestSender().pause();
    m_paused = !m_paused;
}

void CFScoreHandler::makeGetSubscriptionsRequest(int id)
{
    QUrl url(QStringLiteral("/method/users.getSubscriptions"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("user_id"), QString::number(id));
    query.addQueryItem(QStringLiteral("extended"),  QString::number(1));
    query.addQueryItem(QStringLiteral("fields"), "activity");
    query.addQueryItem(QStringLiteral("count"),  QString::number(CF_GROUPS_COUNT));
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("lang"), "ru");
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);

    req->setProperty("id", id);

    connect(req,  &CFRequest::success,  this,  &CFScoreHandler::parseSubscriptions);
    connect(req,  &CFRequest::error,  this,  &CFScoreHandler::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFScoreHandler::parseSubscriptions(QSharedPointer<QIODevice> replyData,
                                        const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        handleError(false, "[CFScoreHandler::parseSubscriptions] Could not open reply data",
                    tr("Не получилось обработать ответ сервера"));
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    QJsonArray jsResponseItems = replyAsObject.value(QLatin1String("response")).toObject()
            .value(QLatin1String("items")).toArray();

    CFGroupModel *groupModel = CFGroupManager::instance().get_groupModel();
    CFInterestModel *interestModel = CFGroupManager::instance().get_interestModel();

    // QVariantLists are needed to execute queries as batches which improves time performance
    QVariantList groupIds;

    int userId = qobject_cast<CFRequest*>(sender())->property("id").toInt();
    int downloadedGroupId = m_userGroupIds[userId];

    QMap<int, int> interestMap;

    // Following loop adds data of each user-group to respective containers
    // and compute user score and interests on the way
    int score = 0;
    int interestScore = 0;
    bool hasGroupDownloadedFrom = false;
    for (const QJsonValue &val : jsResponseItems) {
        QJsonObject jsGroup = val.toObject();

        QString type = jsGroup.value(QLatin1String("type")).toString();
        if (!(type == "group" || type == "page") // Ignore events and etc.
            || jsGroup.contains(QLatin1String("deactivated")))
            continue;

        int groupId = jsGroup.value(QLatin1String("id")).toInt();
        groupIds << groupId;

        // Add to score if group is present in app user's groups
        int groupIndex = groupModel->find(groupId);
        if (groupIndex != INVALID_INDEX) {
            score += groupModel->data(groupModel->index(groupIndex, 0),
                                      GroupRoles::Weight).toInt();

            if (!hasGroupDownloadedFrom && groupId == downloadedGroupId)
                hasGroupDownloadedFrom = true;

            continue;
        }

        // part of interest
        int interestIdx = interestModel->find(jsGroup[QLatin1String("activity")].toString());

        if (interestIdx == INVALID_INDEX) {
            // This interest not in DB so we just ignore it.
            // qCritical() << "Не найден интерес" << jsGroup[QLatin1String("activity")].toString();
            continue;
        }

        int interest = interestModel->at(interestIdx)->get_uid();

        if (interestMap.contains(interest)) {
            interestMap[interest] += 1;
        } else {
            interestMap[interest] = 1;
        }
    }

    if (!hasGroupDownloadedFrom) {
        groupIds << downloadedGroupId;
        CFGroup *group = groupModel->getByUid(QString::number(downloadedGroupId));
        if (group != nullptr)
            score += group->get_weight();
    }

    QVariantList interestIds;
    QVariantList usersInterestIds;
    QVariantList countInterests;

    QList<int> sortedList = interestMap.keys();

    // sort interest by their count
    std::sort(sortedList.begin(), sortedList.end(), [&interestMap](int left, int right) {
        return interestMap[left] > interestMap[right];
    });

    // take only top 10 interests of user
    for (int key : sortedList.mid(0, 10)) {
        int count = interestMap[key];
        interestIds << key;
        usersInterestIds << userId;
        countInterests << count;

        if (CFGroupManager::instance().get_interests().contains(key)){
            interestScore += 1;
        }
    }


    // Add prepared data to DB
    QSqlDatabase::database().transaction();

    // First user group pairs
    QSqlQuery query;
    query.prepare(QStringLiteral("INSERT INTO UserGroups (userId, groupId) "
                                 "VALUES (%1, :groupId)").arg(userId));
    query.bindValue(QStringLiteral(":groupId"), groupIds);
    if (!query.execBatch()) {
        handleError(false, "[CFScoreHandler::parseSubscriptions] User groups insert failed; reason: "
                    + query.lastError().text(),
                    tr("Не получилось сохранить данные"));
        QSqlDatabase::database().rollback();
        return;
    }

    if (!interestIds.empty()) {
        // Then intererest pairs with count
        query.prepare(QStringLiteral("INSERT INTO UserInterest (userId, interestId, countGroups) "
                                     "VALUES (:userId, :interestId, :count)"));
        query.bindValue(QStringLiteral(":userId"), usersInterestIds);
        query.bindValue(QStringLiteral(":interestId"), interestIds);
        query.bindValue(QStringLiteral(":count"), countInterests);

        if (!query.execBatch()) {
            handleError(false, "[CFScoreHandler::parseSubscriptions] User interest insert failed; reason: "
                        + query.lastError().text(),
                        tr("Не получилось сохранить данные"));
            QSqlDatabase::database().rollback();
            return;
        }
    }


    // Then update user score and mark him as scored
    query.prepare(QStringLiteral("UPDATE Users SET groupScore=:groupScore,interestScore=:interestScore,scored=:scored WHERE id=:id"));
    query.bindValue(QStringLiteral(":groupScore"), score);
    query.bindValue(QStringLiteral(":interestScore"), interestScore);
    query.bindValue(QStringLiteral(":scored"), true);
    query.bindValue(QStringLiteral(":id"), userId);
    if (!query.exec()) {
        handleError(false, "[CFScoreHandler::parseSubscriptions] User update scored failed; reason: "
                    + query.lastError().text(),
                    tr("Не получилось сохранить данные"));
        QSqlDatabase::database().rollback();
        return;
    }

    // Commit everything
    if (!QSqlDatabase::database().commit()) {
        handleError(false, "[CFScoreHandler::parseSubscriptions] Commit transaction failed; reason: "
                    + QSqlDatabase::database().lastError().text(),
                    tr("Не получилось сохранить данные"));
        QSqlDatabase::database().rollback();
        return;
    }

    // Everything is OK, reset attempt count
    endIteration(userId);
}

void CFScoreHandler::handleError(bool safe, const QString &technicalMessage, const QString &userfriendlyMessage, int code)
{
    qWarning() << technicalMessage;
    emit error(new CFOpError(code, userfriendlyMessage, technicalMessage));

    if (!safe) {
        m_paused = false;
        CFGlobal::vkApiRequestSender().clearQueue();
        emit finished(false);
    }
}

void CFScoreHandler::errorWithServerOrConnection(int error,
                                                 const QString &errorMessage)
{
    if (error == QNetworkReply::OperationCanceledError) {
        // usual error when we cancel scoring we don't need to handle it
        return;
    }

    bool proceed = false;
    QString message;
    QString debugMessage;
    if (error > QNetworkReply::UnknownServerError) {
        int vkError = error - QNetworkReply::UnknownServerError;

        debugMessage = "[CFScoreHandler::errorWithServerOrConnection] VK API error: "
                + QString::number(vkError) + "; message: " + errorMessage;

        switch (vkError) {
        case CFAPIError::ACCESS_DENIED:
            // Silently proceed
            proceed = true;
            break;
        case CFAPIError::TOO_MANY_REQUESTS:
            message = tr("Слишком много запросов в секунду");
            break;
        case CFAPIError::DELETED_OR_BANNED:
        case CFAPIError::PRIVATE_PROFILE:
            // Silently proceed
            proceed = true;
            break;
        default:
            message = tr("Неизвестная ошибка VK API");
        }
    } else {
        debugMessage = "[CFScoreHandler::errorWithServerOrConnection] Network error: "
                + QString::number(error) + "; message: " + errorMessage;
        message = tr("Сетевая ошибка");
    }

    if (!proceed) {
        handleError(true, debugMessage, message, error);
    } else {
        endIteration(qobject_cast<CFRequest*>(sender())->property("id").toInt());
    }
}

QString CFScoreHandler::selectStatement(CFUserModel *userModel)
{
    QString whereClause = userModel->whereClause();
    if (whereClause.isEmpty()) {
        whereClause += "WHERE ";
    } else {
        whereClause += " AND ";
    }

    whereClause += m_score != INVALID_VALUE ? QString("groupScore >= %1 AND ").arg(m_score) : "";


    whereClause += "scored=0 LIMIT " + QString::number(userModel->get_limit());

    return "SELECT DISTINCT Users.id, Users.groupId FROM Users " + whereClause;
}
