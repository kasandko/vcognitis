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

#ifndef CFGROUP_H
#define CFGROUP_H

#include <QObject>

#include "QQmlHelpers"

#define CF_GROUP_TYPENAME "CFGroup"

class QSqlQuery;

/*!
 * \brief The CFGroup class encapsulates group entity
 */
class CFGroup : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(int, uid);
    QML_WRITABLE_PROPERTY(QString, screenName);
    QML_WRITABLE_PROPERTY(QString, name);
    QML_WRITABLE_PROPERTY(QString, photo);
    QML_WRITABLE_PROPERTY(int, priority);
    QML_WRITABLE_PROPERTY(int, weight);
    QML_WRITABLE_PROPERTY(int, subCount);
    QML_WRITABLE_PROPERTY(bool, subHidden);
    QML_WRITABLE_PROPERTY(int, offset);
    QML_WRITABLE_PROPERTY(bool, validated);
    Q_PROPERTY(bool downloaded READ downloaded NOTIFY downloadedChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged) // [0.0, 1.0]

public:
    explicit CFGroup(QObject *parent = nullptr);
    explicit CFGroup(const QSqlQuery &query, QObject *parent = nullptr);

    bool downloaded() const;

    double progress() const;

public slots:
    void setAndPersistOffset(int offset);
    void setAndPersistSubCount(int subCount);
    void setAndPersistSubHidden(bool subHidden);

signals:
    void downloadedChanged();
    void progressChanged();

private:
    void initConnections();
};

#endif // CFGROUP_H
