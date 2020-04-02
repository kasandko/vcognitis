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
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import ConfederateFinder 1.0

import "../SimpleControls"
import "../Singletons"

Item {
    id: component

    property alias vkWebView: vkWebView

    function goToNextUser() {
        var model = userManager.model;
        if (profileItem.currentView == CFMainScreenView.Favorite)
            model = userManager.favoriteModel;

        var currentUserIndex = model.indexOf(currentUser);

        currentUserIndex++;

        if (currentUserIndex >= model.count)
            currentUserIndex = 0;

        profileItem.currentListView.positionViewAtIndex(currentUserIndex, ListView.Beginning)

        openedWebBrowser = true;

        var user =model.get(currentUserIndex);

        vkWebView.url = "https://vk.com/id" + user.vkId
        currentUser = user
    }

    Behavior on opacity {
        NumberAnimation {
            duration: 400
            easing.type: Easing.InOutQuad
        }
    }

    CFPane {
        padding: loggedIn && !smallResolution ? CFStyle.padding * 0.5 : 0
        topPadding: 0

        anchors {
            fill: parent
            margins: loggedIn && !smallResolution ? CFStyle.padding : 0
            leftMargin: 0
        }

        Material.elevation: 1

        ColumnLayout {
            id: browserLayout

            spacing: 0
            anchors.fill: parent

            Rectangle {
                id: browserHeaderSmall

                Layout.fillWidth: true
                Layout.preferredHeight: smallResolution ? 40 : 52

                color: smallResolution ? CFStyle.blueMain : "transparent"
                visible: loggedIn

                CFButton {
                    id: backToListButton

                    visible: smallResolution
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: CFStyle.padding
                    icon.source: "qrc:/icons/blue_arrow_back.svg"
                    icon.color: enabled ? CFStyle.white : CFStyle.white05
                    Material.foreground: icon.color
                    flat: true
                    text: qsTr("Вернуться к списку")
                    onClicked: openedWebBrowser = false;
                }

                CFButton {
                    id: nextUsersButton

                    contentItem: Row {
                        spacing: 5

                        CFLabel {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Следующий")
                            color: CFStyle.white
                        }

                        CFImage {
                            anchors.verticalCenter: parent.verticalCenter
                            source: "qrc:/icons/mdi_arrow_left.svg"
                            sourceSize: Qt.size(24, 24)
                        }
                    }

                    visible: smallResolution
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: backToListButton.right
                    anchors.leftMargin: CFStyle.padding
                    flat: true

                    onClicked: goToNextUser();
                }


                CFRoundButton {
                    id: goBackButton

                    visible: !smallResolution
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    icon.source: "qrc:/icons/blue_arrow_back.svg"
                    icon.color: enabled ? CFStyle.buttonColor : CFStyle.setColorAlpha(CFStyle.buttonColor, 0.5)
                    Material.foreground: icon.color
                    flat: true
                    onClicked: vkWebView.goBack()
                }

                CFLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: smallResolution ? backToListButton.right : goBackButton.right
                    anchors.right: buttonRow.left
                    anchors.leftMargin: CFStyle.padding * 0.5
                    anchors.rightMargin: anchors.leftMargin
                    text: qsTr("Просмотр профиля: ") + (currentUser ? (currentUser.firstName + " " + currentUser.lastName) : "")
                    elide: Text.ElideRight
                    font.pixelSize: smallResolution ? 16 : 18
                    font.weight: Font.DemiBold
                    color: smallResolution ? CFStyle.white : CFStyle.black08
                    horizontalAlignment: smallResolution ? Text.AlignHCenter : Text.AlignLeft
                }

                CFRow {
                    id: buttonRow

                    leftPadding: CFStyle.padding * (smallResolution ? 1 : 0.5)
                    rightPadding: leftPadding
                    spacing: CFStyle.spacing * 0.75
                    anchors.right: parent.right
                    height: parent.height

                    CFButton {
                        id: favoriteBrowserButton

                        anchors.verticalCenter: parent.verticalCenter

                        visible: currentUser
                        icon.source: currentUser && currentUser.favorite ? "qrc:/icons/Icon_headerstar.svg" : "qrc:/icons/Icon_star.svg"
                        icon.color: smallResolution ? (enabled ? CFStyle.white : CFStyle.white05)
                                                    : (enabled ? CFStyle.buttonColor : CFStyle.setColorAlpha(CFStyle.buttonColor, 0.5))

                        text: qsTr("Избранное")
                        Material.foreground: smallResolution ? (enabled ? CFStyle.white : CFStyle.white05)
                                                             : (enabled ? CFStyle.buttonColor : CFStyle.setColorAlpha(CFStyle.buttonColor, 0.5))
                        flat: true
                        onClicked: currentUser ? currentUser.favorite = !currentUser.favorite : null;
                    }

                    CFVDivider {
                        height: parent.height
                        visible: currentUser
                        color: smallResolution ? CFStyle.setColorAlpha(CFStyle.white, 0.15) : CFStyle.grey
                    }

                    CFButton {
                        enabled: !(userManager.score || userManager.downloadQueue.count || userManager.ageSearch || userManager.rescoringActive)
                        anchors.verticalCenter: parent.verticalCenter

                        icon.source: "qrc:/icons/Icon_logout.svg"
                        icon.color: smallResolution ? (enabled ? CFStyle.white : CFStyle.white05)
                                                    : (enabled ? CFStyle.buttonColor : CFStyle.setColorAlpha(CFStyle.buttonColor, 0.5))
                        text: qsTr("Выйти")
                        Material.foreground: smallResolution ? (enabled ? CFStyle.white : CFStyle.white05)
                                                             : (enabled ? CFStyle.buttonColor : CFStyle.setColorAlpha(CFStyle.buttonColor, 0.5))
                        flat: true
                        onClicked: vkWebView.runJavaScript('document.getElementById("top_logout_link").click()')
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 2
                color: smallResolution ? CFStyle.colorPrimary : "transparent"

                Rectangle {
                    width: parent.width
                    height: 1
                    color: CFStyle.setColorAlpha(CFStyle.white, 0.15)
                    visible: smallResolution
                }

                Rectangle {
                    color: smallResolution ? CFStyle.blueGray : CFStyle.buttonColor
                    height: parent.height
                    width: parent.width * vkWebView.loadProgress / 100
                    visible: vkWebView.loadProgress < 98
                    Behavior on width {
                        NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                    }
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                CFVKWebView {
                    id: vkWebView

                    anchors.fill: parent
                    zoomFactor: appWindow.smallResolution ? 1 : 0.9

                    CFButton {
                        anchors {
                            top: parent.top
                            left: parent.left
                            leftMargin: CFStyle.padding
                            topMargin: CFStyle.padding / 5
                        }

                        z: 9999
                        visible: loggedIn && smallResolution
                        flat: vkWebView.isVk
                        icon.source: "qrc:/icons/arrow_left_white.svg";
                        text: qsTr("Назад")
                        Material.foreground: CFStyle.white
                        onClicked: vkWebView.goBack();
                    }

                    Connections {
                        target: appWindow

                        onSmallResolutionChanged: vkWebView.zoomFactor = appWindow.smallResolution ? 1 :  0.9
                    }
                }

                FastBlur {
                    anchors.fill: vkWebView
                    source: vkWebView
                    radius: 80
                    visible: vkWebView.loadProgress < 98

                    MouseArea {
                        anchors.fill: parent
                    }
                }
            }
        }
    }
}
