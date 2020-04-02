#include "cf.groupassertionhandler.h"

#include <QUrlQuery>
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlError>

#include "network/cf.request.h"
#include "cf.utils.h"
#include "managers/cf.vkauthmanager.h"
#include "managers/cf.groupmanager.h"

const int CFGroupAssertionHandler::k_maxAttempts = 6;

CFGroupAssertionHandler::CFGroupAssertionHandler(QObject *parent) : QObject(parent)
{
    m_groupModel = CFGroupManager::instance().get_groupModel();
    m_assertedRow = 0;
    m_errorOccured = false;
    QTimer::singleShot(0, this, &CFGroupAssertionHandler::assertCurrent);
}

bool CFGroupAssertionHandler::addGroupToDatabase(CFGroup *group, bool isInsert, int oldUid)
{
    QSqlQuery query;

    if (isInsert) {
        query.prepare("INSERT INTO Groups(id, screenName, name, photo, priority, weight, subCount, offset) "
                      "VALUES(:id, :screenName, :name, :photo, :priority, :weight, :sub, :offset)");
    } else {
        query.prepare("UPDATE Groups SET id=:id, screenName=:screenName, name=:name, photo=:photo, "
                      "priority=:priority, weight=:weight, subCount=:sub, offset=:offset "
                      "WHERE id=:oldId");
        query.bindValue(":oldId", oldUid);
    }

    query.bindValue(":id", group->get_uid());
    query.bindValue(":screenName", group->get_screenName());
    query.bindValue(":name", group->get_name());
    query.bindValue(":photo", group->get_photo());
    query.bindValue(":priority", group->get_priority());
    query.bindValue(":weight", group->get_weight());
    query.bindValue(":sub", group->get_subCount());
    query.bindValue(":offset", group->get_offset());

    if (!query.exec()) {
        qWarning() << "[CFGroupAssertionHandler::getByIdSuccess] Can't add or modify group in DB "
                   << query.lastError();
        return false;
    }
    return true;
}

void CFGroupAssertionHandler::assertCurrent()
{
    CFGroup *group = m_groupModel->at(m_assertedRow);

    // Remove vk prefix
    if (group->get_screenName().contains(CFGlobal::VK_HOST_PREFIX + "/")) {
        group->set_screenName(group->get_screenName().remove(CFGlobal::VK_HOST_PREFIX + "/").trimmed());
    }

    // if id looks like publicXXXXXX then change it to clubXXXXXX
    QRegExp reg("^public[0-9]+$");
    reg.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    if (reg.indexIn(group->get_screenName()) != INVALID_INDEX) {
        group->set_screenName(group->get_screenName().replace("public", "club"));
    }

    // if screenName contains only id
    if (QRegExp("^[0-9]+$").indexIn(group->get_screenName()) != INVALID_INDEX) {
        group->set_screenName("club" + group->get_screenName());
    }

    if (!m_groupModel->get_dirtyStatus()) {
        // No any changes
        incrementAndProceed();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT screenName from Groups where id=?;");
    query.bindValue(0, group->get_uid());
    query.exec();

    if (query.next() && group->get_screenName() == query.value(0).toString()) {
        incrementAndProceed();
        return;
    }

    if (group->get_uid() != INVALID_VALUE && !group->get_validated()) {
        if (!addGroupToDatabase(group)) {
            m_errorOccured = true;
            emit errorAt(m_assertedRow, tr("Не вышло добавить группу в базу."));
        }

        return incrementAndProceed();
    }

    if (group->get_screenName().trimmed() == "") {
        emit errorAt(m_assertedRow,
                     tr("Необходимо ввести корректный URL группы либо id номер. "
                        "Например: https://vk.com/public12345678 или public12345678"));
        m_errorOccured = true;
        incrementAndProceed();
        return;
    }

    if (!m_groupModel->isUnique(group->get_screenName())) {
        emit errorAt(m_assertedRow, tr("Данная группа уже добавлена"));
        m_errorOccured = true;
        incrementAndProceed();
        return;
    }

    makeResolveScreenNameRequest(group->get_screenName());
}

void CFGroupAssertionHandler::incrementAndProceed()
{
    m_assertedRow++;
    if (m_assertedRow < m_groupModel->count()) {
        assertCurrent();
    } else {
        emit finished(m_errorOccured);
    }
}

void CFGroupAssertionHandler::makeResolveScreenNameRequest(const QString &screenName)
{
    QUrl reqUrl(QStringLiteral("/method/utils.resolveScreenName"));

    QUrlQuery reqQuery;
    // If there club prefix, we should remove club prifix coz it will improve the result
    reqQuery.addQueryItem(QStringLiteral("screen_name"), screenName);
    reqQuery.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    reqQuery.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    reqUrl.setQuery(reqQuery);
    m_lastUrl = reqUrl;

    CFRequest *req = new CFRequest(reqUrl);
    connect(req,  &CFRequest::success,  this,  &CFGroupAssertionHandler::resolveScreenNameSuccess);
    connect(req,  &CFRequest::error,  this,  &CFGroupAssertionHandler::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFGroupAssertionHandler::resolveScreenNameSuccess(QSharedPointer<QIODevice> replyData, const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        errorWithServerOrConnection(QNetworkReply::UnknownServerError,
                                    "[CFGroupAssertionHandler::resolveScreenNameSuccess] Could not open reply data");
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    m_attempt = 0;

    QJsonValue response = replyAsObject[QLatin1String("response")];

    if (response.isObject()) {
        if (response.toObject().value(QLatin1String("type")) != "group") {
            m_errorOccured = true;
            emit errorAt(m_assertedRow, tr("Объект с данным ID не является группой"));
        } else {
            QString screenName = m_groupModel->at(m_assertedRow)->get_screenName();
            makeGetMembersRequest(screenName);
            return;
        }
    } else {
        m_errorOccured = true;
        emit errorAt(m_assertedRow, tr("Группа с данным ID не найдена"));
    }

    incrementAndProceed();
}

void CFGroupAssertionHandler::makeGetMembersRequest(const QString &screenName)
{
    QUrl url(QStringLiteral("/method/groups.getMembers"));

    QString id = screenName;
    // Transform clubXXXX to XXXX otherwise it will fall with CFAPIError::INVALID_GROUP_ID error
    // even on valid groups
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (screenName.startsWith("club") && re.exactMatch(QString(screenName).remove("club"))) {
        id = QString(screenName).remove("club");
    }

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("group_id"),  id);
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req,  &CFRequest::success,  this,  &CFGroupAssertionHandler::getMembersSuccess);
    connect(req,  &CFRequest::error,  this,  &CFGroupAssertionHandler::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFGroupAssertionHandler::getMembersSuccess(QSharedPointer<QIODevice> replyData, const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        errorWithServerOrConnection(QNetworkReply::UnknownServerError,
                                    "[CFGroupAssertionHandler::getMembersSuccess] Could not open reply data");
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    int userCount = 0;

    if (replyAsObject.contains(QLatin1String("response"))) {
        QJsonObject response = replyAsObject[QLatin1String("response")].toObject();
        userCount = response[QLatin1String("count")].toInt();
    }

    m_attempt = 0;

    if (userCount == 0) {
        m_errorOccured = true;
        emit errorAt(m_assertedRow, tr("Нет доступа к подписчикам группы"));
        incrementAndProceed();
        return;
    }

    makeGetByIdRequest(m_groupModel->at(m_assertedRow)->get_screenName());
}

void CFGroupAssertionHandler::makeGetByIdRequest(const QString &screenName)
{
    QUrl reqUrl(QStringLiteral("/method/groups.getById"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("group_id"), screenName);
    query.addQueryItem(QStringLiteral("fields"), QStringLiteral("members_count"));
    query.addQueryItem(QStringLiteral("access_token"),
                       CFVkAuthManager::instance().get_token());
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    reqUrl.setQuery(query);

    CFRequest *req = new CFRequest(reqUrl);
    connect(req,  &CFRequest::success,  this,  &CFGroupAssertionHandler::getByIdSuccess);
    connect(req,  &CFRequest::error,  this,  &CFGroupAssertionHandler::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFGroupAssertionHandler::getByIdSuccess(QSharedPointer<QIODevice> replyData, const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        errorWithServerOrConnection(QNetworkReply::UnknownServerError,
                                    "[CFGroupAssertionHandler::getByIdSuccess] Could not open reply data");
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    m_attempt = 0;

    QJsonObject jsGroup = replyAsObject.value(QLatin1String("response")).toArray().at(0).toObject();
    CFGroup *group = m_groupModel->at(m_assertedRow);

    if (m_groupModel->indexOf(QString::number(jsGroup.value(QLatin1String("id")).toInt())) != INVALID_INDEX) {
        emit errorAt(m_assertedRow, tr("Данная группа уже добавлена"));
        m_errorOccured = true;
        incrementAndProceed();
        return;
    }

    int count = jsGroup.value(QLatin1String("members_count")).toInt();

    if (count > CFGroupManager::instance().k_maxSubInGroup ||  count == 0) {
        if (count == 0) {
            emit errorAt(m_assertedRow, tr("Нет сведений о количестве подписчиков группы"));
        } else {
            emit errorAt(m_assertedRow,
                         tr("В группе слишком много подписчиков(%1)").arg(CFUtils::subsToString(count)));
        }


        m_errorOccured = true;
        incrementAndProceed();
        return;
    }

    bool isInsert = false;

    int oldUid;

    if (group->get_uid() == INVALID_VALUE) {
        isInsert = true;
    } {
        oldUid = group->get_uid();
    }

    group->set_name(jsGroup.value(QLatin1String("name")).toString());
    group->set_screenName(jsGroup.value(QLatin1String("screen_name")).toString());
    group->set_uid(jsGroup.value(QLatin1String("id")).toInt());
    group->set_photo(jsGroup.value(QLatin1String("photo_200")).toString());
    group->set_subCount(jsGroup.value(QLatin1String("members_count")).toInt());

    if (!addGroupToDatabase(group, isInsert, oldUid)) {
        m_errorOccured = true;
        emit errorAt(m_assertedRow, tr("Не вышло добавить группу в базу."));
    }

    incrementAndProceed();
}

void CFGroupAssertionHandler::errorWithServerOrConnection(int error, const QString &errorMessage)
{
    if (error == QNetworkReply::OperationCanceledError) {
        // usual error when we cancel downloading we don't need to handle it
        return;
    }

    bool vkError = false;
    QString message;
    QString debugMessage;
    if (error > QNetworkReply::UnknownServerError) {
        vkError = true;
        error -= QNetworkReply::UnknownServerError;

        debugMessage = "[CFGroupAssertionHandler::errorWithServerOrConnection] VK API error: "
                + QString::number(error) + "; message: " + errorMessage;
    } else {
        debugMessage = "[CFGroupAssertionHandler::errorWithServerOrConnection] Network error: "
                + QString::number(error) + "; message: " + errorMessage;
    }

    qDebug() << debugMessage;

    if (vkError) {
        if (error == CFAPIError::ACCESS_DENIED) {
            m_errorOccured = true;
            emit errorAt(m_assertedRow, tr("Нет доступа к подписчикам группы"));
            incrementAndProceed();
        } else if (error == CFAPIError::INVALID_GROUP_ID) {
            m_errorOccured = true;
            emit errorAt(m_assertedRow, tr("Группа с данным ID не найдена"));
            incrementAndProceed();
        } else if (error == CFAPIError::ACCESS_TO_GROUP_DENIED) {
            m_errorOccured = true;
            emit errorAt(m_assertedRow, tr("У вас нет доступа к этой группе"));
            incrementAndProceed();
        }
    }
}
