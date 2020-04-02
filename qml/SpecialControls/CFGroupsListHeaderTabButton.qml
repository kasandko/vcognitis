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

import "../Singletons"
import "../SimpleControls"

TabButton {
    id: control

    property int count: count

    font.pixelSize: 14
    font.capitalization: Font.MixedCase
    font.styleName: "Regular"
    Material.foreground: CFStyle.black05
    Material.accent: CFStyle.black08

    contentItem: CFRow {
        anchors.centerIn: parent
        spacing: control.spacing

        CFLabel {
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            font: control.font
            color: !control.enabled ? CFStyle.black05 : control.down || control.checked ? control.Material.accentColor : control.Material.foreground
        }

        CFLabel {
            anchors.verticalCenter: parent.verticalCenter
            text: count
            color: CFStyle.black03
        }
    }
}
