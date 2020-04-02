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

#ifndef CF_GLOBAL_H
#define CF_GLOBAL_H

#include <QObject>

#define SINGLETON(C)                  \
public:                               \
    C(const C &) = delete;            \
    C &operator=(const C &) = delete; \
    static C &instance()              \
    {                                 \
       static C _instance;            \
       return _instance;              \
    }                                 \
private:                              \

namespace {
constexpr const int INVALID_INDEX = -1;
constexpr const int INVALID_VALUE = -1;
}

class GroupRolesClass {
    Q_GADGET
public:
    enum Enum {
        Id = Qt::UserRole + 2,
        ScreenName,
        Name,
        Photo,
        Priority,
        Weight,
        SubCount,
        SubHidden,
        Offset
    };
    Q_ENUM(Enum)
};
typedef GroupRolesClass::Enum GroupRoles;
#define CF_GROUPROLES_TYPENAME "CFGroupRoles"

class UserRolesClass {
    Q_GADGET
public:
    enum Enum {
        Id = Qt::UserRole + 1,
        ScreenName,
        VkId,
        FirstName,
        LastName,
        City,
        Country,
        Sex,
        Bdate,
        PhotoMax,
        Relation,
        Religion,
        Political,
        PeopleMain,
        LifeMain,
        Smoking,
        Alcohol,
        GroupScore,
        InterestScore,
        Scored,
        IsFavorite,
        DesiredGroups,
        UndesiredGroups,
        Age,
        OpenMessages
    };
    Q_ENUM(Enum)
};
typedef UserRolesClass::Enum UserRoles;
#define CF_USERROLES_TYPENAME "CFUserRoles"

class CountryRolesClass {
    Q_GADGET
public:
    enum Enum {
        Id = Qt::UserRole + 2,
        Name,
    };
    Q_ENUM(Enum)
};
typedef CountryRolesClass::Enum CountryRoles;
#define CF_COUNTRYROLES_TYPENAME "CFCountryRoles"

class InterestCategoryClass {
    Q_GADGET
public:
    enum Enum {
        Id = Qt::UserRole + 2,
        Name,
        Tags
    };
    Q_ENUM(Enum)
};
typedef InterestCategoryClass::Enum InterestCategoryRoles;
#define CF_INTERESTCATEGORY_TYPENAME "CFInterestCategoryRoles"

class InterestClass {
    Q_GADGET
public:
    enum Enum {
        Id = Qt::UserRole + 2,
        Name,
        Tags,
        ParentId
    };
    Q_ENUM(Enum)
};
typedef InterestClass::Enum InterestRoles;
#define CF_INTEREST_TYPENAME "CFInterestRoles"

namespace Personal {
class SexClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        Female,
        Male,
        Both
    };
    Q_ENUM(Enum)
};
typedef SexClass::Enum Sex;
#define CF_PERSONAL_SEX_TYPENAME "CFPersonalSex"

class ReligionClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        Judaism,
        Orthodoxy,
        Catholicism,
        Protestantism,
        Islam,
        Buddhism,
        Confucianism,
        SecularHumanism,
        Pastafarianism
    };
    Q_ENUM(Enum)
};
typedef ReligionClass::Enum Religion;
#define CF_PERSONAL_RELIGION_TYPENAME "CFPersonalReligion"

class RelationClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        Single,
        InRelationship,
        Engaged,
        Married,
        Complicated,
        Searching,
        InLove,
        CivilUnion
    };
    Q_ENUM(Enum)
};
typedef RelationClass::Enum Relation;
#define CF_PERSONAL_RELATION_TYPENAME "CFPersonalRelation"

class PoliticalClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        Communist,
        Socialist,
        Moderate,
        Liberal,
        Conservative,
        Monarchist,
        Ultraconservative,
        Apathetic,
        Libertian
    };
    Q_ENUM(Enum)
};
typedef PoliticalClass::Enum Political;
#define CF_PERSONAL_POLITICAL_TYPENAME "CFPersonalPolitical"

class PeopleMainClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        IntellectCreativity,
        KindnessHonesty,
        HealthBeauty,
        WealthPower,
        CouragePersistense,
        HumourLoveForLife
    };
    Q_ENUM(Enum)
};
typedef PeopleMainClass::Enum PeopleMain;
#define CF_PERSONAL_PEOPLEMAIN_TYPENAME "CFPersonalPeopleMain"

class LifeMainClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        FamilyChildren,
        CareerMoney,
        EntertainmentLeisure,
        ScienceResearch,
        ImprovingWorld,
        PersonalDevelopment,
        BeautyArt,
        FameInfluence
    };
    Q_ENUM(Enum)
};
typedef LifeMainClass::Enum LifeMain;
#define CF_PERSONAL_LIFEMAIN_TYPENAME "CFPersonalLifeMain"

class SmokingClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        VeryNegative,
        Negative,
        Compromisable,
        Neutral,
        Positive
    };
    Q_ENUM(Enum)
};
typedef SmokingClass::Enum Smoking;
#define CF_PERSONAL_SMOKING_TYPENAME "CFPersonalSmoking"

class AlcoholClass {
    Q_GADGET
public:
    enum Enum {
        Undefined = 0,
        VeryNegative,
        Negative,
        Compromisable,
        Neutral,
        Positive
    };
    Q_ENUM(Enum)
};
typedef AlcoholClass::Enum Alcohol;
#define CF_PERSONAL_ALCOHOL_TYPENAME "CFPersonalAlcohol"
}

class MainScreenViewClass {
    Q_GADGET

public:
    enum Enum {
        Search,
        Favorite,
        Groups
    };
    Q_ENUM(Enum)
};
#define CF_MAINSCREENVIEW_TYPENAME "CFMainScreenView"
typedef MainScreenViewClass::Enum MainScreenView;

class AppModeClass {
    Q_GADGET

public:
    enum Enum {
        Quick = 0,
        Accurate
    };
    Q_ENUM(Enum)
};
#define CF_APPMODE_TYPENAME "CFAppMode"
typedef AppModeClass::Enum AppMode;

class CFRequestSender;

namespace CFSetting {
    // Auth
    const QString Token = "api_access_token";
    const QString UserVkId = "user_vk_id";

    QString interests();

    QString appMode();

    namespace Filter {
        QString country();
        QString city();
        QString sex();
        QString fromAge();
        QString toAge();
        QString photo();
        QString political();
        QString religion();
        QString lifeMain();
        QString peopleMain();
        QString smoking();
        QString alcohol();
        QString relation();
        QString groups();
        QString limit();
        QString searchAge();
        QString showViewed();
        QString exact();
    }
}

namespace CFGlobal
{
    const QString APP_VK_ID = "6872587";
    const QString VK_API_VERSION = "5.103";
    const QString VK_HOST_PREFIX = "https://vk.com";

    CFRequestSender &vkApiRequestSender();
}

namespace CFAPIError
{
    const int TOO_MANY_REQUESTS = 6;
    const int ACCESS_DENIED = 15;
    const int DELETED_OR_BANNED = 18;
    const int PRIVATE_PROFILE = 30;
    const int INVALID_GROUP_ID = 125;
    const int ACCESS_TO_GROUP_DENIED = 203;
}

#endif // CF_GLOBAL_H
