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

import "../SimpleControls"
import "../Singletons"

Item {
    id : control

    property var model

    CFListView {
        id: list

        spacing: CFStyle.spacing * 0.5 + 1

        model: control.model
        interactive: false

        anchors {
            fill: parent
            margins: CFStyle.padding * 0.75
        }

        delegate: Item {
            implicitWidth: list.width
            implicitHeight: 20

            Rectangle {
                id: pointRectangle

                width: 4
                radius: 2
                height: width
                color: CFStyle.blueMain

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
            }

            CFLabel {
                id: label

                color: CFStyle.blueMain
                elide: Text.ElideRight
                text: model.name

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: pointRectangle.right
                    leftMargin: CFStyle.spacing * 0.5
                    right: parent.right
                }
            }
        }
    }

    CFColumn {
        anchors.centerIn: parent
        visible: list.count === 0
        spacing: CFStyle.spacing * 0.75

        CFImage {
            source: "qrc:/icons/Icon_accurateMode48.svg"
            sourceSize: Qt.size(48, 48)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CFLabel {
            text: qsTr("Нет схожих интересов")
            color: CFStyle.black05
        }
    }
}
