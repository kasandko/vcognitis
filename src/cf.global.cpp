#include "cf.global.h"

#include <QCoreApplication>

#include "cf.requestsender.h"
#include "managers/cf.vkauthmanager.h"

CFRequestSender &CFGlobal::vkApiRequestSender()
{
    static CFRequestSender requestSender;
    static bool inited = false;
    if (!inited) {
        QString agentStr = QString::fromLatin1("%1/%2 (QNetworkAccessManager %3; %4; %5 bit)")
                    .arg(qApp->applicationName())
                    .arg(qApp->applicationVersion())
                    .arg(QSysInfo::prettyProductName())
                    .arg(QLocale::system().name())
                    .arg(QSysInfo::WordSize);

        requestSender.addSharedHeader(QNetworkRequest::UserAgentHeader, agentStr);
        requestSender.setHost(QUrl(QStringLiteral("https://api.vk.com")));

        inited = true;
    }
    return requestSender;
}

QString CFSetting::interests()
{
    return CFVkAuthManager::instance().get_userVkId() + "/interests";
}

QString CFSetting::Filter::country()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/country";
}

QString CFSetting::Filter::city()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/city";
}

QString CFSetting::Filter::sex()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/sex";
}

QString CFSetting::Filter::photo()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/photo";
}

QString CFSetting::Filter::political()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/political";
}

QString CFSetting::Filter::religion()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/religion";
}

QString CFSetting::Filter::lifeMain()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/lifeMain";
}

QString CFSetting::Filter::peopleMain()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/peopleMain";
}

QString CFSetting::Filter::smoking()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/smoking";
}

QString CFSetting::Filter::alcohol()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/alcohol";
}

QString CFSetting::Filter::relation()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/relation";
}

QString CFSetting::Filter::groups()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/groups";
}

QString CFSetting::Filter::limit()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/limit";
}

QString CFSetting::Filter::showViewed()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/showViewed";
}

QString CFSetting::Filter::searchAge()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/searchAge";
}

QString CFSetting::appMode()
{
    return CFVkAuthManager::instance().get_userVkId() + "/appMode";
}

QString CFSetting::Filter::fromAge()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/fromAge";
}

QString CFSetting::Filter::toAge()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/toAge";
}

QString CFSetting::Filter::exact()
{
    return CFVkAuthManager::instance().get_userVkId() + "/filter/exact";
}
