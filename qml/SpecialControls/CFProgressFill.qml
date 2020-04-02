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
import QtGraphicalEffects 1.0

import "../Singletons"

Rectangle {
    id: control

    property double progress: 0
    property bool indeterminate: false

    clip: true
    height: CFStyle.controlHeight

    Image {
        id: imageOriginal

        width: control.indeterminate ? 178 * 5 : Math.floor(parent.width * control.progress)
        height: parent.height
        source: "qrc:/icons/Pattern_download.svg"
        visible: false

        fillMode: Image.TileHorizontally

        SequentialAnimation on width {
            running: indeterminate
            loops: Animation.Infinite
            PropertyAnimation { to: 178 * 10; duration: 9000 }
            PropertyAnimation { to: 178 * 5; duration: 0 }
        }
    }

    Rectangle {
        id: rectangleMask

        anchors.fill: imageOriginal
        radius: control.radius
        visible: false
    }

    OpacityMask {
        id: opacityMask

        anchors.fill: imageOriginal
        source: imageOriginal
        maskSource: rectangleMask
    }
}
