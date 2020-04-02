#include "cf.networkconnect.h"
#include <QCoreApplication>

#define UPDATE_TIME 4000

CFNetworkConnect::CFNetworkConnect(QObject *parent)

    : QObject (parent),
      m_isOnline(true)
{
    m_worker.moveToThread(&m_workerThread);

    connect(&m_worker, &CFNetworkConnectWorker::onlineReceived, this, &CFNetworkConnect::update_isOnline);

    m_workerThread.start();
    QTimer::singleShot(0, &m_worker, &CFNetworkConnectWorker::checkOnline);
}

CFNetworkConnect::~CFNetworkConnect()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void CFNetworkConnectWorker::checkOnline()
{
    if (m_manager == nullptr) {
        m_manager = new QNetworkAccessManager;
    }

    QNetworkReply *reply = m_manager->get(QNetworkRequest(QUrl("https://google.com")));

    connect(reply,  &QNetworkReply::finished, [this, reply](){
        if (reply->error() == QNetworkReply::NoError)
            emit onlineReceived(true);
        else
            emit onlineReceived(false);
    });

    QTimer::singleShot(UPDATE_TIME, [this, reply](){
        if (!reply->isFinished()) {
            reply->abort();
            emit onlineReceived(false);
        }

        reply->deleteLater();
    });

    if (m_checkTimer == nullptr) {
        m_checkTimer = new QTimer;
        connect(m_checkTimer, &QTimer::timeout, this, &CFNetworkConnectWorker::checkOnline);
        m_checkTimer->start(UPDATE_TIME);
    }
}
