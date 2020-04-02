#include "cf.groupmanager.h"
#include "cf.utils.h"

#include <cmath>

#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QRegExp>

#include "network/cf.request.h"
#include "cf.vkauthmanager.h"
#include "cf.usermanager.h"

const int CFGroupManager::k_groupCountForAutomatic = 50;

CFGroupManager::CFGroupManager(QObject *parent)
    : QObject(parent),
      m_groupModel(new CFGroupModel(this)),
      m_positiveGroups(new QQmlObjectListProxyModel(this)),
      m_negativeGroups(new QQmlObjectListProxyModel(this)),
      m_downloadedGroupProxy(new CFDownloadedGroupProxy(m_groupModel, this)),
      m_interestModel(new CFInterestModel(this)),
      m_interestCategoryModel(new CFInterestCategoryModel(this)),
      m_interestSearchModel(new CFInterestFilterProxyModel(m_interestModel, this)),
      m_userGroupsCount(INVALID_VALUE),
      m_interestsDirty(false),
      m_loopedChunkDownloader(nullptr),
      m_maxTimeToDownloadAllMembers(8 * 60 * 60)
{
    initPositiveGroups();
    initNegativeGroups();

    connect(this, &CFGroupManager::interestsChanged, [=]() {
        QSettings().setValue(CFSetting::interests(), CFUtils::serialize(m_interests));
    });

    connect(&CFVkAuthManager::instance(), &CFVkAuthManager::authSuccess,
            this, &CFGroupManager::init);
}

void CFGroupManager::refreshUserGroups(int count)
{
    if (!count) {
        return;
    }

    QUrl url(QStringLiteral("/method/groups.get"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("user_id"), CFVkAuthManager::instance().get_userVkId());
    query.addQueryItem(QStringLiteral("extended"),  QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("fields"), QStringLiteral("activity,members_count"));
    query.addQueryItem(QStringLiteral("lang"), QStringLiteral("ru"));
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);

    req->setProperty("count", count);

    connect(req,  &CFRequest::success,  this, &CFGroupManager::refreshUserGroupsSuccess);
    connect(req,  &CFRequest::error,  this,  &CFGroupManager::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFGroupManager::getGroupsForAnalysys()
{
    QUrl url(QStringLiteral("/method/groups.get"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("user_id"), CFVkAuthManager::instance().get_userVkId());
    query.addQueryItem(QStringLiteral("extended"),  QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("fields"), QStringLiteral("members_count"));
    query.addQueryItem(QStringLiteral("lang"), QStringLiteral("ru"));
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req,  &CFRequest::success,  this, &CFGroupManager::getGroupsForAnalysysSuccess);
    connect(req,  &CFRequest::error,  this,  &CFGroupManager::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

void CFGroupManager::assertModelValidity()
{
    if (m_assert) {
        qWarning() << "CFGroupManager::assertModelValidity called "
                      "when current assertion is not yet finished";
        return;
    }

    if (m_groupModel->count() == 0) {
        emit modelValidityAssertionFinished(false);
        return;
    }

    update_assert(new CFGroupAssertionHandler());

    connect(m_assert, &CFGroupAssertionHandler::finished, [this](bool error){
        emit modelValidityAssertionFinished(error);
        m_assert->deleteLater();
        update_assert(nullptr);
    });
    connect(m_assert, &CFGroupAssertionHandler::errorAt, this, &CFGroupManager::errorAt);
    connect(m_assert, &CFGroupAssertionHandler::error, this, &CFGroupManager::error);
}

void CFGroupManager::refreshUserGroupsCount()
{
    QUrl url(QStringLiteral("/method/groups.get"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("user_id"), CFVkAuthManager::instance().get_userVkId());
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("extended"),  QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("fields"), QStringLiteral("members_count"));
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req,  &CFRequest::success,  this,  [this] (QSharedPointer<QIODevice> replyData) {

        if (!replyData->open(QIODevice::ReadOnly)) {
            qCritical() << "[CFGroupManager::refreshUserGroupsCount::successLambda] Could not open reply data";
            return;
        }

        QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();
        int count = 0;

        for (QJsonValue value : replyAsObject.value(QLatin1String("response"))[QLatin1String("items")].toArray()) {
            int memberCount = value.toObject()["members_count"].toInt();

            if (memberCount < k_maxSubInGroup && memberCount != 0) {
                count++;
            }
        }

        update_userGroupsCount(count);
    });
    connect(req,  &CFRequest::error,  this,  &CFGroupManager::errorWithServerOrConnection);

    CFGlobal::vkApiRequestSender().send(req);
}

CFInterestProxyModel *CFGroupManager::interestByCategoryModel(int index)
{
    return m_interestProxyModels[index];
}

int CFGroupManager::categoriesCount() const
{
    return m_interestProxyModels.count();
}

int CFGroupManager::allottedTimeForTopAnalysys()
{
    return m_allottedTimeInSecsForTopAnalyze;
}

void CFGroupManager::startAutomatic(int allottedTimeInSecs)
{
    m_automaticMode = true;
    m_allottedTimeInSecsForTopAnalyze = qMin(allottedTimeInSecs * 20 / 100, 10 * 60); // 20% from whole time but not more than 10min
    m_allottedTimeInSecs = allottedTimeInSecs - m_allottedTimeInSecsForTopAnalyze;
    refreshUserGroups(k_groupCountForAutomatic); // Automatic mode assumes it is first start up by user so db is empty
}

void CFGroupManager::init()
{
    m_groupModel->select();
    m_interestModel->select();
    m_interestCategoryModel->select();
    initInterestProxies();
    set_interests(CFUtils::deserialize(QSettings().value(CFSetting::interests()).toString()));

    refreshUserGroupsCount();

    emit initialized();
}

void CFGroupManager::initInterestProxies()
{
    for(int i = 0; i < m_interestCategoryModel->count(); i++) {
        CFInterestCategory *category = m_interestCategoryModel->at(i);

        CFInterestProxyModel *model= new CFInterestProxyModel(m_interestModel, this);

        model->set_parentId(category->get_uid());
        model->set_parentName(category->get_name());

        m_interestProxyModels.append(model);
    }
    emit categoriesCountChanged();
}

void CFGroupManager::initPositiveGroups()
{
    m_positiveGroups->setSourceModel(m_groupModel);
    m_positiveGroups->setFilterRole(GroupRoles::Weight);
    m_positiveGroups->setFilterRegExp(QRegExp(QStringLiteral("^([1-9]|10)$")));
    m_positiveGroups->setSortRole(GroupRoles::Priority);
    m_positiveGroups->sort(0, Qt::DescendingOrder);
}

void CFGroupManager::initNegativeGroups()
{
    m_negativeGroups->setSourceModel(m_groupModel);
    m_negativeGroups->setFilterRole(GroupRoles::Weight);
    m_negativeGroups->setFilterRegExp(QRegExp(QStringLiteral("^-([1-9]|10)$")));
    m_negativeGroups->setSortRole(GroupRoles::Priority);
    m_negativeGroups->sort(0, Qt::DescendingOrder);
}

void CFGroupManager::refreshUserGroupsSuccess(QSharedPointer<QIODevice> replyData,
                                      const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    const int maxGroupCount = qobject_cast<CFRequest*>(sender())->property("count").toInt();

    if (!replyData->open(QIODevice::ReadOnly)) {
        qCritical() << "[CFGroupManager::refreshUserGroupsSuccess] Could not open reply data";
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();
    QJsonArray responseArray = replyAsObject.value(QLatin1String("response"))
            .toObject().value(QLatin1String("items")).toArray();


    QMap<int, int> interestMap;

    QList<CFGroup*> groups;

    int count = 0; // Count of not skipped groups, needed for weight init
    for (int i = 0; i < responseArray.count(); i++) {
        QJsonObject jsGroup = responseArray.at(i).toObject();

        QString type = jsGroup.value(QLatin1String("type")).toString();
        if (!(type == "group" || type == "page")
            || jsGroup.contains(QLatin1String("deactivated"))) {
            continue;
        }

        int memberCount = jsGroup["members_count"].toInt();

        if (memberCount > k_maxSubInGroup || memberCount == 0) {
            continue;
        }

        if (m_groupModel->get(QString::number(jsGroup.value(QLatin1String("id")).toInt())) == nullptr) {
            CFGroup *item = new CFGroup();

            item->set_uid(jsGroup.value(QLatin1String("id")).toInt());
            item->set_name(jsGroup.value(QLatin1String("name")).toString());
            item->set_photo(jsGroup.value(QLatin1String("photo_200")).toString());
            item->set_screenName(jsGroup.value(QLatin1String("screen_name")).toString());
            item->set_priority(responseArray.count() - i);

            // Following weight computation will assign weight according to postion, closer to top will receive higher weight
            item->set_weight(10 - std::min((count / 5), 9));
            item->set_subCount(jsGroup.value(QLatin1String("members_count")).toInt());

            groups.append(item);
            m_groupModel->append(item);
            count++;
        }

        int interestId = m_interestModel->idByName(jsGroup.value(QLatin1String("activity")).toString());

        if (interestId != INVALID_VALUE) {
            interestMap[interestId] += 1;
        }

        if (count >= maxGroupCount)
            break;
    }

    if (count > 0) {
        m_groupModel->update_dirtyStatus(true);
    }

    QList<int> sortedInterests = interestMap.keys();

    // sort interest by their count in user subs
    std::sort(sortedInterests.begin(), sortedInterests.end(), [&interestMap](int left, int right) {
        return interestMap[left] > interestMap[right];
    });


    if (m_automaticMode) { // This means refreshUserGroups() was called by startAutomatic()
        // So we need to set up appropriate downloader (there will be multiple implementation in future) and start it

        set_interests(sortedInterests.mid(0, 10));
        for (CFGroup *group : groups) {
            // Persist groups to db
            group->set_validated(true);
            set_interestsDirty(false);
            m_groupModel->update_dirtyStatus(false);
            CFGroupAssertionHandler::addGroupToDatabase(group);
        }

        m_groupModel->select();

        update_loopedChunkDownloader(new CFLoopedChunkDownloader(m_groupModel->toList(), m_allottedTimeInSecs, this));
        connect(m_loopedChunkDownloader, &CFLoopedChunkDownloader::finished, [=]{
            m_loopedChunkDownloader->deleteLater();
            update_loopedChunkDownloader(nullptr);

            CFUserManager::instance().get_countryModel()->select();
            CFUserManager::instance().select();
            CFUserManager::instance().get_model()->updateUsersDownloaded();
            CFUserManager::instance().rescore(m_interests);

            QMetaObject::Connection * const connection = new QMetaObject::Connection;
            *connection = connect(&CFUserManager::instance(), &CFUserManager::rescoreFinished, [connection, this]() {
                // One time connection for auto mode
                CFUserManager::instance().topAnalyze(INVALID_VALUE, m_allottedTimeInSecsForTopAnalyze);
                QObject::disconnect(*connection);
                delete connection;
            });
        });
        connect(m_loopedChunkDownloader, &CFLoopedChunkDownloader::error, [this](CFOpError *error){
            error->update_source(tr("Автоматический режим"));
            emit opError(error);
        });

    } else { // Otherwise it was called manually so we need to display teaching message
        emit userInterestDownloadFinished(sortedInterests.mid(0, 10));
        QTimer::singleShot(100, this, [this]() {
            emit userGroupsRefreshed(); // If there is no any delay model can't update in time and we can't get teach message
        });
    }
}

void CFGroupManager::getGroupsForAnalysysSuccess(QSharedPointer<QIODevice> replyData,
                                          const QMap<QByteArray, QByteArray> &rawHeaders)
{
    Q_UNUSED(rawHeaders)

    if (!replyData->open(QIODevice::ReadOnly)) {
        qCritical() << "[CFGroupManager::refreshUserGroupsSuccess] Could not open reply data";
        return;
    }

    QJsonObject replyAsObject = QJsonDocument::fromJson(replyData->readAll()).object();
    QJsonArray responseArray = replyAsObject.value(QLatin1String("response"))
            .toObject().value(QLatin1String("items")).toArray();


    // Do some analysys for choosing correct automatic strategy here
    // At present though we only compute max possible time to download all members of all groups
    int totalCount = 0;
    int count = 0;
    for (int i = 0; i < responseArray.count(); i++) {
        QJsonObject jsGroup = responseArray.at(i).toObject();

        QString type = jsGroup.value(QLatin1String("type")).toString();
        if (!(type == "group" || type == "page")
            || jsGroup.contains(QLatin1String("deactivated"))) {
            continue;
        }

        int membersCount = jsGroup.value(QLatin1String("members_count")).toInt();

        if (membersCount < k_maxSubInGroup && membersCount != 0) {
            count ++;
            totalCount += jsGroup.value(QLatin1String("members_count")).toInt();
        }

        if (count == k_groupCountForAutomatic)
            break;
    }

    double requestsPerSecond = 1000. / CFRequestSender::k_intervalBetweenRequest;
    double membersPerSecond = CFDownloadHandler::k_offsetStep * requestsPerSecond;
    // 20% to aprox time
    update_maxTimeToDownloadAllMembers( std::round(totalCount / membersPerSecond) * 12 / 10);
}

void CFGroupManager::errorWithServerOrConnection(int error,
                                                 const QString &errorMessage)
{
    qDebug() << "[CFGroupManager::errorWithServerOrConnection] Error: "
             << QString::number(error) << "; message: " << errorMessage;
}


QString CFGroupManager::parsePhoto(const QJsonObject &jsGroup)
{
    QJsonValue jsPhoto = jsGroup.value(QLatin1String("photo_200"));
    QString photo;
    if (jsPhoto.isUndefined()) {
        photo = "https://vk.com/images/camera_200.png?ava=1";
    } else {
        photo = jsPhoto.toString();
    }
    return photo;
}
