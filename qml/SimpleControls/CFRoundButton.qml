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

import "../Singletons"

RoundButton {
    id: control

    topInset: 0
    bottomInset: 0
    rightInset: 0
    leftInset: 0

    padding: 8

    icon.color: !enabled ? CFStyle.setColorAlpha(Material.foreground, 0.5) :
            flat && highlighted ? Material.accentColor :
            highlighted ? Material.primaryHighlightedTextColor : Material.foreground

    background: Rectangle {
        implicitWidth: control.Material.buttonHeight
        implicitHeight: control.Material.buttonHeight

        radius: control.radius
        color: !control.enabled ? flat ? "transparent" : control.Material.buttonDisabledColor
            : control.checked || control.highlighted ? control.Material.highlightedButtonColor : control.Material.buttonColor

        Rectangle {
            width: parent.width
            height: parent.height
            radius: control.radius
            visible: enabled && (control.hovered || control.visualFocus)
            color: control.Material.rippleColor
        }

        Rectangle {
            width: parent.width
            height: parent.height
            radius: control.radius
            visible: enabled && control.down
            color: control.Material.rippleColor
        }

        // The layer is disabled when the button color is transparent so that you can do
        // Material.background: "transparent" and get a proper flat button without needing
        // to set Material.elevation as well
        layer.enabled: control.enabled && control.Material.buttonColor.a > 0
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }
    }
}
