#include <QSqlQuery>
#include <QVariant>

#include "cf.country.h"
#include "cf.global.h"

CFCountry::CFCountry(QObject *parent)
    :QObject(parent),
     m_uid(INVALID_VALUE),
     m_name(tr("Не выбрано"))
{

}

CFCountry::CFCountry(const QSqlQuery &query, QObject *parent)
    : QObject(parent)
{
    m_uid = query.value(QLatin1String("id")).toInt();
    m_name = query.value(QLatin1String("name")).toString();
}
