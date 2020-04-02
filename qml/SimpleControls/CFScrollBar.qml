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
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

import "qrc:/qml/Singletons"

ScrollBar {
    id: control

    property bool targetContainsMouse: false

    padding: 0
    active: targetContainsMouse
    onActiveChanged: active = Qt.binding(() => targetContainsMouse)

    contentItem: Item {
        implicitWidth: control.interactive ? 8 : 6
        implicitHeight: control.interactive ? 8 : 6

        opacity: 0.0

        Rectangle {
            anchors.right: parent.right
            height: parent.height
            width: control.interactive ? (control.hovered || control.pressed) ? 8 : 6 : 6
            radius: 3
            opacity: {
                control.pressed ? 0.5 : control.hovered ? 0.35 : 0.2
            }

            color: CFStyle.black

            Behavior on width {
                NumberAnimation  {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }

            Behavior on opacity {
                NumberAnimation  {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    background: Item {
        opacity: 0.0
        visible: control.interactive

        implicitWidth: control.interactive ? 8 : 6
        implicitHeight: control.interactive ? 8 : 6
    }
}
