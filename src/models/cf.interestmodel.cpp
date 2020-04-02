#include "cf.interestmodel.h"

#include "cf.global.h"

#include <QSqlQuery>

CFInterestModel::CFInterestModel(QObject *parent, const QByteArray &displayRole, const QByteArray &uidRole)
    : QQmlObjectListModel(parent, displayRole, uidRole)
{
}

int CFInterestModel::find(const QString &name) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), InterestRoles::Name) == name) {
            return i;
        }
    }
    return INVALID_INDEX;
}

int CFInterestModel::idByName(const QString &name) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), InterestRoles::Name) == name) {
            return data(index(i, 0), InterestRoles::Id).toInt();
        }
    }
    return INVALID_VALUE;
}

int CFInterestModel::find(const int id) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), InterestRoles::Id) == id) {
            return i;
        }
    }
    return INVALID_INDEX;
}

bool CFInterestModel::select()
{
    QSqlQuery query;
    query.prepare("SELECT * from Interest;");
    if (!query.exec()) {
        return false;
    }

    clear();

    QList<CFInterest *> interests;
    while (query.next()) {
        interests.append(new CFInterest(query, this));
    }

    append(interests);

    return true;
}

