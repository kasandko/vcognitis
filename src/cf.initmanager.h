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

#ifndef CFINITMANAGER_H
#define CFINITMANAGER_H

#include <QObject>

#include "QQmlObjectListModel"
#include "cf.networkconnect.h"
#include "structs/cf.operror.h"

class QQmlApplicationEngine;

class CFInitManager : public QObject
{
    Q_OBJECT
public:
    explicit CFInitManager(int argc, char *argv[], QObject *parent = nullptr);

    void init(QQmlApplicationEngine &engine);

signals:

public slots:

private slots:
    void processError(CFOpError *error);

private:
    CFNetworkConnect *m_networkConnect;
    QQmlObjectListModel<CFOpError> *m_errorModel;
};

Q_DECLARE_METATYPE(QQmlObjectListModel<CFOpError> *)

#endif // CFINITMANAGER_H
