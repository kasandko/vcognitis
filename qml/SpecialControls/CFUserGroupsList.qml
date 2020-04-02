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
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

import "qrc:/qml/SimpleControls"
import "qrc:/qml/Singletons"

Item {
    id : control

    clip: true

    CFListView {
        id: list

        spacing: CFStyle.spacing * 0.5

        model: positiveGroups
        interactive: false

        anchors {
            fill: parent
            margins: CFStyle.padding * 0.75
        }

        delegate: Item {
            implicitWidth: list.width
            implicitHeight: 20

            Rectangle {
                id: rec

                width: 4
                radius: 2
                height: width
                color: CFStyle.green

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
            }

            CFLabel {
                id: label

                color: CFStyle.colorPrimary
                elide: Text.ElideRight
                text: name

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: rec.right
                    leftMargin: CFStyle.spacing * 0.5
                    right: parent.right
                }
            }
        }
    }

    CFRow {
        id: dotRow

        property color color: {
            if (negativeGroups.rowCount() > 0 && positiveGroups.rowCount() > 5) {
                return CFStyle.colorPrimary;
            }

            if (positiveGroups.rowCount() <= 5 && negativeGroups.rowCount() > 0) {
                return CFStyle.red;
            }

            return CFStyle.green;
        }

        visible: negativeGroups.rowCount() > 0 || positiveGroups.rowCount() > 5
        spacing: CFStyle.spacing * 0.25

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: CFStyle.padding * 0.4

        Repeater {
            model: 3

            Rectangle {
                width: 4
                height: width
                radius: width / 2
                color: dotRow.color
            }
        }
    }

    CFColumn {
        anchors.centerIn: parent
        visible: list.count === 0
        spacing: CFStyle.spacing  * 0.75

        CFImage {
            source: "qrc:/icons/Icon_sub_48.svg"
            sourceSize: Qt.size(48, 48)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CFLabel {
            text: qsTr("Нет схожих подписок")
            color: CFStyle.black05
        }
    }

    Ripple {
        width: parent.width
        height: parent.height
        pressed: mouseArea.pressed
        anchor: control
        active: mouseArea.containsPress
        color: control.Material.rippleColor
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: userGroupsDialog.initAndOpen(firstName + " " + lastName, positiveGroups, negativeGroups)
    }
}
