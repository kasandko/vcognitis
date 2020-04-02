#include "cf.initmanager.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>
#include <QCoreApplication>
#include <QSettings>

#include "cf.database.h"
#include "cf.utils.h"
#include "managers/cf.vkauthmanager.h"
#include "managers/cf.groupmanager.h"
#include "managers/cf.usermanager.h"

#define CF_APPURI "ConfederateFinder"

QString UNCREATABLE_ENUM_MESSAGE = "Enums are uncreatable";
QString UNCREATABLE_GENERAL_MESSAGE = "Provided class is uncreatable";

CFInitManager::CFInitManager(int argc, char *argv[], QObject *parent) : QObject(parent)
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    QCoreApplication::setOrganizationName(QStringLiteral("Barbatum"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("barbatum.ru"));
    QCoreApplication::setApplicationName(QStringLiteral("VCognitis"));

    QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/Roboto-Medium.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/Roboto-Regular.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/Roboto-Black.ttf"));
    QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/Roboto-Light.ttf"));
}

void CFInitManager::init(QQmlApplicationEngine &engine)
{
    // Needed so CFDownloadHandler::parseMembers would run sequentially one after another
    QThreadPool::globalInstance()->setMaxThreadCount(1);

    m_networkConnect = new CFNetworkConnect(this);
    m_errorModel = new QQmlObjectListModel<CFOpError>(this);


    // Enum registration
    qRegisterMetaType<GroupRoles>(CF_GROUPROLES_TYPENAME);
    qmlRegisterUncreatableType<GroupRolesClass>(CF_APPURI, 1, 0,
                                                CF_GROUPROLES_TYPENAME,
                                                UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<CountryRoles>(CF_COUNTRYROLES_TYPENAME);
    qmlRegisterUncreatableType<CountryRolesClass>(CF_APPURI, 1, 0,
                                               CF_COUNTRYROLES_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<InterestRoles>(CF_INTEREST_TYPENAME);
    qmlRegisterUncreatableType<InterestClass>(CF_APPURI, 1, 0,
                                               CF_INTEREST_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::Relation>(CF_PERSONAL_RELATION_TYPENAME);
    qmlRegisterUncreatableType<Personal::RelationClass>(CF_APPURI, 1, 0,
                                               CF_PERSONAL_RELATION_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::Religion>(CF_PERSONAL_RELIGION_TYPENAME);
    qmlRegisterUncreatableType<Personal::ReligionClass>(CF_APPURI, 1, 0,
                                                CF_PERSONAL_RELIGION_TYPENAME,
                                                UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::PeopleMain>(CF_PERSONAL_PEOPLEMAIN_TYPENAME);
    qmlRegisterUncreatableType<Personal::PeopleMainClass>(CF_APPURI, 1, 0,
                                               CF_PERSONAL_PEOPLEMAIN_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::LifeMain>(CF_PERSONAL_LIFEMAIN_TYPENAME);
    qmlRegisterUncreatableType<Personal::LifeMainClass>(CF_APPURI, 1, 0,
                                               CF_PERSONAL_LIFEMAIN_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::Smoking>(CF_PERSONAL_SMOKING_TYPENAME);
    qmlRegisterUncreatableType<Personal::SmokingClass>(CF_APPURI, 1, 0,
                                               CF_PERSONAL_SMOKING_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::Alcohol>(CF_PERSONAL_ALCOHOL_TYPENAME);
    qmlRegisterUncreatableType<Personal::AlcoholClass>(CF_APPURI, 1, 0,
                                               CF_PERSONAL_ALCOHOL_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::Political>(CF_PERSONAL_POLITICAL_TYPENAME);
    qmlRegisterUncreatableType<Personal::PoliticalClass>(CF_APPURI, 1, 0,
                                               CF_PERSONAL_POLITICAL_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<Personal::Sex>(CF_PERSONAL_SEX_TYPENAME);
    qmlRegisterUncreatableType<Personal::SexClass>(CF_APPURI, 1, 0,
                                               CF_PERSONAL_SEX_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<MainScreenView>(CF_MAINSCREENVIEW_TYPENAME);
    qmlRegisterUncreatableType<MainScreenViewClass>(CF_APPURI, 1, 0,
                                               CF_MAINSCREENVIEW_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);
    qRegisterMetaType<AppMode>(CF_APPMODE_TYPENAME);
    qmlRegisterUncreatableType<AppModeClass>(CF_APPURI, 1, 0,
                                               CF_APPMODE_TYPENAME,
                                               UNCREATABLE_ENUM_MESSAGE);

    // Struct types
    qmlRegisterType<CFOpError>(CF_APPURI, 1, 0,
                               CF_OPERROR_TYPENAME);
    qmlRegisterType<CFGroup>(CF_APPURI, 1, 0,
                             CF_GROUP_TYPENAME);


    // Model registration
    qmlRegisterUncreatableType<CFGroupModel>(CF_APPURI, 1, 0,
                                             CF_GROUPMODEL_TYPENAME,
                                             UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFUserModel>(CF_APPURI, 1, 0,
                                            CF_USERMODEL_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFFavoriteModel>(CF_APPURI, 1, 0,
                                            CF_FAVORITEMODEL_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFCountryModel>(CF_APPURI, 1, 0,
                                            CF_COUNTRYMODEL_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFCountrySortFilterModel>(CF_APPURI, 1, 0,
                                            CF_COUNTRYSORTFILTERPROXY_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<QQmlObjectListModel<CFOpError>>(CF_APPURI, 1, 0,
                                            CF_OPERRORLIST_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFInterestProxyModel>(CF_APPURI, 1, 0,
                                            CF_INTERESTPROXY_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFDownloadedGroupProxy>(CF_APPURI, 1, 0,
                                            CF_DOWNLOADEDGROUPPROXY_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<QQmlObjectListProxyModel>(CF_APPURI, 1, 0,
                                            QQMLOBJECTLISTPROXYMODEL_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFLoopedChunkDownloader>(CF_APPURI, 1, 0,
                                            CF_LOOPEDCHUNKDOWNLOADER_TYPENAME,
                                            UNCREATABLE_GENERAL_MESSAGE);


    // Other types
    qmlRegisterUncreatableType<CFDownloadHandler>(CF_APPURI, 1, 0,
                                                  CF_DOWNLOADHANDLER_TYPENAME,
                                                  UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFScoreHandler>(CF_APPURI, 1, 0,
                                               CF_SCOREHANDLER_TYPENAME,
                                               UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFAgeSearchHandler>(CF_APPURI, 1, 0,
                                               CF_AGESEARCHHANDLER_TYPENAME,
                                               UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFSpecifyAgeHandler>(CF_APPURI, 1, 0,
                                               CF_SPECIFYAGEHANDLER_TYPENAME,
                                               UNCREATABLE_GENERAL_MESSAGE);
    qmlRegisterUncreatableType<CFDownloadQueue>(CF_APPURI, 1, 0,
                                               CF_DOWNLOADQUEUE_TYPENAME,
                                               UNCREATABLE_GENERAL_MESSAGE);

    connect(&CFGroupManager::instance(), &CFGroupManager::opError,
            this, &CFInitManager::processError);
    connect(&CFUserManager::instance(), &CFUserManager::opError,
            this, &CFInitManager::processError);


    engine.rootContext()->setContextProperty(QLatin1String("vkAuthManager"),
                                             &CFVkAuthManager::instance());
    engine.rootContext()->setContextProperty(QLatin1String("groupManager"),
                                             &CFGroupManager::instance());
    engine.rootContext()->setContextProperty(QLatin1String("userManager"),
                                             &CFUserManager::instance());
    engine.rootContext()->setContextProperty(QLatin1String("utils"),
                                             &CFUtils::instance());

    engine.rootContext()->setContextProperty(QLatin1String("networkConnect"),
                                             m_networkConnect);
    engine.rootContext()->setContextProperty(QLatin1String("errorModel"),
                                             m_errorModel);
}

void CFInitManager::processError(CFOpError *error)
{
    int errorCode = error->get_code();
    if (errorCode <= QNetworkReply::UnknownServerError ||
        errorCode == QNetworkReply::UnknownServerError + CFAPIError::TOO_MANY_REQUESTS) {
        error->deleteLater();
        return;
    }

    error->update_code(errorCode - QNetworkReply::UnknownServerError);
    error->setParent(m_errorModel);
    m_errorModel->append(error);
}
