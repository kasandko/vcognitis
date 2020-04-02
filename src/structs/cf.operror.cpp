#include "cf.operror.h"

#include "cf.global.h"

CFOpError::CFOpError(QObject *parent)
    : QObject(parent),
      m_code(INVALID_VALUE),
      m_timestamp(QDateTime::currentDateTime()),
      m_groupId(INVALID_VALUE)
{

}

CFOpError::CFOpError(int code,
                     const QString &userfriendlyMessage,
                     const QString &technicalMessage, QObject *parent)
    : QObject(parent),
      m_code(code),
      m_userfriendlyMessage(userfriendlyMessage),
      m_technicalMessage(technicalMessage),
      m_timestamp(QDateTime::currentDateTime()),
      m_groupId(INVALID_VALUE)
{

}
