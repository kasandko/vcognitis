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

import "../SimpleControls"
import "../Singletons"

Dialog {
    id: control

    property string rejectText: qsTr("Отмена")
    property string acceptText: qsTr("ОК")

    modal: true

    topPadding: 0
    padding: 0

    header: Item {
        implicitHeight: headerLabel.implicitHeight + CFStyle.padding * 1.5
        implicitWidth: headerLabel.implicitWidth + CFStyle.spacing + CFStyle.padding * 2 + 20

        CFLabel {
            id: headerLabel

            font.pixelSize: 18
            text: title
            color: CFStyle.black08
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: CFStyle.padding
            anchors.left: parent.left
        }

        CFRoundButton {
            icon.source: "qrc:/icons/closeDelegate.svg"

            Material.foreground: CFStyle.black05
            flat: true

            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: CFStyle.padding * 0.6
            anchors.right: parent.right

            onClicked: reject()
        }
    }

    footer: Item {
        implicitHeight: footerRow.implicitHeight + CFStyle.padding * 2
        implicitWidth: rejectButton.width + acceptButton.width + CFStyle.spacing + CFStyle.padding * 2

        CFRow {
            id: footerRow

            spacing: CFStyle.spacing
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: CFStyle.padding

            CFButton {
                id: rejectButton

                text: rejectText
                width: 80
                Material.background: CFStyle.blueLight
                Material.foreground: CFStyle.blue08
                flat: true
                onClicked: reject()
            }

            CFButton {
                id: acceptButton

                text: acceptText
                width: 150
                onClicked: accept()
            }
        }
    }
}
