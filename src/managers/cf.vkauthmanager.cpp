#include "cf.vkauthmanager.h"

#include <QUrl>
#include <QUrlQuery>
#include <QSettings>
#include <QCoreApplication>
#include <QTimer>

#include "network/cf.request.h"

#include "cf.database.h"
#include "cf.utils.h"

namespace URL {
    QUrl AuthUrl = QUrl("https://oauth.vk.com/authorize");
    QUrl AuthRedirectUrl = QUrl("https://oauth.vk.com/blank.html");
}

CFVkAuthManager::CFVkAuthManager(QObject *parent)
    : QObject(parent),
      m_token(QSettings().value(CFSetting::Token).toString()),
      m_userVkId(QSettings().value(CFSetting::UserVkId).toString())
{
    connect(this, &CFVkAuthManager::tokenChanged,
            [=]() { QSettings().setValue(CFSetting::Token, m_token); });
    connect(this, &CFVkAuthManager::userVkIdChanged,
            [=]() {
        QSettings().setValue(CFSetting::UserVkId, m_userVkId);
    });
    connect(this, &CFVkAuthManager::authSuccess,
            this, &CFVkAuthManager::makeTrackVisitorRequest);

    if (m_token.isEmpty()) {
        QTimer::singleShot(0, this, &CFVkAuthManager::authRequired);
    } else {
        CFDatabase::init(m_userVkId);
        // Delay is needed for interface to complete rendering
        QTimer::singleShot(3000, this, &CFVkAuthManager::authSuccess);
    }
}

void CFVkAuthManager::loginOut()
{
    update_token("");

    QTimer::singleShot(0, this, &CFVkAuthManager::authRequired);
    emit loggedOut();
}

QString CFVkAuthManager::authUrl()
{
    QUrl url = URL::AuthUrl;
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("client_id"), CFGlobal::APP_VK_ID);

    quint32 permissions = Groups | Offline;
    query.addQueryItem(QStringLiteral("scope"), QString::number(permissions));

    query.addQueryItem(QStringLiteral("display"), QStringLiteral("page"));
    query.addQueryItem(QStringLiteral("redirect_uri"), URL::AuthRedirectUrl.url());
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("response_type"), QStringLiteral("token"));
    url.setQuery(query);

    return url.url();
}

QString CFVkAuthManager::authRedirectUrl()
{
    return URL::AuthRedirectUrl.url();
}

void CFVkAuthManager::parseAuthResult(const QString &redirectUrl)
{
    QUrl url(redirectUrl);
    QUrlQuery query(url.fragment());
    if (query.hasQueryItem(QStringLiteral("access_token"))) {
        update_token(query.queryItemValue(QStringLiteral("access_token")));
        update_userVkId(query.queryItemValue(QStringLiteral("user_id")));

        CFDatabase::init(m_userVkId);

        CFUtils::instance().appModeFetch();

        emit authSuccess();
    } else {
        emit authError(
            query.queryItemValue(QStringLiteral("error")),
            query.queryItemValue(QStringLiteral("error_description"))
        );
    }
}

void CFVkAuthManager::makeTrackVisitorRequest()
{
    QUrl url(QStringLiteral("/method/stats.trackVisitor"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("v"), CFGlobal::VK_API_VERSION);
    query.addQueryItem(QStringLiteral("access_token"), CFVkAuthManager::instance().get_token());
    url.setQuery(query);

    CFRequest *req = new CFRequest(url);
    connect(req,  &CFRequest::error,  [](int  error, const  QString  &errorMessage) {
        qDebug() << "[CFVkAuthManager::makeTrackVisitorRequest] Error: " + QString::number(error)
                    + "; message: " + errorMessage;
    });

    CFGlobal::vkApiRequestSender().send(req);
}
