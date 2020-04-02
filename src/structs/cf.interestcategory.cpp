#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>


#include "cf.interestcategory.h"
#include "cf.global.h"

CFInterestCategory::CFInterestCategory(QObject *parent)
    : QObject(parent),
      m_uid(INVALID_VALUE)
{

}

CFInterestCategory::CFInterestCategory(const QSqlQuery &query, QObject *parent)
    : QObject(parent)
{
    m_uid = query.value(QLatin1String("id")).toInt();
    m_name = query.value(QLatin1String("name")).toString();
    m_tags = query.value(QLatin1String("tags")).toString();
}
