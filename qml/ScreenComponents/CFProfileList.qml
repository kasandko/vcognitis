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

import "../SimpleControls"
import "../SpecialControls"
import "../Singletons"

CFListView {
    id: listView

    property bool containsMouse: false

    clip: false
    spacing: CFStyle.spacing * 0.5
    delegate: Item {
        height: 175
        width: listView.width

        CFProfileDelegate {
            height: parent.height
            width: smallResolution ? 900 : (parent.width - CFStyle.padding * 1.6)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: smallResolution ? 0 : (CFStyle.padding * 0.2)
        }
    }

    ScrollBar.vertical: CFScrollBar{ targetContainsMouse: listView.containsMouse }
}
