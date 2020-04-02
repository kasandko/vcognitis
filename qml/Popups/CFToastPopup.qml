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
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

import "../Singletons"
import "../SimpleControls"

Popup {
    id: control

    property int autoHideInterval: 5000

    function show(message) {
        autoHideTimer.start();
        messageLabel.text = message;

        control.open();
    }


    padding: CFStyle.padding / 2
    rightPadding: CFStyle.padding * 1.75

    Material.elevation: 1

    background: Rectangle {
        radius: 4* 1* 1
        color: CFStyle.black08

        layer.enabled: control.Material.elevation > 0
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }


        CFImage {
            source: "qrc:/icons/close14White.svg"
            sourceSize: Qt.size(14, 14)
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: CFStyle.padding / 2

            MouseArea {
                anchors.fill: parent
                anchors.margins: -1

                onClicked: {
                    autoHideTimer.stop();
                    control.close();
                }
            }
        }
    }

    contentItem: CFLabel {
        id: messageLabel

        font.pixelSize: 18
        color: CFStyle.white
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            autoHideTimer.stop();
            control.close();
        }
    }

    Timer {
        id: autoHideTimer

        repeat: false
        interval: autoHideInterval

        onTriggered: control.close()
    }
}
