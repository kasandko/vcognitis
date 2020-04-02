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

import "../SimpleControls"
import "../Singletons"

Popup {
    id: control

    modal: true
    closePolicy: Popup.NoAutoClose

    CFColumn {
        anchors.centerIn: parent
        spacing: CFStyle.spacing

        CFImage {
            anchors.horizontalCenter: parent.horizontalCenter

            source: "qrc:/icons/Icon_wifioff.svg"
            sourceSize: Qt.size(96, 96)
        }

        CFLabel {
            text: qsTr("Отсутствует интернет-соединение")
            font.pixelSize: 18
            font.weight: Font.DemiBold
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CFLabel {
            text: qsTr("Проверьте сетевые кабели, модем и\nмаршрутизатор")
            color: CFStyle.black05
            horizontalAlignment: Text.AlignHCenter
            lineHeight: 20
            lineHeightMode: Text.FixedHeight
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
