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

import "../Singletons"
import "../SimpleControls"

Rectangle {
    id: control

    property string text: ""
    property color textColor: CFStyle.blueMain
    property string source: "qrc:/icons/close14.svg"
    property size sourceSize: Qt.size(14,14)

    signal iconClicked();
    signal clicked()

    color: CFStyle.blueLight
    radius: 2
    width: contentRow.implicitWidth + 16
    height: CFStyle.controlHeight

    MouseArea {
        anchors.fill: parent
        onClicked: control.clicked()
    }

    CFRow {
        id: contentRow

        spacing: 5
        anchors.centerIn: parent

        CFLabel {
            font.pixelSize: 12
            anchors.verticalCenter: parent.verticalCenter
            color: control.textColor
            text: control.text
        }

        CFImage {
            anchors.verticalCenter: parent.verticalCenter
            source: control.source
            sourceSize: control.sourceSize

            MouseArea {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                cursorShape: Qt.PointingHandCursor

                onClicked: control.iconClicked()
            }
        }
    }
}
