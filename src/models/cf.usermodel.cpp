#include "cf.usermodel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>

#include "cf.utils.h"

CFUserModel::CFUserModel(QObject *parent,
                         const QByteArray &displayRole,
                         const QByteArray &uidRole)
    : QQmlObjectListModel(parent, displayRole, uidRole),
      m_country(INVALID_VALUE),
      m_sex(0),
      m_fromAge(0),
      m_toAge(0),
      m_photo(false),
      m_limit(0),
      m_searchAge(false),
      m_exact(false),
      m_lastViewedIndex(INVALID_INDEX),
      m_showViewed(false),
      m_usersDownloaded(0),
      m_viewedCount(0),
      m_lastUpdatedViewedIndex(0)
{
    connect(this, &CFUserModel::countryChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::country(), m_country);
    });
    connect(this, &CFUserModel::cityChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::city(), QJsonDocument(m_city).toJson());
    });
    connect(this, &CFUserModel::sexChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::sex(), m_sex);
    });
    connect(this, &CFUserModel::fromAgeChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::fromAge(), m_fromAge);
    });
    connect(this, &CFUserModel::toAgeChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::toAge(), m_toAge);
    });
    connect(this, &CFUserModel::photoChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::photo(), m_photo);
    });
    connect(this, &CFUserModel::politicalChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::political(), CFUtils::serialize(m_political));
    });
    connect(this, &CFUserModel::religionChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::religion(), CFUtils::serialize(m_religion));
    });
    connect(this, &CFUserModel::lifeMainChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::lifeMain(), CFUtils::serialize(m_lifeMain));
    });
    connect(this, &CFUserModel::peopleMainChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::peopleMain(), CFUtils::serialize(m_peopleMain));
    });
    connect(this, &CFUserModel::smokingChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::smoking(), CFUtils::serialize(m_smoking));
    });
    connect(this, &CFUserModel::alcoholChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::alcohol(), CFUtils::serialize(m_alcohol));
    });
    connect(this, &CFUserModel::relationChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::relation(), CFUtils::serialize(m_relation));
    });
    connect(this, &CFUserModel::groupsChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::groups(), CFUtils::serialize(m_groups));
    });
    connect(this, &CFUserModel::limitChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::limit(), m_limit);
    });
    connect(this, &CFUserModel::searchAgeChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::searchAge(), m_searchAge);
    });
    connect(this, &CFUserModel::showViewedChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::showViewed(), m_showViewed);
    });
    connect(this, &CFUserModel::exactChanged, [=]() {
        QSettings().setValue(CFSetting::Filter::exact(), m_exact);
    });

    m_viewedTimer.setSingleShot(true);
    m_viewedTimer.setInterval(2000);
    connect(&m_viewedTimer, &QTimer::timeout, [this]{
        updateViewedProfiles();
    });
    connect(this, &CFUserModel::lastViewedIndexChanged, [this]{
        m_viewedTimer.start();
    });
    connect(this, &CFUserModel::showViewedChanged, this, &CFUserModel::select);
}

void CFUserModel::updateUsersDownloaded()
{
    QSqlQuery query;
    query.prepare("SELECT (SELECT COUNT() FROM Users) AS count");
    if (!query.exec()) {
        qWarning() << "[CFUserModel::updateUsersDownloaded] "
                   << query.lastError();
        return;
    }

    if (query.next()) {
        update_usersDownloaded(query.value(QStringLiteral("count")).toInt());
    }
}

int CFUserModel::viewedPercent() const
{
    return m_queryResultCount ? (m_lastViewedIndex + 1) * 100 / m_queryResultCount : 0;
}

QString CFUserModel::selectStatement(bool order, int limit, int offset) const
{
    int appMode = CFUtils::instance().get_appMode();
    // INVALID_VALUE means respective clause should be skipped
    Q_ASSERT (limit != INVALID_VALUE ? true : (offset == INVALID_VALUE));

    QString selectStr = "SELECT DISTINCT id,screenName,firstName,lastName,cityId,countryId,sex,"
                        "bdate,photoMax,relation,political,religion,peopleMain,lifeMain,smoking,"
                        "alcohol,groupScore,interestScore,scored,favorite,inaccurateAge,"
                        "Users.groupId,openMessages,viewed,groupScore+interestScore AS fullScore FROM Users ";
    QString whereStr = whereClause(true);
    if (appMode == AppMode::Accurate) {
        if (whereStr.isEmpty()) {
            whereStr += "WHERE";
        } else {
            whereStr += " AND";
        }
        whereStr += " scored=1";
    }
    selectStr += whereStr;

    if (order) {
        selectStr += QStringLiteral(" ORDER BY %1 DESC")
                .arg("fullScore");
    }

    if (limit != INVALID_VALUE) {
        selectStr += " LIMIT " + QString::number(limit);
    }

    if (offset != INVALID_VALUE) {
        selectStr += " OFFSET " + QString::number(offset);
    }

    return selectStr;
}

QString CFUserModel::countStatement() const
{
    QString countStr = "SELECT COUNT() AS count FROM (SELECT DISTINCT id FROM Users %1)";
    QString whereStr = whereClause(true);
    if (CFUtils::instance().get_appMode() == AppMode::Accurate) {
        if (whereStr.isEmpty()) {
            whereStr += "WHERE";
        } else {
            whereStr += " AND";
        }
        whereStr += " scored=1";
    }

    return countStr.arg(whereStr);
}

QString CFUserModel::countViewedStatement() const
{
    QString countStr = "SELECT COUNT() AS count from (SELECT DISTINCT id FROM Users %1)";
    QString whereStr = whereClause(false, true);
    if (CFUtils::instance().get_appMode() == AppMode::Accurate) {
        if (whereStr.isEmpty()) {
            whereStr += "WHERE";
        } else {
            whereStr += " AND";
        }
        whereStr += " scored=1";
    }

    return countStr.arg(whereStr);
}

bool CFUserModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return rowCount() < m_queryResultCount;
}

void CFUserModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent)

    int offset = m_offset;
    if (!m_showViewed) {
        // Because viewed list is not static so offset does not applies in the same way as in static list
        offset -= m_lastUpdatedViewedIndex;
    }

    QSqlQuery query;
    query.prepare(selectStatement(true, m_step, offset));
    if (!query.exec()) {
        return;
    }
    m_offset += m_step;

    // Append via list because if users will be appended directly to model
    // it will cause rowInserted signal to be emitted each time
    // (while appending with list will it emit only once)
    QList<CFUser *> users;
    while (query.next()) {
        users.append(new CFUser(query, this));
    }

    append(users);
}

bool CFUserModel::select()
{
    emit askResetPosition();

    int limit = m_step * 4;
    m_offset = 0;

    QSqlQuery query;
    query.prepare(countStatement());
    if (!query.exec()) {
        return false;
    }
    if (query.next()) {
        m_queryResultCount = query.value(QStringLiteral("count")).toInt();
    }

    query.prepare(countViewedStatement());
    if (!query.exec()) {
        return false;
    }
    if (query.next()) {
        update_viewedCount(query.value(QStringLiteral("count")).toInt());
    }

    query.prepare(selectStatement(true, limit, m_offset));
    if (!query.exec()) {
        return false;
    }
    m_offset += limit;

    clear();
    set_lastViewedIndex(INVALID_INDEX);
    m_lastUpdatedViewedIndex = 0;

    QList<CFUser *> users;
    while (query.next()) {
        users.append(new CFUser(query, this));
    }

    append(users);

    emit selectFinished();

    return true;
}

void CFUserModel::initFilters()
{
    set_country(QSettings().value(CFSetting::Filter::country()).toInt());
    set_city(QJsonDocument::fromJson(QSettings().value(CFSetting::Filter::city()).toString().toUtf8()).array());
    set_sex(QSettings().value(CFSetting::Filter::sex()).toInt());
    set_fromAge(QSettings().value(CFSetting::Filter::fromAge()).toInt());
    set_toAge(QSettings().value(CFSetting::Filter::toAge()).toInt());
    set_photo(QSettings().value(CFSetting::Filter::photo()).toBool());
    set_political(CFUtils::deserialize(QSettings().value(CFSetting::Filter::political()).toString()));
    set_religion(CFUtils::deserialize(QSettings().value(CFSetting::Filter::religion()).toString()));
    set_lifeMain(CFUtils::deserialize(QSettings().value(CFSetting::Filter::lifeMain()).toString()));
    set_peopleMain(CFUtils::deserialize(QSettings().value(CFSetting::Filter::peopleMain()).toString()));
    set_smoking(CFUtils::deserialize(QSettings().value(CFSetting::Filter::smoking()).toString()));
    set_alcohol(CFUtils::deserialize(QSettings().value(CFSetting::Filter::alcohol()).toString()));
    set_relation(CFUtils::deserialize(QSettings().value(CFSetting::Filter::relation()).toString()));
    set_groups(CFUtils::deserialize(QSettings().value(CFSetting::Filter::groups()).toString()));
    set_limit(QSettings().value(CFSetting::Filter::limit(), 50000).toInt());
    set_searchAge(QSettings().value(CFSetting::Filter::searchAge(), false).toBool());
    set_showViewed(QSettings().value(CFSetting::Filter::showViewed(), false).toBool());
    set_exact(QSettings().value(CFSetting::Filter::exact(), false).toBool());
}

void CFUserModel::refreshFavorite(int userId, bool value)
{
    QModelIndexList searchResult = match(index(0, 0), roleForName("vkId"), userId);

    if (!searchResult.isEmpty()) {
        at(searchResult.first().row())->set_favorite(value);
    }
}

int CFUserModel::checkPersonal(const QList<int> &personal, int value)
{
    if (personal.isEmpty()) {
        return 0;
    }

    if (personal.contains(value)) {
        return 1;
    }

    return -1;
}

int CFUserModel::usersCountByFilterSettings(int countryId, int sex, QJsonArray cityList, bool photo,
                                            int fromAge, int toAge, QVariantList relationList,
                                            QVariantList groupList, bool exact,
                                            QVariantList politicalList, QVariantList religionList,
                                            QVariantList lifeMainList, QVariantList peopleMainList,
                                            QVariantList smokingList, QVariantList alcoholList)
{
    // save old data
    int saveCountry = m_country;
    int saveSex = m_sex;
    QJsonArray saveCity = m_city;
    QList<int> saveRelation = m_relation;
    bool savePhoto = m_photo;
    int saveFromAge = m_fromAge;
    int saveToAge = m_toAge;
    QList<int> saveGroup = m_groups;
    bool saveExact = m_exact;
    QList<int> savePolitical = m_political;
    QList<int> saveReligion = m_religion;
    QList<int> saveLifeMain = m_lifeMain;
    QList<int> savePeopleMain = m_peopleMain;
    QList<int> saveSmoking = m_smoking;
    QList<int> saveAlcohol = m_alcohol;

    // set temp data to make request without notify
    m_country = countryId;
    m_sex = sex;
    m_photo = photo;
    m_fromAge = fromAge;
    m_toAge = toAge;
    m_city = cityList;
    m_relation.clear();
    for (QVariant i : relationList) {
        m_relation.append(i.toInt());
    }
    m_groups.clear();
    for (QVariant i : groupList) {
        m_groups.append(i.toInt());
    }
    m_exact = exact;
    m_political.clear();
    for (QVariant i : politicalList) {
        m_political.append(i.toInt());
    }
    m_religion.clear();
    for (QVariant i : religionList) {
        m_religion.append(i.toInt());
    }
    m_lifeMain.clear();
    for (QVariant i : lifeMainList) {
        m_lifeMain.append(i.toInt());
    }
    m_peopleMain.clear();
    for (QVariant i : peopleMainList) {
        m_peopleMain.append(i.toInt());
    }
    m_smoking.clear();
    for (QVariant i : smokingList) {
        m_smoking.append(i.toInt());
    }
    m_alcohol.clear();
    for (QVariant i : alcoholList) {
        m_alcohol.append(i.toInt());
    }

    // get statement
    QString countStatement = "SELECT COUNT() AS count FROM (SELECT DISTINCT id FROM Users %1)";

    countStatement = countStatement.arg(whereClause(false));

    // recover old data
    m_country = saveCountry;
    m_sex = saveSex;
    m_city = saveCity;
    m_relation = saveRelation;
    m_photo = savePhoto;
    m_fromAge = saveFromAge;
    m_toAge = saveToAge;
    m_groups = saveGroup;
    m_exact = saveExact;
    m_political = savePolitical;
    m_religion = saveReligion;
    m_lifeMain = saveLifeMain;
    m_peopleMain = savePeopleMain;
    m_smoking = saveSmoking;
    m_alcohol = saveAlcohol;

    // Exec sql query and return result
    QSqlQuery query(countStatement);

    if (!query.exec()) {
        qWarning() << "[CFUserModel::usersCountByFilterSettings] Sql query failed:" << query.lastError();
        return  0;
    }

    if (!query.next()) {
        qWarning() << "[CFUserModel::usersCountByFilterSettings] no count found";
        return 0;
    }

    return query.value(QStringLiteral("count")).toInt();
}

int CFUserModel::usersCountByFilterSettingsScored(int countryId, int sex, QJsonArray cityList,
                                                  bool photo, int fromAge, int toAge,
                                                  QVariantList relationList, QVariantList groupList,
                                                  bool exact, QVariantList politicalList,
                                                  QVariantList religionList,
                                                  QVariantList lifeMainList,
                                                  QVariantList peopleMainList,
                                                  QVariantList smokingList,
                                                  QVariantList alcoholList)
{
    // save old data
    int saveCountry = m_country;
    int saveSex = m_sex;
    QJsonArray saveCity = m_city;
    QList<int> saveRelation = m_relation;
    bool savePhoto = m_photo;
    int saveFromAge = m_fromAge;
    int saveToAge = m_toAge;
    QList<int> saveGroup = m_groups;
    bool saveExact = m_exact;
    QList<int> savePolitical = m_political;
    QList<int> saveReligion = m_religion;
    QList<int> saveLifeMain = m_lifeMain;
    QList<int> savePeopleMain = m_peopleMain;
    QList<int> saveSmoking = m_smoking;
    QList<int> saveAlcohol = m_alcohol;

    // set temp data to make request without notify
    m_country = countryId;
    m_sex = sex;
    m_photo = photo;
    m_fromAge = fromAge;
    m_toAge = toAge;
    m_city = cityList;
    m_relation.clear();
    for (QVariant i : relationList) {
        m_relation.append(i.toInt());
    }
    m_groups.clear();
    for (QVariant i : groupList) {
        m_groups.append(i.toInt());
    }
    m_exact = exact;
    m_political.clear();
    for (QVariant i : politicalList) {
        m_political.append(i.toInt());
    }
    m_religion.clear();
    for (QVariant i : religionList) {
        m_religion.append(i.toInt());
    }
    m_lifeMain.clear();
    for (QVariant i : lifeMainList) {
        m_lifeMain.append(i.toInt());
    }
    m_peopleMain.clear();
    for (QVariant i : peopleMainList) {
        m_peopleMain.append(i.toInt());
    }
    m_smoking.clear();
    for (QVariant i : smokingList) {
        m_smoking.append(i.toInt());
    }
    m_alcohol.clear();
    for (QVariant i : alcoholList) {
        m_alcohol.append(i.toInt());
    }

    // get statement
    QString countStatement = "SELECT COUNT() AS count FROM (SELECT DISTINCT id FROM Users %1)";

    QString whereStr = whereClause(false);
    if (whereStr.isEmpty()) {
        whereStr += "WHERE";
    } else {
        whereStr += " AND";
    }
    whereStr += " scored=1";
    countStatement = countStatement.arg(whereStr);

    // recover old data
    m_country = saveCountry;
    m_sex = saveSex;
    m_city = saveCity;
    m_relation = saveRelation;
    m_photo = savePhoto;
    m_fromAge = saveFromAge;
    m_toAge = saveToAge;
    m_groups = saveGroup;
    m_exact = saveExact;
    m_political = savePolitical;
    m_religion = saveReligion;
    m_lifeMain = saveLifeMain;
    m_peopleMain = savePeopleMain;
    m_smoking = saveSmoking;
    m_alcohol = saveAlcohol;

    // Exec sql query and return result
    QSqlQuery query(countStatement);

    if (!query.exec()) {
        qWarning() << "[CFUserModel::usersCountByFilterSettingsScored] Sql query failed:" << query.lastError();
        return  0;
    }

    if (!query.next()) {
        qWarning() << "[CFUserModel::usersCountByFilterSettingsScored] no count found";
        return 0;
    }

    return query.value(QStringLiteral("count")).toInt();
}

void CFUserModel::updateViewedProfiles()
{
    QString idString;

    for (; m_lastUpdatedViewedIndex <= m_lastViewedIndex; m_lastUpdatedViewedIndex++) {
        idString += QString::number(at(m_lastUpdatedViewedIndex)->get_vkId()) + ",";
    }
    m_viewedIndexList.clear();
    idString.chop(1);

    QSqlQuery query(QStringLiteral("UPDATE Users SET viewed=1 WHERE id IN (%1)").arg(idString));
    if (!query.exec())
        qWarning() << "[CFUserManager::updateViewedProfiles] Sql query failed:" << query.lastError();
}

QString CFUserModel::whereClause(bool withViewed, bool viewedOnly, bool withAge, bool userGroupsFlag) const
{
    QString whereStr;

    // Group check uses JOIN that must be appended before WHERE keword
    if (!m_groups.isEmpty() && userGroupsFlag) {
        whereStr = "INNER JOIN UserGroups ON UserGroups.userId = Users.id ";
    }

    whereStr += "WHERE";

    if (viewedOnly) {
        whereStr += " viewed=1 AND";
    } else if (withViewed && !m_showViewed) {
        whereStr += " viewed=0 AND";
    }

    if (m_country != INVALID_VALUE && m_country) {
        whereStr += " countryId=" + QString::number(m_country) + " AND";
    }

    if (!m_city.isEmpty()) {
        QString cityStr;
        for (QJsonValue city : m_city) {
            int cityId = city["id"].toInt();

            cityStr += QString::number(cityId) + ',';
        }
        cityStr.chop(1);

        whereStr += " cityId IN (" + cityStr + ") AND";
    }

    if (m_sex != Personal::Sex::Both && m_sex != Personal::Sex::Undefined) {
        whereStr += " sex=" + QString::number(m_sex) + " AND";
    }

    if (withAge && m_fromAge != 0) {
        int bdateFrom = QDate::currentDate().addYears(-m_fromAge).toJulianDay();
        int bdateTo = QDate::currentDate().addYears(-m_toAge - 1).toJulianDay();

        whereStr += " bdate>=" + QString::number(bdateTo)
                + " AND bdate<=" + QString::number(bdateFrom) + " AND";
    }

    if (m_photo) {
        whereStr += " photoMax <> 'https://vk.com/images/camera_200.png?ava=1' AND";
    }

    if (!m_relation.isEmpty()) {
        QString relationStr;
        for (int relationId : m_relation) {
            relationStr += QString::number(relationId) + ',';
        }
        relationStr.chop(1);

        whereStr += " ( relation IN (" + relationStr + ")";

        if (!m_exact) {
            whereStr += " OR relation IS NULL";
        }

        whereStr += " ) AND";
    }

    if (!m_groups.isEmpty()) {
        QString groupsStr;
        for (int groupId : m_groups) {
            groupsStr += QString::number(groupId) + ',';
        }
        groupsStr.chop(1);

        whereStr += " UserGroups.groupId IN (" + groupsStr + ") AND";
    }

    if (m_exact) {
        if (!m_political.isEmpty()) {
            QString valStr;
            for (int val : m_political) {
                valStr += QString::number(val) + ',';
            }
            valStr.chop(1);

            whereStr += " political IN (" + valStr + ") AND";
        }

        if (!m_religion.isEmpty()) {
            QString valStr;
            for (int val : m_religion) {
                valStr += QString::number(val) + ',';
            }
            valStr.chop(1);

            whereStr += " religion IN (" + valStr + ") AND";
        }

        if (!m_lifeMain.isEmpty()) {
            QString valStr;
            for (int val : m_lifeMain) {
                valStr += QString::number(val) + ',';
            }
            valStr.chop(1);

            whereStr += " lifeMain IN (" + valStr + ") AND";
        }

        if (!m_peopleMain.isEmpty()) {
            QString valStr;
            for (int val : m_peopleMain) {
                valStr += QString::number(val) + ',';
            }
            valStr.chop(1);

            whereStr += " peopleMain IN (" + valStr + ") AND";
        }

        if (!m_smoking.isEmpty()) {
            QString valStr;
            for (int val : m_smoking) {
                valStr += QString::number(val) + ',';
            }
            valStr.chop(1);

            whereStr += " smoking IN (" + valStr + ") AND";
        }

        if (!m_alcohol.isEmpty()) {
            QString valStr;
            for (int val : m_alcohol) {
                valStr += QString::number(val) + ',';
            }
            valStr.chop(1);

            whereStr += " alcohol IN (" + valStr + ") AND";
        }
    }

    if (whereStr == "WHERE") { // No filters are set
        whereStr = QString();
    } else {
        whereStr.chop(4); // Chop " AND"
    }

    return whereStr;
}
