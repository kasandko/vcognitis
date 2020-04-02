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

import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import ConfederateFinder 1.0

import "../SimpleControls"
import "../Singletons"

Item {
    id: control

    function showProfile() {
        openedWebBrowser = true;
        currentUser = qtObject
        vkWebView.url = "https://vk.com/id" + model.vkId;
    }

    function showMessageDialog() {
        openedWebBrowser = true;
        currentUser = qtObject
        vkWebView.url = "https://vk.com/im?sel=" + model.vkId;
    }

    implicitHeight: pane.implicitHeight
    implicitWidth: pane.implicitWidth

    CFPane {
        id: pane

        padding: 0
        anchors.fill: parent

        Material.elevation: currentWebViwevUserBorder.visible ? 5 : 1

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                MouseArea {
                    anchors.fill: parent

                    cursorShape: Qt.PointingHandCursor
                    onClicked: showProfile()
                }

                Rectangle {
                    anchors.fill: avatarImg
                    anchors.margins: model.favorite ? -4 : 1
                    color: CFStyle.blueMain
                    visible: anchors.margins <= 0
                    radius: width / 2

                    Behavior on anchors.margins {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.InOutQuad
                        }
                    }
                }

                CFImage {
                    id: avatarImg

                    rounded: true
                    adapt: true

                    source: model.photoMax

                    width: height

                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                        margins: CFStyle.padding
                    }
                }

                Rectangle {
                    width: 40
                    height: width
                    radius: width / 2
                    color: CFStyle.blueMain

                    anchors {
                        left: avatarImg.left
                        top: avatarImg.top
                    }

                    CFLabel {
                        anchors.centerIn: parent

                        color: CFStyle.white
                        text: groupScore + interestScore
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                    }

                    MouseArea {
                        hoverEnabled: true
                        anchors.fill: parent
                        propagateComposedEvents: true

                        onClicked: {
                            mouse.accepted = false;
                        }

                        CFToolTip {
                            x: 0
                            visible: parent.containsMouse
                            text: qsTr("Уровень единства")
                        }
                    }


                }

                CFLabel {
                    id: nameLabel

                    text: model.firstName + " " + model.lastName
                    color: CFStyle.black08
                    elide: Text.ElideRight

                    anchors {
                        left: avatarImg.right
                        right: parent.right
                        top: avatarImg.top
                        leftMargin: CFStyle.padding
                        rightMargin: leftMargin
                    }

                    font.pixelSize: 18
                }

                CFRow {
                    id: ageRow

                    spacing: CFStyle.spacing * 0.5

                    anchors {
                        left: avatarImg.right
                        top: nameLabel.bottom
                        leftMargin: CFStyle.padding
                        topMargin: CFStyle.padding * 0.4
                    }

                    CFPersonalIcon {
                        visible: true
                        personalList: userManager.model.relation
                        value: model.relation
                        icon.source: "qrc:/icons/Icon_age.svg"
                        toolTipText: qsTr("Семейное положение: ") + filterDialog.filterModels.relation(relation)
                    }

                    CFLabel {

                        text: {
//                            if (userManager.currentAgeSpecifyVkId == vkId)
//                                return qsTr("Уточняем...")

                            if (age === CFConstants.invalidValue)
                                return qsTr("Возраст скрыт")

                            if (inaccurateAge)
                                return qsTr("Примерно %n (лет)", "", age)

                            return qsTr("%n (лет)", "", age)
                        }
                        color: {
//                            if (userManager.ageSpecifier)
//                                return CFStyle.black05 // means age specifier currently is busy

//                            if (inaccurateAge || age === CFConstants.invalidValue)
//                                return Material.primary

                            return CFStyle.black05
                        }

                        anchors.verticalCenter: parent.verticalCenter

                        font.pixelSize: 14

//                        MouseArea {
//                            enabled: !userManager.ageSpecifier && (inaccurateAge || age === CFConstants.invalidValue)
//                            anchors.fill: parent
//                            anchors.margins: -1

//                            hoverEnabled: true
//                            onClicked: {
//                                userManager.specifyAge(model.qtObject)
//                            }

//                            CFToolTip {
//                                visible: parent.containsMouse && parent.enabled && inaccurateAge
//                                text: qsTr("Уточнить возраст")
//                            }
//                        }
                    }
                }

                CFRow {
                    id: personalIconsRow
                    anchors.top: ageRow.bottom
                    anchors.topMargin: CFStyle.padding * 0.5
                    anchors.left: avatarImg.right
                    anchors.leftMargin: CFStyle.padding
                    spacing: CFStyle.padding * 0.5

                    CFPersonalIcon {
                        personalList: userManager.model.political
                        value: model.political
                        icon.source: "qrc:/icons/Icon_political.svg"
                        toolTipText: qsTr("Политические взгляды: ") + filterDialog.filterModels.political(political)
                    }

                    CFPersonalIcon {
                        personalList: userManager.model.religion
                        value: religion
                        icon.source: "qrc:/icons/Icon_religion.svg"
                        toolTipText: qsTr("Мировоззрение: ") + filterDialog.filterModels.religion(religion)
                    }

                    CFPersonalIcon {
                        personalList: userManager.model.lifeMain
                        value: lifeMain
                        icon.source: "qrc:/icons/Icon_mainInLife.svg"
                        toolTipText: qsTr("Главное в жизни: ") + filterDialog.filterModels.lifeMain(lifeMain)
                    }

                    CFPersonalIcon {
                        personalList: userManager.model.peopleMain
                        value: peopleMain
                        icon.source: "qrc:/icons/Icon_mainInOthers.svg"
                        toolTipText: qsTr("Главное в людях: ") + filterDialog.filterModels.peopleMain(peopleMain)
                    }

                    CFPersonalIcon {
                        personalList: userManager.model.smoking
                        value: smoking
                        icon.source: "qrc:/icons/Icon_smoking.svg"
                        toolTipText: qsTr("Отношение к курению: ") + filterDialog.filterModels.smoking(smoking)
                    }

                    CFPersonalIcon {
                        personalList: userManager.model.alcohol
                        value: alcohol
                        icon.source: "qrc:/icons/Icon_alcohol.svg"
                        toolTipText: qsTr("Отношение к алкоголю: ") + filterDialog.filterModels.alcohol(alcohol)
                    }
                }

                Row {
                    anchors {
                        top: personalIconsRow.bottom
                        topMargin: CFStyle.padding * 0.75
                        left: avatarImg.right
                        leftMargin: CFStyle.padding
                    }

                    spacing: CFStyle.padding * 0.75

                    CFButton {
                        id: favoriteButton

                        icon.source: "qrc:/icons/Icon_star.svg"
                        text: qsTr("Избранное")

                        Material.background: model.favorite ? CFStyle.blueLight : CFStyle.buttonColor
                        Material.foreground: model.favorite ? Material.primary : "white"

                        onClicked: {
                            model.favorite = !model.favorite;
                        }
                    }

                    CFButton {
                        display: AbstractButton.IconOnly
                        icon.source: "qrc:/icons/Icon_mail.svg"
                        width: 44
                        enabled: model.openMessages

                        onClicked: showMessageDialog()
                    }
                }
            }

            CFVDivider {
                Layout.fillHeight: true
            }

            CFUserGroupsList {
                Layout.fillHeight: true
                Layout.preferredWidth: 220
            }

            CFVDivider {
                Layout.fillHeight: true
            }

            CFUserInterestsList {
                model: interests

                Layout.fillHeight: true
                Layout.preferredWidth: 220
            }
        }

        Rectangle {
            id: currentWebViwevUserBorder

            anchors.fill: parent
            radius: 2
            color: "transparent"
            border.width: 1
            border.color: CFStyle.buttonColor
            visible: model.qtObject === currentUser
        }
    }
}
