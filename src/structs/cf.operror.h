/****************************************************************************
**
** Copyright © 2020 Barbatum™.
** All rights reserved.
** Contact: support@barbatum.ru
**
**
** This file is part of VCognitis project.
**
** VCognitis is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** VCognitis is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with VCognitis.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CFOPERROR_H
#define CFOPERROR_H

#include <QObject>
#include <QDateTime>

#include "QQmlHelpers"

#define CF_OPERROR_TYPENAME "CFOpError"

/*!
 * \brief The CFOpError class encapsulates info about errors
 * that can occur during downloading and scoring operations
 */
class CFOpError : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(int, code)
    QML_READONLY_PROPERTY(QString, userfriendlyMessage)
    QML_READONLY_PROPERTY(QString, technicalMessage)
    QML_READONLY_PROPERTY(QDateTime, timestamp)
    QML_READONLY_PROPERTY(int, groupId) // Used when applicable
    QML_READONLY_PROPERTY(QString, source) // Procedure that caused error
public:
    explicit CFOpError(QObject *parent = nullptr);
    CFOpError(int code,
              const QString &userfriendlyMessage,
              const QString &technicalMessage,
              QObject *parent = nullptr);

signals:

};

#endif // CFOPERROR_H
