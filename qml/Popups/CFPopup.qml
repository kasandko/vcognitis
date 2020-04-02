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
import QtGraphicalEffects 1.12

import "../Singletons"
import "../SimpleControls"

Popup {
    id: popup

    property string title: ""

    topPadding: CFStyle.padding + 70
    padding: CFStyle.padding

    background: Rectangle {
        anchors.fill: parent
        color:  CFStyle.white
        radius: 2

        CFLabel {
            id: titleLabel

            font.pixelSize: 18
            text: title
            verticalAlignment: CFLabel.AlignVCenter
            font.weight: Font.DemiBold

            anchors.left: parent.left
            anchors.leftMargin: CFStyle.padding
            anchors.top: parent.top
            anchors.bottom: horizontalDivider.top
        }

        CFImage {
            anchors.right: parent.right
            anchors.verticalCenter: titleLabel.verticalCenter
            anchors.margins: CFStyle.padding
            sourceSize: Qt.size(24,24)
            source: "qrc:/icons/closePopup.svg"

            MouseArea {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                onClicked: popup.close()
                cursorShape: Qt.PointingHandCursor
            }
        }

        CFHDivider {
            id: horizontalDivider

            width: parent.width
            anchors.top: parent.top
            anchors.margins: 70
        }
    }
}
