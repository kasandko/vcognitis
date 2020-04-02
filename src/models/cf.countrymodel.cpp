#include "cf.countrymodel.h"

#include "cf.global.h"


#include <QSqlQuery>


CFCountryModel::CFCountryModel(QObject *parent, const QByteArray &displayRole, const QByteArray &uidRole)
    : QQmlObjectListModel(parent, displayRole, uidRole)
{
}

int CFCountryModel::find(int id) const
{
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i, 0), CountryRoles::Id) == id) {
            return i;
        }
    }
    return INVALID_INDEX;
}

bool CFCountryModel::select()
{
    QSqlQuery query;
    query.prepare("SELECT * from Countries;");
    if (!query.exec()) {
        return false;
    }

    clear();

    QList<CFCountry *> countries;
    while (query.next()) {
        countries.append(new CFCountry(query, this));
    }

    append(countries);
    append(new CFCountry(this));

    return true;
}
