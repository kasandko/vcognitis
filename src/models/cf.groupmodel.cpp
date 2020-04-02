#include "cf.groupmodel.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <QDebug>

#include "cf.global.h"
#include "managers/cf.usermanager.h"

CFGroupModel::CFGroupModel(QObject *parent, const QByteArray &displayRole, const QByteArray &uidRole)
    : QQmlObjectListModel(parent, displayRole, uidRole),
      m_dirtyStatus(false)
{
    connect(this, &CFGroupModel::rowsAboutToBeRemoved, [this]() {

        update_dirtyStatus(true);
    });
    connect(this, &CFGroupModel::rowsRemoved, [this]() {
        QList<int> newEmptyList;

        for (int i = 0 ; i < toList().count(); i++) {
            if(at(i)->get_screenName() == "")
                newEmptyList.append(i);
        }

        set_emptyIndexes(newEmptyList);
        set_haveEmpty(!m_emptyIndexes.empty());
    });
}

bool CFGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    update_dirtyStatus(true);

    if (role == GroupRoles::ScreenName) {
        if (m_emptyIndexes.contains(index.row()) && value.toString() != "") {
            m_emptyIndexes.removeAll(index.row());
            if (m_emptyIndexes.empty())
                set_haveEmpty(false);
        }

        if (value.toString().trimmed() == "") {
            set_haveEmpty(true);
            m_emptyIndexes.append(index.row());
        }

    }

    return QQmlObjectListModel::setData(index, value, role);
}

bool CFGroupModel::submitAll()
{
    // Nothing to submit
    if (!get_dirtyStatus())
        return true;

    QSqlDatabase::database().transaction();

    QSqlQuery query;

    // Clear table
    query.exec("DELETE FROM Groups;");

    QVariantList ids;
    QVariantList screenNames;
    QVariantList names;
    QVariantList photos;
    QVariantList priorities;
    QVariantList weights;
    QVariantList subs;
    QVariantList offsets;
    QVariantList subHiddens;

    for (CFGroup *item : toList()) {
        ids << item->get_uid();
        screenNames << item->get_screenName();
        names << item->get_name();
        photos << item->get_photo();
        priorities << item->get_priority();
        weights << item->get_weight();
        subs << item->get_subCount();
        offsets << item->get_offset();
        subHiddens << item->get_subHidden();
    }

    // Fill table from model
    query.prepare("INSERT INTO Groups (id, name, screenName, photo, priority, weight, subCount, subHidden, offset)"
                  "VALUES (:id, :name, :screenName, :photo, :priority, :weight, :subs, :subHidden, :offset);");
    query.bindValue(QStringLiteral(":id"), ids);
    query.bindValue(QStringLiteral(":name"),  names);
    query.bindValue(QStringLiteral(":screenName"), screenNames);
    query.bindValue(QStringLiteral(":photo"),  photos);
    query.bindValue(QStringLiteral(":priority"), priorities);
    query.bindValue(QStringLiteral(":weight"), weights);
    query.bindValue(QStringLiteral(":subs"), subs);
    query.bindValue(QStringLiteral(":offset"), offsets);
    query.bindValue(QStringLiteral(":subHidden"), subHiddens);

    query.execBatch();

    if (!QSqlDatabase::database().commit()) {
        qWarning() << "[CFGroupModel::submitAll] Can't submit all"
                   << QSqlDatabase::database().lastError();
        return false;
    }

    update_dirtyStatus(false);

    return true;
}

void CFGroupModel::addEmpty(bool desired)
{
    if (m_haveEmpty) {
        emit cantAddEmpty();
        return;
    }

    CFGroup *group = new CFGroup();

    group->set_weight(desired ? 5 : -5);
    group->set_priority(isEmpty() ? 1 : highestPriorityGroup()->get_priority() + 1);

    update_dirtyStatus(true);

    m_emptyIndexes.append(count());
    set_haveEmpty(true);
    append(group);

}

int CFGroupModel::find(int id) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), GroupRoles::Id) == id) {
            return i;
        }
    }
    return INVALID_INDEX;
}

bool CFGroupModel::isUnique(const QString &screenName) const
{
    int count = 0;
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), GroupRoles::ScreenName) == screenName) {
            count++;
            if (count > 1) {
                return false;
            }
        }
    }
    return true;
}

bool CFGroupModel::select()
{
    // Download handlers use pointers to group objects so we need to update these pointer after select
    CFDownloadQueue *downloadQueue = CFUserManager::instance().get_downloadQueue();

    QSqlQuery query;
    query.prepare("SELECT * from Groups;");
    if (!query.exec()) {
        return false;
    }

    QList<CFGroup *> groups;
    while (query.next()) {
        groups.append(new CFGroup(query, this));
        // It is important to update downloads' group pointer
        //before clearing group list  so there will be no dangling pointers
        downloadQueue->replace(groups.last());
    }

    clear();

    append(groups);

    update_dirtyStatus(false);
    set_haveEmpty(false);
    set_emptyIndexes(QList<int>());

    return true;
}

CFGroup *CFGroupModel::highestPriorityGroup() const
{
    if (isEmpty()) {
        return nullptr;
    }

    int highest = 0;

    for (int i = 0; i < count(); i++) {
        if (at(i)->get_priority() > at(highest)->get_priority()) {
            highest = i;
        }
    }

    return at(highest);
}
