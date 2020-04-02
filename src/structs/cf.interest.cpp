#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "cf.interest.h"
#include "cf.global.h"

CFInterest::CFInterest(QObject *parent)
    : QObject(parent),
      m_uid(INVALID_VALUE),
      m_parentId(INVALID_VALUE)
{

}

CFInterest::CFInterest(const QSqlQuery &query, QObject *parent)
    : QObject(parent)
{
    m_uid = query.value(QLatin1String("id")).toInt();
    m_name = query.value(QLatin1String("name")).toString();
    m_tags = query.value(QLatin1String("tags")).toString();
    m_parentId = query.value(QLatin1String("parentId")).toInt();

}
