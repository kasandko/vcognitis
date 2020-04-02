#include "cf.downloadhandler.h"

#include <cmath>

#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>

#include "network/cf.request.h"
#include "cf.utils.h"
#include "cf.database.h"
#include "managers/cf.vkauthmanager.h"
#include "managers/cf.usermanager.h"

const int CFDownloadHandler::k_offsetStep = 1000;


CFDownloadHandler::CFDownloadHandler(CFGroup *group,
                                     QObject *parent)
    : QObject(parent),
      m_group(group),
      m_requestCount(0),
      m_friendsAndMyself(CFUserManager::instance().friendsAndMyself())
{
    resetAllRequestCount();
}

CFDownloadHandler::~CFDownloadHandler()
{
    // Wait for all watchers of DownloadHandler, if we don't app will crash
    for(QFutureWatcher<ParseMembersResult> *item : m_watcherArray) {
        item->waitForFinished();
    }
}

void CFDownloadHandler::resetAllRequestCount()
{
    m_allRequestCount = 0;
    for (int i = m_group->get_offset(); i < m_group->get_subCount(); i += k_offsetStep) {
        m_allRequestCount++;
    }
    emit allRequestCountChanged(m_allRequestCount);
}

void CFDownloadHandler::start()
{
    m_partlyDownload = false;
    m_offsetDoneVec.clear();
    update_requestCount(0);
    m_inProgress = true;
    emit started();

    m_offsetDoneVec.append(m_group->get_offset());

    for (int i = m_group->get_offset(); i < m_group->get_subCount(); i += k_offsetStep) {
        makeGetMembersRequest(i);
    }
}

void CFDownloadHandler::startPartly(int subCount)
{
    m_partlyDownload = true;
    m_offsetDoneVec.clear();
    update_requestCount(0);
    m_inProgress = true;
    emit started();

    m_offsetDoneVec.append(m_group->get_offset());

    for (int i = m_group->get_offset(), j = 0; i < m_group->get_subCount() && j < subCount; i += k_offsetStep, j += k_offsetStep) {
        m_partlyLastOffset = i;
        makeGetMembersRequest(i);
    }
}

void CFDownloadHandler::stop()
{
    if (m_inProgress) {
        m_inProgress = false;
        m_partlyDownload = false;
        CFGlobal::vkApiRequestSender().clearQueue();
        resetAllRequestCount();
    }
}

void CFDownloadHandler::endIteration(int processedUserCount, int offset)
{
    m_requestCount++;

    if (processedUserCount == 0) return;

    emit chunkFinished(processedUserCount);

    if (offset != -1) {
        m_offsetDoneVec.append(offset + processedUserCount);

        std::sort(m_offsetDoneVec.begin(), m_offsetDoneVec.end());

        int indexToRemove = 0;
        int minOffset = m_offsetDoneVec.first();
        for(int i = 1; i < m_offsetDoneVec.length(); i++) {
            int currentOffset = m_offsetDoneVec[i];

            if (currentOffset - k_offsetStep <= minOffset) {
                minOffset = currentOffset;
                indexToRemove = i;
                continue;
            }
            break;
        }

        if (indexToRemove > 0)
            m_offsetDoneVec.remove(0, indexToRemove);

        if (m_group->get_offset() < m_offsetDoneVec.first())
            m_group->setAndPersistOffset(m_offsetDoneVec.first());

        if (m_group->get_offset() >= m_group->get_subCount() && m_requestCount == m_allRequestCount) {
            emit finished();
            m_inProgress = false;
        } else if (m_partlyDownload && m_offsetDoneVec.first() > m_partlyLastOffset) {
            emit partlyDownloadFinished();
            m_inProgress = false;
            resetAllRequestCount();
        }
    }
}

void CFDownloadHandler::makeGetMembersRequest(int offset)
{
    QUrl url(QStringLiteral("/method/groups.getMembers"));

    QString fields = QStringLiteral("first_name,last_name,deactivated,sex,bdate,"
                                    "city,country,photo_max,domain,last_seen,relation,personal,"
                                    "universities,schools,maiden_name,can_write_private_message");

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("group_id"),  QString::number(m_group->get_uid()));
    query.addQueryItem(QStringLiteral("offset"),  QString::number(offset));
    query.addQueryItem(QStringLiteral("count"),  QString::number(k_offsetStep));
    query.addQueryItem(QStringLiteral("lang"), "ru");
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    query.addQueryItem(QStringLiteral("fields"),  fields);

    url.setQuery(query);

    CFRequest *req = new CFRequest(url);

    req->setProperty("offset", offset);

    connect(req,  &CFRequest::success,  this,  &CFDownloadHandler::parseMembers);
    connect(req,  &CFRequest::error,  this,  &CFDownloadHandler::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFDownloadHandler::parseMembers(QSharedPointer<QIODevice> replyData,
                                     const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        handleError(false, "[CFDownloadHandler::parseMembers] Could not open reply data",
                    tr("Не получилось обработать ответ сервера"));
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    QJsonObject jsResponse = replyAsObject.value(QLatin1String("response")).toObject();

    m_group->setAndPersistSubCount(jsResponse.value(QLatin1String("count")).toInt());

    QJsonArray jsUsers = jsResponse.value(QLatin1String("items")).toArray();
    int senderOffset = qobject_cast<CFRequest*>(sender())->property("offset").toInt();
    int processedCount = jsUsers.count();

    QFutureWatcher<ParseMembersResult> *watcher = new QFutureWatcher<ParseMembersResult>(this);

    // Store watcher
    m_watcherArray.append(watcher);

    connect(watcher, &QFutureWatcher<ParseMembersResult>::finished,
            [senderOffset, processedCount, watcher, this] {
        ParseMembersResult result = watcher->future().result();

        if (result.error != ParseMembersResult::NoError) {
            handleError(false, result.debugErrorMessage, result.errorMessage, result.error);
        } else {
            endIteration(processedCount, senderOffset);
        }
        m_watcherArray.removeAll(watcher);
        watcher->deleteLater();
    });

    // TODO Proper db handling (for example remove database connection on errors)
     watcher->setFuture(QtConcurrent::run([jsUsers, this] {
        QString connectionName = "parseMembers";

        ParseMembersResult result;

        // It is recommended by Qt to scope dbConnection so it would be destroyed automatically
        {
            QSqlDatabase dbConnection = QSqlDatabase::addDatabase(CFDatabase::DB_TYPE,
                                                     connectionName);

            dbConnection.setDatabaseName(CFDatabase::fullPath(CFVkAuthManager::instance().get_userVkId()));

            if (!dbConnection.open()) {
                result.error = ParseMembersResult::Open;
                result.debugErrorMessage = "[CFDownloadHandler::parseMembers] Could not open database";
                result.errorMessage = tr("Не получилось сохранить данные");
                return result;
            }

            // Data will be added directly to database as batches so each column requires a QVariantList
            QVariantList countryIds;
            QVariantList countryNames;

            QVariantList ids;
            QVariantList screenNames;
            QVariantList firstNames;
            QVariantList lastNames;
            QVariantList userCityIds;
            QVariantList userCountryIds;
            QVariantList sexes;
            QVariantList bdates;
            QVariantList photoMaxes;
            QVariantList relations;
            QVariantList politicals;
            QVariantList peopleMains;
            QVariantList lifeMains;
            QVariantList smokings;
            QVariantList alcohols;
            QVariantList religions;
            QVariantList inaccurateAges;
            QVariantList openMessages;

            // For each valid user in resposnse...
            for (const QJsonValue &val : jsUsers) {
                QJsonObject jsUser = val.toObject();

                if (!jsUser.value(QLatin1String("deactivated")).isUndefined()) {
                    continue;
                }

                if (jsUser.value("is_closed").toBool(false)) {
                    continue; // We don't need to parse any closed accounts
                }

                QDateTime lastSeen = QDateTime::fromSecsSinceEpoch(
                    jsUser.value(QLatin1String("last_seen")).toObject()
                            .value(QLatin1String("time")).toInt()
                );

                if (lastSeen.daysTo(QDateTime::currentDateTime()) > 30) {
                    continue; // Was online too long ago
                }

                int id = jsUser.value(QLatin1String("id")).toInt();
                if (m_friendsAndMyself.contains(id)) {
                    continue; // Ignore friends and myself
                }

                QJsonValue jsCountry = jsUser.value(QLatin1String("country"));
                if (!jsCountry.isUndefined()) {
                    QString title = jsCountry[QLatin1String("title")].toString();
                    // Nonexistent country - then ignore it
                    if (title == "") {
                        continue;
                    }
                }

                // ...populate qvariant contantainers with appropriate data
                ids << id;
                screenNames << jsUser.value(QLatin1String("domain")).toString();
                firstNames << jsUser.value(QLatin1String("first_name")).toString();
                lastNames << jsUser.value(QLatin1String("last_name")).toString();

                // Handle country
                QVariant countryId(QVariant::Int);
                if (!jsCountry.isUndefined()) {
                    QString title = jsCountry.toObject().value(QLatin1String("title")).toString();
                    // Nonexistent country - then ignore it
                    if (title == "")
                        continue;

                    countryIds << jsCountry.toObject().value(QLatin1String("id")).toInt();
                    countryNames << jsCountry.toObject().value(QLatin1String("title")).toString();
                    countryId.setValue(countryIds.last());
                }
                userCountryIds << countryId;

                // Handle city
                QJsonValue jsCity = jsUser.value(QLatin1String("city"));

                userCityIds << (jsCity.isUndefined() ? -1 : jsCity.toObject().value(QLatin1String("id")).toInt());


                QJsonValue sex = jsUser.value(QLatin1String("sex"));
                sexes << (sex.isUndefined() ? QVariant(QVariant::Int) : sex.toInt());

                // Handle birthday
                QJsonValue jsBdate = jsUser.value(QLatin1String("bdate"));
                QVariant bdate(QVariant::LongLong);
                QVariant inaccurateAge(QVariant::Bool);
                if (!jsBdate.isUndefined()) {
                    QString dateAsStr = jsBdate.toString();
                    QDate date = dateAsStr.length() <= 5 ? QDate::fromString(dateAsStr, "d.M")
                                                         : QDate::fromString(dateAsStr, "d.M.yyyy");
                    if (date.year() != 1900) { // This means that year is not hidden
                        bdate.setValue(date.toJulianDay());
                    }
                }
                if (bdate.isNull()) { // Try to guess if birthday is hidden
                    int byear = guessBirthYear(jsUser);

                    if (byear > 1900) {
                        bdate.setValue(QDate(byear, 7, 1).toJulianDay());
                        inaccurateAge.setValue(true);
                    }
                }
                bdates << bdate;
                inaccurateAges << inaccurateAge;

                photoMaxes << jsUser.value(QLatin1String("photo_max")).toString();

                openMessages << jsUser.value(QLatin1String("can_write_private_message")).toInt(0);

                // Personal info
                QJsonValue relation = jsUser.value(QLatin1String("relation"));
                if (relation.isUndefined()) {
                    // Educated guess: if maiden_name is set than consider woman married
                    if (jsUser.value(QLatin1String("maiden_name")).toString().isEmpty()) {
                        relations << QVariant(QVariant::Int);
                    } else {
                        relations << Personal::Relation::Married;
                    }
                } else {
                    relations << relation.toInt();
                }

                QJsonValue political = jsUser.value(QLatin1String("personal"))
                        .toObject().value("political");
                politicals << (political.isUndefined() ? QVariant(QVariant::Int) : political.toInt());

                QJsonValue religion = jsUser.value(QLatin1String("personal"))
                        .toObject().value("religion_id");
                religions << (religion.isUndefined() ? QVariant(QVariant::Int) : parseReligion(religion.toInt()));

                QJsonValue peopleMain = jsUser.value(QLatin1String("personal"))
                        .toObject().value("people_main");
                peopleMains << (peopleMain.isUndefined() ? QVariant(QVariant::Int) : peopleMain.toInt());

                QJsonValue lifeMain = jsUser.value(QLatin1String("personal"))
                        .toObject().value("life_main");
                lifeMains << (lifeMain.isUndefined() ? QVariant(QVariant::Int) : lifeMain.toInt());

                QJsonValue smoking = jsUser.value(QLatin1String("personal"))
                        .toObject().value("smoking");
                smokings << (smoking.isUndefined() ? QVariant(QVariant::Int) : smoking.toInt());

                QJsonValue alcohol = jsUser.value(QLatin1String("personal"))
                        .toObject().value("alcohol");
                alcohols << (alcohol.isUndefined() ? QVariant(QVariant::Int) : alcohol.toInt());
            }

            // Now all acquired data will be added to database
            // Note that all queries executed as batches
            // this is done for optimizations (that is why QVariant containers were needed)
            dbConnection.transaction();

            // First insert countries
            QSqlQuery query(dbConnection);
            query.prepare(QStringLiteral("INSERT INTO Countries (id, name) VALUES (:id, :name)"));
            query.bindValue(QStringLiteral(":id"), countryIds);
            query.bindValue(QStringLiteral(":name"), countryNames);
            if (!query.execBatch()) {
                result.error = ParseMembersResult::CountriesInsert;
                result.debugErrorMessage = "[CFDownloadHandler::parseMembers] Country insert failed; reason: "
                        + query.lastError().text();
                result.errorMessage = tr("Не получилось сохранить данные");

                dbConnection.rollback();
                return result;
            }

            // Then users
            QString queryString = QStringLiteral("INSERT INTO Users (id, screenName, firstName, lastName, "
                                          "cityId, countryId, sex, bdate, photoMax, relation, "
                                          "political, peopleMain, lifeMain, smoking, alcohol, "
                                          "religion, inaccurateAge, openMessages, groupId) "
                                          "VALUES (:id, :screenName, :firstName, :lastName, :cityId, "
                                          ":countryId, :sex, :bdate, :photoMax, :relation, :political, "
                                          ":peopleMain, :lifeMain, :smoking, :alcohol, :religion, "
                                          ":inaccurateAge, :openMessages, %1);").arg(m_group->get_uid());
            query.prepare(queryString);
            query.bindValue(QStringLiteral(":id"), ids);
            query.bindValue(QStringLiteral(":screenName"), screenNames);
            query.bindValue(QStringLiteral(":firstName"),  firstNames);
            query.bindValue(QStringLiteral(":lastName"),  lastNames);
            query.bindValue(QStringLiteral(":countryId"), userCountryIds);
            query.bindValue(QStringLiteral(":cityId"), userCityIds);
            query.bindValue(QStringLiteral(":sex"), sexes);
            query.bindValue(QStringLiteral(":bdate"), bdates);
            query.bindValue(QStringLiteral(":photoMax"), photoMaxes);
            query.bindValue(QStringLiteral(":relation"), relations);
            query.bindValue(QStringLiteral(":political"), politicals);
            query.bindValue(QStringLiteral(":religion"), religions);
            query.bindValue(QStringLiteral(":peopleMain"), peopleMains);
            query.bindValue(QStringLiteral(":lifeMain"), lifeMains);
            query.bindValue(QStringLiteral(":smoking"), smokings);
            query.bindValue(QStringLiteral(":alcohol"), alcohols);
            query.bindValue(QStringLiteral(":inaccurateAge"), inaccurateAges);
            query.bindValue(QStringLiteral(":openMessages"), openMessages);
            if (!query.execBatch()) {
                result.error = ParseMembersResult::UsersInsert;
                result.debugErrorMessage = "[CFDownloadHandler::parseMembers] User insert failed; reason: "
                        + query.lastError().text();
                result.errorMessage = tr("Не получилось сохранить данные");

                dbConnection.rollback();
                return result;
            }

            // Finally add processed group to users group list
            queryString = QStringLiteral("INSERT INTO UserGroups (userId, groupId) VALUES (:userId, %1);")
                    .arg(m_group->get_uid());
            query.prepare(queryString);
            query.bindValue(":userId", ids);
            if (!query.execBatch()) {
                result.error = ParseMembersResult::UserGroupsInsert;
                result.debugErrorMessage = "[CFDownloadHandler::parseMembers] User groups insert failed; reason: "
                        + query.lastError().text();
                result.errorMessage = tr("Не получилось сохранить данные");

                dbConnection.rollback();
                return result;
            }

            // Commit everything
            if (!dbConnection.commit()) {
                result.error = ParseMembersResult::Commit;
                result.debugErrorMessage = "[CFDownloadHandler::parseMembers] Commit transaction failed; reason: "
                        + QSqlDatabase::database().lastError().text();
                result.errorMessage = tr("Не получилось сохранить данные");

                dbConnection.rollback();
                return result;
            }
        }
        QSqlDatabase::removeDatabase(connectionName);

        return result;
    }));
}

void CFDownloadHandler::handleError(bool safe, const QString &technicalMessage,
                                    const QString &userfriendlyMessage,
                                    int code)
{
    qWarning() << technicalMessage;
    emit error(new CFOpError(code, userfriendlyMessage, technicalMessage));

    if (!safe) {
        stop();
        emit finished(false);
    }
}

void CFDownloadHandler::errorWithServerOrConnection(int error,
                                                    const QString &errorMessage)
{
    if (error == QNetworkReply::OperationCanceledError) {
        // usual error when we cancel downloading we don't need to handle it
        return;
    }

    bool safe = true;
    QString message;
    QString debugMessage;
    if (error > QNetworkReply::UnknownServerError) {
        int vkError = error - QNetworkReply::UnknownServerError;

        debugMessage = "[CFDownloadHandler::errorWithServerOrConnection] VK API error: "
                + QString::number(vkError) + "; message: " + errorMessage;

        switch (vkError) {
        case CFAPIError::ACCESS_DENIED:
            message = tr("Подписчики скрыты");
            safe = false;
            break;
        case CFAPIError::TOO_MANY_REQUESTS:
            message = tr("Слишком много запросов в секунду");
            break;
        default:
            message = tr("Неизвестная ошибка VK API");
        }
    } else {
        debugMessage = "[CFDownloadHandler::errorWithServerOrConnection] Network error: "
                + QString::number(error) + "; message: " + errorMessage;
        message = tr("Сетевая ошибка");
    }

    handleError(safe, debugMessage, message, error);
}

int CFDownloadHandler::parseReligion(int religionId) const
{
    if (religionId == 167)
        return Personal::Religion::Judaism;
    if (religionId == 102)
        return Personal::Religion::Orthodoxy;
    if (religionId == 101)
        return Personal::Religion::Catholicism;
    if (religionId == 107)
        return Personal::Religion::Protestantism;
    if (religionId == 124)
        return Personal::Religion::Islam;
    if (religionId == 129)
        return Personal::Religion::Buddhism;
    if (religionId == 139)
        return Personal::Religion::Confucianism;
    if (religionId == 200)
        return Personal::Religion::SecularHumanism;
    if (religionId == 201)
        return Personal::Religion::Pastafarianism;

    return Personal::Religion::Undefined;
}

int CFDownloadHandler::guessBirthYear(const QJsonObject &jsUser)
{
    QJsonValue schools = jsUser.value(QLatin1String("schools"));

    if (!schools.isUndefined()) {
        QJsonArray schoolArr = schools.toArray();

        int minYearFrom = 9999;
        int minYearType = -1;
        int maxYearTo = 1900;
        int maxYearType = -1;
        QSet<int> allowedTypes = {0,1,2,8,9,10,11,12,13};
        for (const QJsonValue school : schoolArr) {
            int yearFrom = school[QLatin1String("year_from")].toInt();
            int yearTo = school[QLatin1String("year_to")].toInt();
            int type = school[QLatin1String("type")].toInt();

            if (!allowedTypes.contains(type))
                continue;

            if (yearFrom < minYearFrom) {
                minYearFrom = yearFrom;
                minYearType = type;
            }

            if (yearTo > maxYearTo) {
                maxYearTo = yearTo;
                maxYearType = type;
            }
        }

        if (minYearFrom < 9999 && minYearFrom > 1900) {
            switch (minYearType) {
            case 0:case 1:case 2:
                return minYearFrom - 7;
            case 8:case 9:case 10:case 11:case 12:
                return minYearFrom - 15;
            }
        }

        if (maxYearTo > 1900) {
            switch (maxYearType) {
            case 0:case 1:case 2:
                return maxYearTo - 18;
            case 8:case 9:case 10:case 11:case 12:
                return maxYearTo - 19;
            }
        }
    }

    QJsonValue universities = jsUser.value(QLatin1String("universities"));
    if (!universities.isUndefined()) {
        QJsonArray universityArr = universities.toArray();

        int minGraduation = 9999;
        for (const QJsonValue university : universityArr) {
            int graduation = university["graduation"].toInt();

            if (graduation < minGraduation)
                minGraduation = graduation;
        }

        if (minGraduation < 9999 && minGraduation > 1900)
            return minGraduation - 23;
    }

    return 1900; //return invalid year
}
