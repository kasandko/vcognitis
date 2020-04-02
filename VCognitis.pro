QT += quick webengine sql multimedia concurrent

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "$$PWD/src"
DEPENDPATH += "$$PWD/src"

INCLUDEPATH += "$$PWD/lib-qt-qml-tricks/include"

OBJECTS_DIR  = _obj
MOC_DIR      = _moc
RCC_DIR      = _rcc

SOURCES += \
        main.cpp \
        src/cf.agesearchhandler.cpp \
        src/cf.database.cpp \
        src/cf.downloadhandler.cpp \
        src/cf.downloadqueue.cpp \
        src/cf.global.cpp \
        src/cf.groupassertionhandler.cpp \
        src/cf.initmanager.cpp \
        src/cf.loopedchunkdownloader.cpp \
        src/cf.networkconnect.cpp \
        src/cf.requestsender.cpp \
        src/cf.scorehandler.cpp \
        src/cf.specifyagehandler.cpp \
        src/cf.utils.cpp \
        src/managers/cf.groupmanager.cpp \
        src/managers/cf.usermanager.cpp \
        src/managers/cf.vkauthmanager.cpp \
        src/models/cf.countrymodel.cpp \
        src/models/cf.countrysortfiltermodel.cpp \
        src/models/cf.downloadedgroupproxy.cpp \
        src/models/cf.favoritemodel.cpp \
        src/models/cf.groupmodel.cpp \
        src/models/cf.interestcategorymodel.cpp \
        src/models/cf.interestfilterproxymodel.cpp \
        src/models/cf.interestmodel.cpp \
        src/models/cf.interestproxymodel.cpp \
        src/models/cf.usermodel.cpp \
        src/network/cf.network.cpp \
        src/network/cf.networkutils.cpp \
        src/network/cf.request.cpp \
        src/structs/cf.country.cpp \
        src/structs/cf.group.cpp \
        src/structs/cf.interest.cpp \
        src/structs/cf.interestcategory.cpp \
        src/structs/cf.operror.cpp \
        src/structs/cf.user.cpp

RESOURCES += $$PWD/qml.qrc
RC_FILE = vcognitis.rc
TARGET  = "VCognitis"

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }

    target.path = $$PREFIX/bin

    shortcutfiles.files = resources/VKConfederateFinder.desktop
    shortcutfiles.path = $$PREFIX/share/applications/
    data.files += resources/vcognitis.png
    data.path = $$PREFIX/share/pixmaps/

    INSTALLS += shortcutfiles
    INSTALLS += data
}

INSTALLS += target

DISTFILES += \
    resources/VKConfederateFinder.desktop \
    resources/vcognitis.png \
    version/version.pri


HEADERS += \
    lib-qt-qml-tricks/src/qqmlobjectlistmodel.h \
    src/cf.agesearchhandler.h \
    src/cf.database.h \
    src/cf.downloadhandler.h \
    src/cf.downloadqueue.h \
    src/cf.global.h \
    src/cf.groupassertionhandler.h \
    src/cf.initmanager.h \
    src/cf.loopedchunkdownloader.h \
    src/cf.networkconnect.h \
    src/cf.requestsender.h \
    src/cf.scorehandler.h \
    src/cf.specifyagehandler.h \
    src/cf.utils.h \
    src/managers/cf.groupmanager.h \
    src/managers/cf.usermanager.h \
    src/managers/cf.vkauthmanager.h \
    src/models/cf.countrymodel.h \
    src/models/cf.countrysortfiltermodel.h \
    src/models/cf.downloadedgroupproxy.h \
    src/models/cf.favoritemodel.h \
    src/models/cf.groupmodel.h \
    src/models/cf.interestcategorymodel.h \
    src/models/cf.interestfilterproxymodel.h \
    src/models/cf.interestmodel.h \
    src/models/cf.interestproxymodel.h \
    src/models/cf.usermodel.h \
    src/network/cf.network.h \
    src/network/cf.networkutils.h \
    src/network/cf.request.h \
    src/structs/cf.country.h \
    src/structs/cf.group.h \
    src/structs/cf.interest.h \
    src/structs/cf.interestcategory.h \
    src/structs/cf.operror.h \
    src/structs/cf.user.h

TRANSLATIONS = i18n/vcognitis_ru.ts

include("version/version.pri")
