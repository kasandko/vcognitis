#include "cf.user.h"

#include <QJsonObject>
#include <QSqlQuery>
#include <QVariant>
#include <QDate>
#include <QSqlError>

#include "cf.global.h"
#include "managers/cf.groupmanager.h"

CFUser::CFUser(QObject *parent)
    : QObject(parent),
      m_vkId(INVALID_VALUE),
      m_city(INVALID_VALUE),
      m_country(INVALID_VALUE),
      m_sex(INVALID_VALUE),
      m_bdate(INVALID_VALUE),
      m_relation(INVALID_VALUE),
      m_political(INVALID_VALUE),
      m_peopleMain(INVALID_VALUE),
      m_lifeMain(INVALID_VALUE),
      m_smoking(INVALID_VALUE),
      m_alcohol(INVALID_VALUE),
      m_groupScore(0),
      m_interestScore(0),
      m_scored(false),
      m_favorite(false),
      m_inaccurateAge(false),
      m_groupId(INVALID_VALUE),
      m_openMessages(false),
      m_viewed(false)
{
    connect(this, &CFUser::favoriteChanged, this, &CFUser::persistFavorite);

    initProxy();
}

CFUser::CFUser(const QSqlQuery &query, QObject *parent)
    : QObject(parent)
{
    m_screenName = query.value(QLatin1String("screenName")).toString();
    m_vkId = query.value(QLatin1String("id")).toInt();
    m_firstName = query.value(QLatin1String("firstName")).toString();
    m_lastName = query.value(QLatin1String("lastName")).toString();

    QVariant value = query.value(QLatin1String("cityId"));
    m_city = value.isNull() ? INVALID_VALUE : value.toInt();

    value = query.value(QLatin1String("countryId"));
    m_country = value.isNull() ? INVALID_INDEX : value.toInt();

    value = query.value(QLatin1String("sex"));
    m_sex = value.isNull() ? INVALID_VALUE : value.toInt();

    value =  query.value(QLatin1String("bdate"));
    m_bdate = value.isNull() ? INVALID_VALUE : value.toLongLong();

    m_photoMax = query.value(QLatin1String("photoMax")).toString();

    value = query.value(QLatin1String("relation"));
    m_relation = value.isNull() ? INVALID_VALUE : value.toInt();

    value = query.value(QLatin1String("political"));
    m_political = value.isNull() ? INVALID_VALUE : value.toInt();

    value = query.value(QLatin1String("religion"));
    m_religion = value.isNull() ? INVALID_VALUE : value.toInt();

    value = query.value(QLatin1String("peopleMain"));
    m_peopleMain = value.isNull() ? INVALID_VALUE : value.toInt();

    value = query.value(QLatin1String("lifeMain"));
    m_lifeMain = value.isNull() ? INVALID_VALUE : value.toInt();

    value = query.value(QLatin1String("smoking"));
    m_smoking = value.isNull() ? INVALID_VALUE : value.toInt();

    value = query.value(QLatin1String("alcohol"));
    m_alcohol = value.isNull() ? INVALID_VALUE : value.toInt();

    m_groupScore = query.value(QLatin1String("groupScore")).toInt();
    m_interestScore = query.value(QLatin1String("interestScore")).toInt();
    m_scored = query.value(QLatin1String("scored")).toBool();

    m_favorite = query.value(QLatin1String("favorite")).toBool();
    m_inaccurateAge = query.value(QLatin1String("inaccurateAge")).toBool();

    value = query.value(QLatin1String("groupId"));
    m_groupId = value.isNull() ? INVALID_VALUE : value.toInt();

    m_openMessages = query.value(QLatin1String("openMessages")).toBool();

    m_viewed = query.value(QLatin1String("viewed")).toBool();

    connect(this, &CFUser::favoriteChanged, this, &CFUser::persistFavorite);

    initProxy();
}

void CFUser::persistFavorite()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Users SET favorite=:favorite WHERE id=:id"));
    query.bindValue(QStringLiteral(":favorite"), m_favorite);
    query.bindValue(QStringLiteral(":id"), m_vkId);
    if (!query.exec()) {
        qWarning() << "[CFUser::persistFavorite] Update favorite failed";
    }
}

QList<int> CFUser::topInterests()
{
    QSqlQuery query;
    // Get all interests which are associated to current user id
    query.prepare(QStringLiteral("SELECT Interest.id, UserInterest.countGroups FROM UserInterest "
                                 "JOIN Interest ON UserInterest.interestId=Interest.id "
                                 "WHERE UserInterest.userId=:userId;"));
    query.bindValue(QStringLiteral(":userId"), m_vkId);
    query.exec();

    // We need only 5(or less) top interest of user
    // So we store all interest in map where key is id, value is count of groups of that interest
    QMap<int, int> interestMap;
    while(query.next()) {
        interestMap[query.value(0).toInt()] = query.value(1).toInt();
    }

    QList<int> sortedInterests = interestMap.keys();

    // sort interest by their count
    std::sort(sortedInterests.begin(), sortedInterests.end(), [&interestMap](int left, int right) {
        return interestMap[left] > interestMap[right];
    });

    return sortedInterests.mid(0, 5);
}

int CFUser::calculateAge() const
{
    if (m_bdate == INVALID_VALUE)
        return INVALID_VALUE;

    QDate bdate = QDate::fromJulianDay(m_bdate);
    QDate curDate = QDate::currentDate();
    int age = curDate.year() - bdate.year() - 1;
    if (curDate.month() > bdate.month()
            || (curDate.month() == bdate.month() && curDate.day() >= bdate.day())) {
        age++;
    }
    return age;
}

void CFUser::initProxy()
{
    CFGroupModel *groups = CFGroupManager::instance().get_groupModel();

    m_positiveGroups = new QSortFilterProxyModel(this);
    m_positiveGroups->setSourceModel(groups);

    m_negativeGroups = new QSortFilterProxyModel(this);
    m_negativeGroups->setSourceModel(groups);

    m_interests = new QSortFilterProxyModel(this);
    m_interests->setSourceModel(CFGroupManager::instance().get_interestModel());

    m_positiveGroups->setFilterRole(GroupRoles::Id);
    m_negativeGroups->setFilterRole(GroupRoles::Id);
    m_interests->setFilterRole(InterestRoles::Id);

    refreshFilterStrings();

    connect(groups, &CFGroupModel::modelReset, this, &CFUser::refreshFilterStrings);
    connect(&CFGroupManager::instance(), &CFGroupManager::interestsChanged, this, &CFUser::refreshFilterStrings);
}

void CFUser::refreshFilterStrings()
{
    QSqlQuery query;

    // Get all groups which are associated to current user id
    query.prepare(QStringLiteral("SELECT Groups.id,Groups.weight FROM UserGroups "
                                 "JOIN Groups ON UserGroups.groupId=Groups.id "
                                 "WHERE UserGroups.userId=:userId;"));
    query.bindValue(QStringLiteral(":userId"), m_vkId);
    query.exec();

    QString positiveGroupsString;
    QString negativeGroupsString;

    // Now magic part:
    // We should make regExp like: ^2$|...|^100$
    // Then reqExp will filter only ids associated group ids
    while(query.next()) {
        int id = query.value(0).toInt();
        int weight = query.value(1).toInt();

        if (weight > 0) {
            positiveGroupsString += "^" + QString::number(id) + "$|";
        } else {
            negativeGroupsString += "^" + QString::number(id) + "$|";
        }
    }

    QString interestRegExpString;

    for (int id : topInterests()) {
        interestRegExpString += "^" + QString::number(id) + "$|";
    }

    // If we have associated elements then remove last "|"
    // Else add bad id to hide all elements(we should show no group)
    if (!positiveGroupsString.isEmpty()) {
        positiveGroupsString.chop(1);
    } else {
        positiveGroupsString = "^-1$";
    }

    if (!negativeGroupsString.isEmpty()) {
        negativeGroupsString.chop(1);
    } else {
        negativeGroupsString = "^-1$";
    }

    if (!interestRegExpString.isEmpty()) {
        interestRegExpString.chop(1);
    } else {
        interestRegExpString = "^-1$";
    }
    // Set up filter regExp
    m_positiveGroups->setFilterRegExp(positiveGroupsString);
    m_negativeGroups->setFilterRegExp(negativeGroupsString);
    m_interests->setFilterRegExp(interestRegExpString);
}
