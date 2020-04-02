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
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.12

import "../Singletons"

Menu {
    id: control

    Material.elevation: 2

    padding: 0

    delegate: MenuItem {
        id: menuItem

        focusPolicy: Qt.NoFocus
        implicitWidth: 100
        implicitHeight: CFStyle.controlHeight

        contentItem: CFLabel {
            text: menuItem.text
            font: menuItem.font
            opacity: enabled ? 1.0 : 0.3
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            implicitWidth: 100
            implicitHeight: 40
            opacity: enabled ? 1 : 0.3
            color: menuItem.highlighted ? CFStyle.blueLight : "transparent"
        }
    }

    background: Rectangle {
        implicitWidth: 150
        implicitHeight: CFStyle.controlHeight
        color: CFStyle.white
        radius: 2

        layer.enabled: control.enabled
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }
    }
}
