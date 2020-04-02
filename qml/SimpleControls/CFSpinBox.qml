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

SpinBox {
    id: control

    height: CFStyle.controlHeight
    width: 100

    font.pixelSize: 14

    opacity: enabled ? 1 : 0.5
    topPadding: 0
    bottomPadding: 0
    spacing: 0
    leftInset: 0
    rightInset: 0

    contentItem: TextInput {
        text: control.displayText

        font: control.font
        color: CFStyle.black08
        selectionColor: CFStyle.blue08
        selectedTextColor: CFStyle.white
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        cursorDelegate: CursorDelegate { }

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
    }

    background: Rectangle {
        color: CFStyle.white
        radius: 2
        border.width: 1
        border.color: CFStyle.blueLight
        implicitWidth: 96
        implicitHeight: CFStyle.controlHeight
    }

    up.indicator: Item {
        opacity: up.hovered ? up.pressed ? 0.5 : 0.7 : 1
        x: control.mirrored ? 0 : parent.width - width
        implicitWidth: implicitHeight
        implicitHeight: CFStyle.controlHeight
        height: parent.height
        width: height

        CFImage {
            anchors.centerIn: parent
            source: "qrc:/icons/Arrow_left.svg"
            rotation: control.mirrored ? 0 : 180
            sourceSize: Qt.size(24, 24)
        }
    }

    down.indicator: Item {
        opacity: down.hovered ? down.pressed ? 0.5 : 0.7 : 1
        x: control.mirrored ? parent.width - width : 0
        implicitWidth: implicitHeight
        implicitHeight: CFStyle.controlHeight
        height: parent.height
        width: height

        CFImage {
            anchors.centerIn: parent
            source: "qrc:/icons/Arrow_left.svg"
            rotation: control.mirrored ? 180 : 0
            sourceSize: Qt.size(24, 24)
        }
    }
}
