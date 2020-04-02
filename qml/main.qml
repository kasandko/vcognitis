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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.12
import ConfederateFinder 1.0

import "./ScreenComponents"
import "./ScreenComponents/CFFilterDrawer"
import "./Popups"
import "./SimpleControls"
import "./SpecialControls"
import "./Singletons"

ApplicationWindow {
    id: appWindow

    property int currentView: CFMainScreenView.Search
    property bool greater: vkAuthManager.token == ""
    property bool loggedIn: vkAuthManager.token != ""

    property bool closeAccepted: !(groupManager.groupModel.dirtyStatus || groupManager.interestsDirty)

    property bool smallResolution: width < 1920
    property bool openedWebBrowser: false

    // User that is being browsed in webview (after clicking on delegate in user list)
    property var currentUser: null

    property alias vkWebView: webViewItem.vkWebView

    property bool groupManagerInitialized: false
    property bool userManagerInitialized: false

    // True if filter drawer must be opened after switching from group window
    property bool openFilters: false

    onClosing: {
        close.accepted = closeAccepted || unsavedChangesDialog.save;
        if (!close.accepted) {
            unsavedChangesDialog.exit();
        }
    }

    onLoggedInChanged: {
        if (!loggedIn) {
            groupManagerInitialized = false;
            userManagerInitialized = false;
        }
    }

    visible: true
    minimumWidth: 1366
    visibility: ApplicationWindow.Maximized
    minimumHeight: 730
    title: qsTr("Barbatum VCognitis")
    color: "#EDEEF0"


    onCurrentViewChanged: {
        if (currentView === CFMainScreenView.Favorite) {
            userManager.favoriteModel.select();
        }
    }

    CFStartupPlaceholder {
        id: startupPlaceHolder

        anchors.fill: parent
        visible: !rootItem.visible
    }

    Item {
        id: rootItem

        anchors.fill: parent
        visible: !loggedIn || (userManagerInitialized && groupManagerInitialized)

        Item {
            width: parent.width
            height: parent.height - appHeader.height
            anchors.top: parent.top
            anchors.topMargin: appHeader.height

            Item {
                anchors.fill: parent

                visible: currentView === CFMainScreenView.Search || currentView === CFMainScreenView.Favorite

                CFProfileItem {
                    id: profileItem

                    currentView: appWindow.currentView
                    height: parent.height
                    width: appWindow.smallResolution ? appWindow.width : appWindow.width * 0.5
                }

                CFWebViewItem {
                    id: webViewItem

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.topMargin: smallResolution && loggedIn ? - CFStyle.mainHeaderHeight : 0
                    width: appWindow.smallResolution || !loggedIn ? parent.width : parent.width - profileItem.width
                    x: parent.width - width
                    opacity: !smallResolution || openedWebBrowser || !loggedIn ? 1 : 0
                    visible: opacity
                }
            }

            CFGroupItem {
                id: groupItem

                anchors.fill: parent
                visible: currentView === CFMainScreenView.Groups
                enabled: !userManager.score
            }
        }
        
        // Header should be above content(to proper shadows)
        CFMainHeader {
            id: appHeader

            visible: height && !(smallResolution && openedWebBrowser)
            height: loggedIn ? CFStyle.mainHeaderHeight : 0
            width: parent.width
        }

    }

    CFRoundButton {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: CFStyle.padding / 2
        icon.source: "qrc:/icons/Icon_info.svg"
        padding: 2
        width: 20
        height: width
        onClicked: aboutPopup.open();
        Material.background: enabled ? CFStyle.aboutButtonColor
                                     : CFStyle.setColorAlpha(CFStyle.aboutButtonColor, 0.5)
        Material.foreground: enabled ? CFStyle.white : CFStyle.white05
        enabled: rootItem.enabled
        visible: (currentView === CFMainScreenView.Search
                  || currentView === CFMainScreenView.Favorite)
                 && rootItem.visible && (smallResolution ? !openedWebBrowser : true)
    }

    CFGreater {
        visible: greater
        anchors.fill: parent

        onClicked: {
            greater = false
        }
    }

    CFStartPage {
        id: changeAppModePage

        visible: false
        anchors.fill: parent
    }

    Connections {
        target: groupManager

        onError: if (currentView !== CFMainScreenView.Groups) globalErrorToast.error(qsTr("При работе с группами возникла ошибка"), message)
        onInitialized: groupManagerInitialized = true;
    }

    Connections {
        target: userManager

        onScoringSuccessful: globalToast.show(qsTr("Фильтры успешно применены"))
        onError: globalErrorToast.error(qsTr("Загрузка подписчиков"), message)
        onInitialized: userManagerInitialized = true
    }

    Connections {
        target: vkAuthManager
        onAuthSuccess: if (utils.appMode === -1) {changeAppModePage.visible = true; groupManager.getGroupsForAnalysys(); }
        onLoggedOut: { greater = true; openedWebBrowser = false }
    }

    Connections {
        target: utils
        onAppModeChanged: userManager.select()
    }

    CFErrorPopup {
        id: globalErrorToast

        x: (parent.width - width) / 2
        y: parent.height - height - CFStyle.padding * 1.5
    }

    CFToastPopup {
        id: globalToast

        x: CFStyle.padding
        y: parent.height - height - CFStyle.padding
    }

    CFConfirmDialog {
        id: confirmDialog

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
    }

    CFFilterDrawer {
        id: filterDialog

        y: CFStyle.mainHeaderHeight
        Material.elevation: 0
    }


    CFCitySelectorPopup {
        id: citySelectorPopup

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2
    }

    CFDownloadedGroupSelectorPopup {
        id: downloadedGroupSelectorPopup

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2
    }

    CFInterestPopup {
        id: interestPopup

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2
    }

    CFUserGroupsDialog {
        id: userGroupsDialog

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2

        width: 840
        height: 720
    }

    CFTopAnalyzePopup {
        id: topAnalyzePopup

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2

        Connections {
            target: userManager.downloadQueue

            onFinished: {
                if (utils.appMode == CFAppMode.Quick) {
                    topAnalyzePopup.open()
                }
            }
        }
    }

    Shortcut {
       sequence: "Ctrl+B"
       onActivated: topAnalyzePopup.open()
   }

    CFNoConnectionPopup {
        id: noConnectionPopup

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2

        width: 500
        height: 300

        Connections {
            target: networkConnect
            onIsOnlineChanged: {
                if (networkConnect.isOnline) {
                    noConnectionPopup.close();
                } else {
                    noConnectionPopup.open();
                }
            }
        }
    }

    CFUnsavedDialog {
        id: unsavedChangesDialog

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2
    }

    CFAboutPopup {
        id: aboutPopup

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2
    }

    CFErrorLogPopup {
        id: errorLogPopup

        x: (appWindow.width - width) / 2
        y: (appWindow.height - height) / 2
    }

    Shortcut {
        sequence: "Ctrl+Shift+L"
        onActivated: errorLogPopup.open()
    }
}
