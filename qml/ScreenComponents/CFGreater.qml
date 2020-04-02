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

import "../Singletons"
import "../SimpleControls"
import "../SpecialControls"

Rectangle {
    id: control

    signal clicked();

    color: CFStyle.grey

    CFImage {
        source: "qrc:/images/greaterBackground.jpg"
        anchors.centerIn: parent
        sourceSize: Qt.size(1920, 1309)
        opacity: 0.25
    }

    MouseArea {
        anchors.fill: parent
    }

    CFClickableLogo {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: CFStyle.spacing
        }
    }

    CFColumn {
        anchors.centerIn: parent

        spacing: 25

        CFImage {
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc:/images/greaterCircleImage.png"
            sourceSize: Qt.size(300, 300)
        }

        Item {
            height: 1
            width: 1
        }

        CFLabel {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Добро пожаловать в VCognitis")
            color: CFStyle.black
            font.pixelSize: 48
        }

        CFLabel {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Здесь вы можете найти единомышленников или новые знакомства")
            color: CFStyle.black
            font.pixelSize: 18
        }

        Item {
            height: 1
            width: 1
        }

        CFButton {
            anchors.horizontalCenter: parent.horizontalCenter
            icon.source: "qrc:/icons/vkLogo.svg"
            icon.height: 24
            icon.width: 24
            text: qsTr("Войти через Вконтакте")

            onClicked: control.clicked()
        }
    }
}
