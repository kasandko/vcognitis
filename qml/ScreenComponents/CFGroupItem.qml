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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0
import ConfederateFinder 1.0

import "../SimpleControls"
import "../Singletons"
import "../SpecialControls"
import "../Popups"

Item {

    property var interestSelector: smallResolution ? interestSelector2 : interestSelector1

    function resetSelectors() {
        interestSelector1.reset(groupManager.interests);
        interestSelector2.reset(groupManager.interests);
    }

    // I think it is not a best solution to sync interestSelectors, but it works good
    ListModel {
        id: interestSelectionListModel

        onCountChanged: {
            interestSelector1.updateShortIndex();
            interestSelector2.updateShortIndex();
        }
    }

    CFColumn {
        visible: !smallResolution
        anchors.fill: parent
        padding: CFStyle.padding
        spacing: CFStyle.padding

        CFInterestSelector {
            id: interestSelector1

            Material.elevation: 2
            width: parent.width - parent.padding * 2
            height: 70

            Component.onCompleted: reset(groupManager.interests);

            enabled: !userManager.downloadQueue.count
        }

        CFGroupManager {
            enableStatus: !smallResolution
            Material.elevation: 2
            width: parent.width - parent.padding * 2

            height: parent.height - (interestSelector1.visible ? 3 : 2) * CFStyle.padding - (interestSelector1.visible ? interestSelector1.height : 0)
            leftPadding: 0
            rightPadding: 0
        }
    }

    CFRow {
        visible: smallResolution

        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        padding: CFStyle.padding
        spacing: CFStyle.padding

        CFInterestSelector {
            id: interestSelector2

            Material.elevation: 2
            width: 364
            height: parent.height - 2 * CFStyle.padding

            enabled: !userManager.downloadQueue.count
        }

        CFGroupManager {
            enableStatus: smallResolution
            headLess: true
            Material.elevation: 2
            width: appWindow.minimumWidth - parent.padding * 3 - interestSelector2.width
            height: parent.height - 2 * CFStyle.padding

            leftPadding: 0
            rightPadding: 0
        }
    }

    Popup {
        id: teachPopup

        x: smallResolution ? (parent.width / 2  - 225) : (CFStyle.padding + 55)
        y: smallResolution ? (CFStyle.padding + 121) : (CFStyle.padding * 2 + interestSelector1.height + 121)

        closePolicy: Popup.NoAutoClose
        exit: null
        enter: null
        rightPadding: 24

        onActiveFocusChanged: if (!activeFocus) close();

        background: Rectangle {
            color: CFStyle.blueTeachToolTip
            radius: 8

            CFImage {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: CFStyle.padding / 2
                source: "qrc:/icons/close14White.svg"
                sourceSize: Qt.size(14, 14)

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -1
                    onClicked: teachPopup.close()
                    cursorShape: Qt.PointingHandCursor
                }
            }

            Rectangle {
                id: shadowRect

                color: CFStyle.blueTeachToolTip
                radius: 8
                height: 10
                width: parent.width - 4
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
            }

            DropShadow {
                anchors.fill: shadowRect
                verticalOffset: 10
                radius: 10
                samples: 17
                color: CFStyle.setColorAlpha("#000000", 0.14)
                source: shadowRect
            }
        }

        contentItem: CFLabel {
            color: CFStyle.white
            text: qsTr("Нажмите сохранить и после загрузите нужные\nгруппы для начала поиска единомышленников")
        }

        Connections {
            target: groupManager

            onUserGroupsRefreshed: {
                if (currentView === CFMainScreenView.Groups
                        && userManager.model.count === 0) {

                    teachPopup.open();
                    teachPopup.forceActiveFocus();
                    openFilters = true;
                }
            }
        }
    }
}
