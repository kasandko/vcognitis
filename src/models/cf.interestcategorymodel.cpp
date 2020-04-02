#include "cf.interestcategorymodel.h"

#include "cf.global.h"

#include <QSqlQuery>


CFInterestCategoryModel::CFInterestCategoryModel(QObject *parent, const QByteArray &displayRole, const QByteArray &uidRole)
    : QQmlObjectListModel(parent, displayRole, uidRole)
{
}

int CFInterestCategoryModel::find(const QString &name) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), InterestCategoryRoles::Name) == name) {
            return i;
        }
    }
    return INVALID_INDEX;
}

bool CFInterestCategoryModel::select()
{
    QSqlQuery query;
    query.prepare("SELECT * from InterestCategory;");
    if (!query.exec()) {
        return false;
    }

    clear();

    QList<CFInterestCategory *> interests;
    while (query.next()) {
        interests.append(new CFInterestCategory(query, this));
    }

    append(interests);

    return true;
}
