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
import QtGraphicalEffects 1.0

Image {
    id: img

    property bool rounded: false
    property bool adapt: false

    height: sourceSize.height
    width: sourceSize.width

    layer.enabled: rounded
    layer.effect: OpacityMask {
        maskSource: Item {
            width: img.width
            height: img.height
            Rectangle {
                anchors.centerIn: parent
                width: img.adapt ? img.width : Math.min(img.width, img.height)
                height: img.adapt ? img.height : width
                radius: Math.min(width, height)
            }
        }
    }
}
