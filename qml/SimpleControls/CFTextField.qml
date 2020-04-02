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

import "qrc:/qml/Singletons"

TextField {
    id: control

    property bool error: false
    property string iconSource: ""
    property size iconSize: Qt.size(0, 0)
    property bool withContextMenu: false
    property bool clearButtonEnabled: false

    signal clearClicked()

    topPadding: 0
    bottomPadding: 0
    leftPadding: iconSource == "" ? CFStyle.padding * 0.5 : iconSize.width + CFStyle.padding * 0.75
    rightPadding: leftPadding
    selectByMouse: true
    placeholderTextColor: CFStyle.black03
    selectedTextColor: CFStyle.white
    selectionColor: CFStyle.blue08

    MouseArea {
        enabled: withContextMenu && !control.readOnly && control.enabled

        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.RightButton)
                contextMenu.popup()
            else
                mouse.accepted = false;
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        CFMenu {
            id: contextMenu

            Action { text: qsTr("Вставить"); onTriggered: control.paste()}
        }
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: CFStyle.controlHeight
        color: CFStyle.backgroundPrimay
        radius: 2

        CFImage {
            anchors.left: parent.left
            anchors.leftMargin: CFStyle.padding / 2
            anchors.verticalCenter: parent.verticalCenter
            source: control.iconSource
            sourceSize: iconSize
        }

        Rectangle {
            id: clearButtonRect

            anchors {
                right: parent.right
                rightMargin: CFStyle.padding / 4
                verticalCenter: parent.verticalCenter
            }

            visible: control.clearButtonEnabled && control.text != ""
            height: 20
            width: height
            radius: width / 2
            color: CFStyle.blueGray
            opacity: 0.5
        }

        CFImage {
            anchors.centerIn: clearButtonRect //avoid opacity hack
            source: "qrc:/icons/clear10px.svg"
            sourceSize: Qt.size(10, 10)
            visible: clearButtonRect.visible

            MouseArea {
                anchors.centerIn: parent
                height: clearButtonRect.height
                width: clearButtonRect.width
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    control.text = "";
                    control.clearClicked()
                }
            }
        }

        border {
            width: 1
            color: error ? CFStyle.red : control.activeFocus ? CFStyle.colorPrimary : CFStyle.backgroundSecondary
        }
    }
}
