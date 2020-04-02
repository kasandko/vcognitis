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
import QtQuick.Controls.Material 2.12

import "../Singletons"
import "../SimpleControls"

Item {
    id: control

    property int weight: 1
    property bool desired: true

    property int buttonCount: 10

    property int hoveredIndex: CFConstants.invalidIndex

    signal updateWeight(int weight)

    implicitHeight: CFStyle.roundButtonSize
    implicitWidth: CFStyle.roundButtonSize * buttonCount + contentRow.spacing * (buttonCount - 1)

    Row {
        id: contentRow
        anchors.fill: parent

        spacing: -7

        Repeater {
            model: buttonCount

            delegate: CFRoundButton {
                property bool full: hoveredIndex === CFConstants.invalidIndex ? index < weight ? true : false : index <= hoveredIndex ? true : false

                Material.foreground: full ? desired ? CFStyle.red : CFStyle.blueMain : CFStyle.grey5

                icon.width: 24
                icon.height: 24
                icon.source: full ? "qrc:/icons/Icon_like_full.svg" : "qrc:/icons/Icon_like_empty.svg"
                flat: true

                onHoveredChanged: if (hovered) hoveredIndex = index; else hoveredIndex = CFConstants.invalidIndex

                onClicked: updateWeight(index + 1)

                CFToolTip {
                    y: - height

                    visible: parent.hovered
                    text:  (model.index + 1) + "/" + 10
                }
            }
        }
    }
}
