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

    color: CFStyle.grey

    CFImage {
        source: "qrc:/images/greaterBackground.jpg"
        anchors.centerIn: parent
        sourceSize: Qt.size(1920, 1309)
        opacity: 0.25
    }

    CFClickableLogo {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: CFStyle.spacing
        }
    }

    CFPane {
        anchors.centerIn: parent

        width: 324
        height: 216

        CFColumn {
            anchors.centerIn: parent

            spacing: CFStyle.spacing

            CFImage {
                source: "qrc:/icons/Icon_timelapse_96.svg"
                sourceSize: Qt.size(96, 96)

                anchors.horizontalCenter: parent.horizontalCenter
            }

            CFLabel {
                text: qsTr("Пожалуйста, подождите")
                font.weight: Font.DemiBold
                font.pixelSize: 18
            }

            CFLabel {
                text: qsTr("Идёт загрузка данных из базы")
                color: CFStyle.black05
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
