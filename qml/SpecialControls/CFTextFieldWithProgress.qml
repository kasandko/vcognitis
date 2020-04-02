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
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Singletons"
import "../SimpleControls"
import "../SpecialControls"

CFTextField {
    id: control

    property bool inProgress: false
    property double progress: 0 // [0.0, 1.0]

    property color backgroundColor: CFStyle.backgroundPrimay

    readOnly: inProgress

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: CFStyle.controlHeight
        color: error ? CFStyle.red005 : control.backgroundColor
        radius: inProgress ? 4 : 2

        border {
            width: 1
            color: inProgress ? CFStyle.blueGray : control.activeFocus ? CFStyle.colorPrimary : error ? CFStyle.red : CFStyle.grey
        }

        CFProgressFill {
            color: backgroundColor
            radius: parent.radius - 1
            progress: control.progress
            anchors.fill: parent
            anchors.margins: 1
        }
    }
}
