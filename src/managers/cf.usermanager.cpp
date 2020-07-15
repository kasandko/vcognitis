#include "cf.usermanager.h"

#include <QSettings>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

#include "managers/cf.vkauthmanager.h"
#include "managers/cf.groupmanager.h"
#include "network/cf.request.h"
#include "cf.utils.h"
#include "cf.database.h"

#include "cf.global.h"

CFUserManager::CFUserManager(QObject *parent)
    : QObject(parent),
      m_model(new CFUserModel(this)),
      m_favoriteModel(new CFFavoriteModel(this)),
      m_countryModel(new CFCountryModel(this)),
      m_countryProxyModel(new CFCountrySortFilterModel(m_countryModel, this)),
      m_downloadQueue(new CFDownloadQueue(this)),
      m_score(nullptr),
      m_ageSearch(nullptr),
      m_ageSpecifier(nullptr),
      m_currentAgeSpecifyVkId(INVALID_VALUE)
{
    m_notificationPlayer.setMedia(QUrl("qrc:/sounds/notification.mp3"));

    // Downloading process adds new countries so we need to reselect them after download
    // (regardless whether download was successfull or how long was it)
    connect(m_downloadQueue, &CFDownloadQueue::cancelled, [this]{
        m_countryModel->select();
    });
    connect(m_downloadQueue, &CFDownloadQueue::finished, [this]{
        m_countryModel->select();
        m_notificationPlayer.play();
    });
    connect(m_downloadQueue, &CFDownloadQueue::error, [this](CFOpError *error){
        error->update_source(tr("Очередь загрузки"));
        emit opError(error);
    });

    connect(&m_rescoreFutureWatcher, &QFutureWatcher<bool>::finished, [this]{
        update_rescoringActive(false);
        emit rescoreFinished(m_rescoreFutureWatcher.future().result());
    });

    connect(&m_topAnalyzeTimeWatcher, &QFutureWatcher<int>::finished, [this]  {
        if (m_topAnalyzeTimeWatcher.future().result() == INVALID_VALUE)
            return;

        emit this->topAnalyzeTimeFinished(m_topAnalyzeTimeWatcher.future().result());
    });

    // Following connect is to refresh 'favorite' property in main model
    // when user does something with it in favorite model
    connect(m_favoriteModel, &CFUserModel::dataChanged,
            [this] (const QModelIndex &topLeft,
                 const QModelIndex &bottomRight,
                 const QVector<int> &roles) {
        Q_UNUSED(roles)
        Q_ASSERT(topLeft == bottomRight);

        CFUser *user = m_favoriteModel->at(topLeft.row());
        m_model->refreshFavorite(user->get_vkId(), user->get_favorite());
    });

    // Data init will happen upon successful auth, because it depends on active user ID
    connect(&CFVkAuthManager::instance(), &CFVkAuthManager::authSuccess,
            this, &CFUserManager::init);
}

void CFUserManager::rescore(QList<int> interests)
{
    update_rescoringActive(true);
    update_rescoringPercent(0.0);
    m_rescoreFutureWatcher.setFuture(QtConcurrent::run([interests, this]{
        QString connectionName = "rescore";

        double mapFillingWeight = 0.2;
        double interestsMapFillingWeight = 0.2;
        double updateRequestsMapFilingWeight = 0.45;
        // It is recommended by Qt to scope dbConnection so it would be destroyed automatically
        {
            QSqlDatabase dbConnection = QSqlDatabase::addDatabase(CFDatabase::DB_TYPE,
                                                     connectionName);

            dbConnection.setDatabaseName(CFDatabase::fullPath(CFVkAuthManager::instance().get_userVkId()));

            if (!dbConnection.open()) {
                qCritical() << "[CFUserManager::rescore] Could not open database";
                return false;
            }

            // Take all users and calc their initial score
            QSqlQuery query(dbConnection);
            query.prepare(QStringLiteral("SELECT Users.id,SUM(Groups.weight) AS score FROM Users "
                                         "LEFT JOIN UserGroups ON UserGroups.userId = Users.id "
                                         "LEFT JOIN Groups ON Groups.id = UserGroups.groupId "
                                         "GROUP BY Users.id"));

            if (!query.exec()) {
                return false;
            }

            QMap<int, int> valuesMap;

            // Hack for obtain size of select when we use sqlite
            int length = 0;
            if(query.last())
            {
                length =  query.at() + 1;
                query.first();
                query.previous();
            }

            int i = 0;

            // Place all data from select to map where key is a id of profile value - score
            while (query.next()) {
                i++;

                valuesMap[query.value(QStringLiteral("id")).toInt()] = query.value(QStringLiteral("score")).toInt();

                // Each 1000 users update rescoring counter
                if (i % 1000 == 0)
                    update_rescoringPercent(static_cast<double>(i) / length * mapFillingWeight);
            }
            update_rescoringPercent(mapFillingWeight);

            QMap<int, int> interestScoreMap;
            if (interests.length() != 0) {

                // Take interest by each user.
                query.prepare(QStringLiteral("SELECT userId, interestId FROM UserInterest "
                                             "LEFT JOIN Users ON UserInterest.userId = Users.id "
                                             "WHERE Users.scored=1;"));

                if (!query.exec()) {
                    return false;
                }

                // Hack for obtain size of select when we use sqlite
                if(query.last())
                {
                    length =  query.at() + 1;
                    query.first();
                    query.previous();
                }

                i = 0;

                // If interest in current list of interest - add 1 point to user, if it is not - do nothing
                while (query.next()) {
                    i++;

                    if (!interests.contains(query.value(QStringLiteral("interestId")).toInt())) {
                        continue;
                    }

                    if (interestScoreMap.contains(query.value(QStringLiteral("userId")).toInt()))
                        interestScoreMap[query.value(QStringLiteral("userId")).toInt()] += 1;
                    else
                        interestScoreMap[query.value(QStringLiteral("userId")).toInt()] = 1;

                    if (i % 1000 == 0)
                        update_rescoringPercent(mapFillingWeight + static_cast<double>(i) / length * interestsMapFillingWeight);
                }
            }
            update_rescoringPercent(mapFillingWeight + interestsMapFillingWeight);

            i = 0;
            length =  valuesMap.keys().length();

            dbConnection.transaction();
            // Update all affected profiles
            for (int id : valuesMap.keys()) {
                i++;
                int score = valuesMap[id];
                int interestScore = interestScoreMap.value(id, 0);

                query.prepare(QStringLiteral("UPDATE Users SET groupScore=:groupScore, interestScore=:interestScore WHERE id=:id"));
                query.bindValue(QStringLiteral(":id"), id);
                query.bindValue(QStringLiteral(":groupScore"), score);
                query.bindValue(QStringLiteral(":interestScore"), interestScore);

                if (!query.exec()) {
                    qDebug() << query.lastError();
                    dbConnection.rollback();
                    return false;
                }

                if (i % 1000 == 0)
                    update_rescoringPercent(mapFillingWeight + interestsMapFillingWeight + static_cast<double>(i) / length * updateRequestsMapFilingWeight);
            }
            update_rescoringPercent(mapFillingWeight + interestsMapFillingWeight + updateRequestsMapFilingWeight);

            if (!dbConnection.commit()) {
                qDebug() << dbConnection.lastError();
                dbConnection.rollback();
                return false;
            }
            update_rescoringPercent(1.0);
        }

        QSqlDatabase::removeDatabase(connectionName);

        return true;
    }));

    CFGroupManager::instance().set_interests(interests);
    CFGroupManager::instance().set_interestsDirty(false);
}

void CFUserManager::citySearch(QString inQuery, int countryId)
{
    QUrl url(QStringLiteral("/method/database.getCities"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("country_id"), QString::number(countryId));
    query.addQueryItem(QStringLiteral("q"),  inQuery);
    query.addQueryItem(QStringLiteral("count"),  QStringLiteral("50"));
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("lang"), QStringLiteral("ru"));
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req,  &CFRequest::success,  this,  &CFUserManager::citySearchSuccess);
    connect(req,  &CFRequest::error,  this,  &CFUserManager::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFUserManager::specifyAge(CFUser *user)
{
    if (m_ageSpecifier) {
        qWarning() << "CFUserManager::specifyAge called "
                      "when current check is not yet finished";
        return;
    }

    update_currentAgeSpecifyVkId(user->get_vkId());
    update_ageSpecifier(new CFSpecifyAgeHandler(user));
    connect(m_ageSpecifier, &CFSpecifyAgeHandler::finished, [this]{
        m_ageSpecifier->deleteLater();
        update_ageSpecifier(nullptr);
        update_currentAgeSpecifyVkId(INVALID_VALUE);
    });
    connect(m_ageSpecifier, &CFSpecifyAgeHandler::error, [this](CFOpError *error){
         error->update_source(tr("Уточнение возраста"));
         emit opError(error);
    });
}

void CFUserManager::errorWithServerOrConnection(int error,
                                                const QString &errorMessage)
{
    qDebug() << "[CFUserManager::errorWithServerOrConnection] Error: "
                + QString::number(error) + "; message: " + errorMessage;
}

void CFUserManager::init()
{
    m_countryModel->select();
    m_model->initFilters();
    select();

    m_model->updateUsersDownloaded();
    initFriendsAndMyself();

    emit initialized();
}

void CFUserManager::playNotification()
{
    m_notificationPlayer.play();
}

QString CFUserManager::topAnalyzeWhereStatement()
{
    QString whereClause = m_model->whereClause();
    if (whereClause.isEmpty()) {
        whereClause += "WHERE ";
    } else {
        whereClause += " AND ";
    }

    whereClause += "groupScore >= :score ";

    whereClause += "LIMIT " + QString::number(m_model->get_limit());

    return whereClause;
}

void CFUserManager::startScoring()
{
    if (m_score || m_ageSearch) {
        qWarning() << "CFUserManager::startScoring called "
                      "when current scoring is not yet finished";
        return;
    }

    initScore();
    if (m_model->get_searchAge()) {
        update_ageSearch(new CFAgeSearchHandler());
        connect(m_ageSearch, &CFAgeSearchHandler::finished, [this](bool success){
            if (!success) {
                CFGlobal::vkApiRequestSender().clearQueue();
            }

            m_ageSearch->deleteLater();
            update_ageSearch(nullptr);
            m_score->start();
        });
        connect(m_ageSearch, &CFAgeSearchHandler::error, [this](CFOpError *error){
           error->update_source(tr("Поиск возраста"));
           emit opError(error);
        });
    } else {
        m_score->start();
    }
}

void CFUserManager::cancelScoring()
{
    if (m_ageSearch) {
        CFGlobal::vkApiRequestSender().clearQueue();
        m_ageSearch->deleteLater();
        update_ageSearch(nullptr);
    }

    removeScore();
}

void CFUserManager::select()
{
    m_model->select();
    m_favoriteModel->select();
}

void CFUserManager::citySearchSuccess(QSharedPointer<QIODevice> replyData, const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    replyData->open(QIODevice::ReadOnly);
    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();
    QJsonObject response = replyAsObject[QLatin1String("response")].toObject();

    if (response["count"].toInt() == 0) {
        emit noCitiesFound();
        return;
    }

    emit citySearchFinished(QJsonDocument(response[QLatin1String("items")].toArray()).toJson());
}

void CFUserManager::initScore()
{
    update_score(new CFScoreHandler(m_model, INVALID_VALUE, this));
    connect(m_score, &CFScoreHandler::finished, [=]{
        emit scoringSuccessful();
        m_notificationPlayer.play();
        removeScore();
    });
    connect(m_score, &CFScoreHandler::error, [this](CFOpError *error){
        error->update_source(tr("Подсчет оценки"));
        emit opError(error);
    });
}

void CFUserManager::removeScore()
{
    m_score->deleteLater();
    CFGlobal::vkApiRequestSender().clearQueue();
    update_score(nullptr);
    select();
}

QSet<int> CFUserManager::friendsAndMyself() const
{
    return m_friendsAndMyself;
}

QPair<int, int> CFUserManager::topAnalyzeTimeScore(int timeInSec, bool wait)
{
    QFutureWatcher<QPair<int, int>> watcher;

    watcher.setFuture(QtConcurrent::run([timeInSec, this, wait]{

        // Just get count of users with score more or equal to score param
        QString connectionName = "topAnalyzeTimeScore";

        int thresholdInSecDown = timeInSec * 9 / 10; // -10%
        int thresholdInSecUp = timeInSec * 11 / 10;  // +10%

        QString whereStatement = topAnalyzeWhereStatement();

        int resultTime = 0;
        int resultScore = 0;

        // It is recommended by Qt to scope dbConnection so it would be destroyed automatically
        {
            QSqlDatabase dbConnection = QSqlDatabase::addDatabase(CFDatabase::DB_TYPE,
                                                     connectionName);

            dbConnection.setDatabaseName(CFDatabase::fullPath(CFVkAuthManager::instance().get_userVkId()));

            if (!dbConnection.open()) {
                qCritical() << "[CFUserManager::topAnalyzeTime] Could not open database";
                return QPair<int, int>(-1, -1);
            }
            QSqlQuery query(dbConnection);

            int down = 5;
            int up = 100;
            int middle = (up + down) / 2;


            while (true) {
                query.prepare(QString("SELECT COUNT() as count FROM (SELECT DISTINCT Users.id FROM Users %1);").arg(whereStatement));

                query.bindValue(":score", middle);

                if (!query.exec()) {
                    qWarning() << "[CFUserManager::topAnalyzeTimeScore] Can't exec query" << query.lastError();

                    return QPair<int, int>(-1, -1);
                }
                query.next();
                // Because usualy 3 rps (we can analyze maximum 3 user in second).
                resultTime = query.value("count").toInt() / 3;

                if (resultTime == timeInSec || (resultTime >= thresholdInSecDown && resultTime <= thresholdInSecUp )) {
                    resultScore = middle;
                    break;
                }

                if (resultTime > timeInSec)
                    down = middle + 1;
                else
                    up = middle - 1;


                if (down > up) {
                    resultScore = middle;
                    break;
                }

                middle = (up + down) / 2;
            }
        }
        QSqlDatabase::removeDatabase(connectionName);

        if (!wait)
            emit this->topAnalyzeTimeScoreFinished(resultTime, resultScore);

        return QPair<int, int>(resultTime, resultScore);
    }));

    if (wait) {
        watcher.waitForFinished();
        return watcher.future().result();
    }
    return QPair<int, int>();
}

void CFUserManager::topAnalyzeTimeScoreQml(int time)
{
    topAnalyzeTimeScore(time, false);
}

void CFUserManager::topAnalyzeTime(int score)
{
    m_topAnalyzeTimeWatcher.setFuture(QtConcurrent::run([this, score]{
        // Just get count of users with score more or equal to score param
        QString connectionName = "topAnalyzeTime";

        int time = 0;

        // It is recommended by Qt to scope dbConnection so it would be destroyed automatically
        {
            QSqlDatabase dbConnection = QSqlDatabase::addDatabase(CFDatabase::DB_TYPE,
                                                     connectionName);

            dbConnection.setDatabaseName(CFDatabase::fullPath(CFVkAuthManager::instance().get_userVkId()));

            if (!dbConnection.open()) {
                qCritical() << "[CFUserManager::topAnalyzeTime] Could not open database";
                return -1;
            }


            QSqlQuery query(dbConnection);

            query.prepare(QString("SELECT COUNT() as count FROM (SELECT DISTINCT Users.id FROM Users %1);").arg(topAnalyzeWhereStatement()));

            query.bindValue(":score", score);

            if (!query.exec()) {
                qWarning() << "[CFUserManager::topAnalyzeTime] Can't exec query" << query.lastError();

                return -1;
            }

            query.next();

            // Because usualy 3 rps (we can analyze maximum 3 user in second).
            time = query.value("count").toInt() / 3;
        }
        QSqlDatabase::removeDatabase(connectionName);

        return time;
    }));
}

void CFUserManager::topAnalyze(int inScore, int allotteredTime)
{
    int score = inScore;

    // if auto mode, than we shoul choose score value that will last allotteredTime
    if (inScore == INVALID_VALUE) {
        QPair<int, int> result = topAnalyzeTimeScore(allotteredTime, true);

        score = result.second;
    }

    update_score(new CFScoreHandler(m_model, score, this));
    connect(m_score, &CFScoreHandler::finished, [=]{
        if (inScore == INVALID_INDEX) {
            emit CFGroupManager::instance().automaticFinished();
        } else {
            emit scoringSuccessful();
        }
        m_notificationPlayer.play();
        removeScore();
    });
    connect(m_score, &CFScoreHandler::error, [this](CFOpError *error){
       error->update_source(tr("Подсчет оценки"));
       emit opError(error);
    });

    m_score->start();
}

void CFUserManager::initFriendsAndMyself()
{
    QString appUserId = CFVkAuthManager::instance().get_userVkId();

    if (appUserId.isEmpty()) {
        return;
    }

    m_friendsAndMyself.clear();

    m_friendsAndMyself.insert(CFVkAuthManager::instance().get_userVkId().toInt());

    makeGetFriendsRequest();

    makeMeRequest();
}

void CFUserManager::makeGetFriendsRequest()
{
    QUrl url(QStringLiteral("/method/friends.get"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("user_id"), CFVkAuthManager::instance().get_userVkId());
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req,  &CFRequest::success,  this,  &CFUserManager::parseFriends);
    connect(req,  &CFRequest::error,  this,  &CFUserManager::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFUserManager::parseFriends(QSharedPointer<QIODevice> replyData,
                                 const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    if (replyAsObject.contains(QLatin1String("error"))) {
        qWarning() << "[CFUserManager::parseFriends] VK API error: "
                   << replyAsObject.value(QLatin1String("error")).toObject()
                      .value(QLatin1String("error_msg")).toString();
        return;
    }

    QJsonArray idArray = replyAsObject.value(QLatin1String("response"))
            .toObject().value(QLatin1String("items")).toArray();

    for (const QJsonValue &val : idArray) {
        m_friendsAndMyself.insert(val.toInt());
    }
}

void CFUserManager::makeMeRequest()
{
    QUrl url(QStringLiteral("/method/account.getProfileInfo"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req,  &CFRequest::success,  this,  &CFUserManager::parseMe);
    connect(req,  &CFRequest::error,  this, &CFUserManager::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFUserManager::parseMe(QSharedPointer<QIODevice> replyData, const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();

    if (replyAsObject.contains(QLatin1String("error"))) {
        qWarning() << "[CFUserManager::parseMe] VK API error: "
                   << replyAsObject.value(QLatin1String("error")).toObject()
                      .value(QLatin1String("error_msg")).toString();
        return;
    }

    QJsonObject jsUser = replyAsObject.value(QLatin1String("response")).toObject();

    CFUser *curUser = new CFUser(this);

    QJsonValue sex = jsUser.value(QLatin1String("sex"));

    curUser->set_sex(sex.isUndefined() ? -1 : sex.toInt());

    QJsonValue jsBdate = jsUser.value(QLatin1String("bdate"));
    qint64 bdate = 0;
    if (!jsBdate.isUndefined()) {
        QString dateAsStr = jsBdate.toString();
        QDate date = dateAsStr.length() <= 5 ? QDate::fromString(dateAsStr, "d.M")
                                             : QDate::fromString(dateAsStr, "d.M.yyyy");
        if (date.year() != 1900) { // This means that year is not hidden
            bdate = date.toJulianDay();
        }
    }

    curUser->set_bdate(bdate);

    curUser->set_firstName(jsUser.value(QLatin1String("first_name")).toString());
    curUser->set_lastName(jsUser.value(QLatin1String("last_name")).toString());

    update_currentUser(curUser);
}

