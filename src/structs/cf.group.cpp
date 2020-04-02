#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "cf.group.h"
#include "cf.global.h"

CFGroup::CFGroup(QObject *parent)
    : QObject(parent),
      m_uid(INVALID_VALUE),
      m_priority(0),
      m_weight(INVALID_VALUE),
      m_subCount(0),
      m_subHidden(false),
      m_offset(0),
      m_validated(false)
{
    initConnections();
}

CFGroup::CFGroup(const QSqlQuery &query, QObject *parent)
    : QObject(parent),
      m_validated(true)
{
    m_uid = query.value(QLatin1String("id")).toInt();
    m_screenName = query.value(QLatin1String("screenName")).toString();
    m_name = query.value(QLatin1String("name")).toString();
    m_photo = query.value(QLatin1String("photo")).toString();
    m_priority = query.value(QLatin1String("priority")).toInt();
    m_weight = query.value(QLatin1String("weight")).toInt();
    m_subCount = query.value(QLatin1String("subCount")).toInt();
    m_subHidden = query.value(QLatin1String("subHidden")).toBool();
    m_offset = query.value(QLatin1String("offset")).toInt();

    initConnections();
}

bool CFGroup::downloaded() const
{
    return m_uid != INVALID_VALUE && m_offset >= m_subCount;
}

double CFGroup::progress() const
{
    return m_subCount ? std::min(static_cast<double>(m_offset) / m_subCount, 1.) : 0;
}

void CFGroup::setAndPersistOffset(int offset)
{
    if (m_offset != offset) {
        QSqlQuery query;
        query.prepare(QStringLiteral("UPDATE Groups SET offset=:offset WHERE id=:id"));
        query.bindValue(QStringLiteral(":offset"), offset);
        query.bindValue(QStringLiteral(":id"), m_uid);
        if (!query.exec()) {
            qWarning() << "[CFGroup::setAndPersistOffset] Update offset failed; uid: " + QString::number(m_uid);
        } else {
            m_offset = offset;
            emit offsetChanged(m_offset);
        }
    }
}

void CFGroup::setAndPersistSubCount(int subCount)
{
    if (m_subCount != subCount) {
        QSqlQuery query;
        query.prepare(QStringLiteral("UPDATE Groups SET subCount=:subCount WHERE id=:id"));
        query.bindValue(QStringLiteral(":subCount"), subCount);
        query.bindValue(QStringLiteral(":id"), m_uid);
        if (!query.exec()) {
            qWarning() << "[CFGroup::setAndPersistSubCount] Update subCount failed; uid: " + QString::number(m_uid);
        } else {
            m_subCount = subCount;
            emit subCountChanged(m_subCount);
        }
    }
}

void CFGroup::setAndPersistSubHidden(bool subHidden)
{
    if (m_subHidden != subHidden) {
        QSqlQuery query;
        query.prepare(QStringLiteral("UPDATE Groups SET subHidden=:subHidden WHERE id=:id"));
        query.bindValue(QStringLiteral(":subHidden"), subHidden);
        query.bindValue(QStringLiteral(":id"), m_uid);
        if (!query.exec()) {
            qWarning() << "[CFGroup::setAndPersistSubHidden] Update subHidden failed; uid: "
                          + QString::number(m_uid);
        } else {
            m_subHidden = subHidden;
            emit subHiddenChanged(subHidden);
        }
    }
}

void CFGroup::initConnections()
{
    connect(this, &CFGroup::offsetChanged, this, &CFGroup::progressChanged);
    connect(this, &CFGroup::subCountChanged, this, &CFGroup::progressChanged);
    connect(this, &CFGroup::offsetChanged, [this]() {
        if (!m_offset || m_offset >= m_subCount) {
            emit downloadedChanged();
        }
    });
    connect(this, &CFGroup::subCountChanged, [this]() {
        if (m_offset >= m_subCount) {
            emit downloadedChanged();
        }
    });
}
