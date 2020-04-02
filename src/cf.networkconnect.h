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

#ifndef CF_NETWORKCONFIGWRAPPER_H
#define CF_NETWORKCONFIGWRAPPER_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include "QQmlHelpers"

class CFNetworkConnectWorker : public QObject {
    Q_OBJECT

signals:
    void onlineReceived(bool isOnline);

public slots:
    void checkOnline();

private:
    QNetworkAccessManager *m_manager = nullptr;
    QTimer *m_checkTimer = nullptr;
};

class CFNetworkConnect: public QObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, isOnline)
public:
    explicit CFNetworkConnect(QObject *parent = nullptr);
    ~CFNetworkConnect();

signals:
    void nextRequest();

private:
    CFNetworkConnectWorker m_worker;
    QThread m_workerThread;
};

#endif // CF_NETWORKCONFIGWRAPPER_H
