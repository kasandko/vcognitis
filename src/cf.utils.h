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

#ifndef CFUTILS_H
#define CFUTILS_H

#include <QObject>

#include "QQmlHelpers"

#include "cf.global.h"
#include "version.h"

class CFUtils : public QObject
{
    Q_OBJECT
    SINGLETON(CFUtils)
    QML_WRITABLE_PROPERTY(int, appMode)
    QML_CONSTANT_PROPERTY(QString, appVersion)
    QML_CONSTANT_PROPERTY(QString, appCommit)

public:
    explicit CFUtils(QObject *parent = nullptr);

    static qint64 microsecsSinceEpoch();
    /**
     * @brief Serializes int list into string to save in settings
     */
    static QString serialize(const QList<int> &intList);
    /**
     * @brief Deserializes string got from settings as int list
     * @param str
     * @return
     */
    static QList<int> deserialize(const QString &str);

    /**
     * Update appMode from settings file
     */
    void appModeFetch();

signals:

public slots:
    /*!
     * \brief subsToString converts num representation on sub to word equivalent 150000 to 150k
     * \param subCount
     * \return
     */
    static QString subsToString(int subCount);

    static QString viewedToString(int viewedCount);

    /*!
     * \brief Converts seconds to "d д. h ч. m мин." string
     */
    static QString secs2TimeString(int secs);

    static bool openUrlInDesktopBrowser(const QString &url);
};

#endif // CFUTILS_H
