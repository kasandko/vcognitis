#include "cf.favoritemodel.h"

#include <QSqlQuery>

#include "cf.utils.h"

CFFavoriteModel::CFFavoriteModel(QObject *parent,
                                 const QByteArray &displayRole,
                                 const QByteArray &uidRole)
    : QQmlObjectListModel(parent, displayRole, uidRole)

{

}

bool CFFavoriteModel::select()
{
    emit askResetPosition();

    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT *,groupScore+interestScore AS fullScore FROM Users WHERE favorite=1 ORDER BY %1 DESC")
                  .arg("fullScore"));
    if (!query.exec()) {
        return false;
    }

    clear();

    QList<CFUser *> users;
    while (query.next()) {
        users.append(new CFUser(query, this));
    }

    append(users);

    return true;
}
