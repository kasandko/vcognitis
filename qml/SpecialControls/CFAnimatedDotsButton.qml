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
import "../SimpleControls"

/* This button does not change color when disabled but instead shows animated dots instead of text */
CFButton {
    id: control

    property bool animation: false
    property bool flicker: false

    contentItem: Item {
        implicitHeight: iconLabel.implicitHeight
        implicitWidth: iconLabel.implicitWidth

        IconLabel {
            id: iconLabel

            anchors.fill: parent
            visible: !animation

            spacing: control.spacing
            mirrored: control.mirrored
            display: control.display

            icon: control.icon
            text: control.text
            font: control.font
            color: !control.enabled ? CFStyle.setColorAlpha(control.Material.foreground, 0.5) :
                control.flat && control.highlighted ? control.Material.accentColor :
                control.highlighted ? control.Material.primaryHighlightedTextColor : control.Material.foreground
        }

        CFAnimatedDots {
            anchors.centerIn: parent
            visible: animation
        }
    }

    background: Rectangle {
        implicitWidth: 64
        implicitHeight: CFStyle.controlHeight

        radius: control.radius
        color: control.highlighted ? control.highlightColor :
                                     (!control.enabled && !control.animation) ?
                                         CFStyle.setColorAlpha(control.Material.buttonColor, 0.5) : control.Material.buttonColor

        SequentialAnimation on color {
            running: flicker && !animation
            loops: Animation.Infinite

            ColorAnimation { to: CFStyle.buttonColorSaturated; duration: 700 }
            ColorAnimation { to: CFStyle.buttonColor; duration: 700 }
        }

        PaddedRectangle {
            y: parent.height - 4
            width: parent.width
            height: 4
            radius: control.radius
            topPadding: -2
            clip: true
            visible: control.checkable && (!control.highlighted || control.flat)
            color: control.checked && control.enabled ? control.Material.accentColor : control.Material.secondaryTextColor
        }

        // The layer is disabled when the button color is transparent so you can do
        // Material.background: "transparent" and get a proper flat button without needing
        // to set Material.elevation as well
        layer.enabled: control.enabled && control.Material.buttonColor.a > 0 && !control.flat
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }

        Ripple {
            clipRadius: control.radius
            width: parent.width
            height: parent.height
            pressed: control.pressed
            anchor: control
            active: control.down || control.visualFocus || control.hovered
            color: control.flat && control.highlighted ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }
    }
}
