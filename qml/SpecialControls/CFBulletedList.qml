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

import "../SimpleControls"
import "../Singletons"

CFColumn {
    id: control

    property alias title: titleLabel.text
    property var model: []

    spacing: CFStyle.padding * 0.5

    CFLabel {
        id: titleLabel

        width: parent.width
        font.weight: Font.Medium
    }

    CFColumn {
        width: parent.width

        Repeater {
            model: control.model

            delegate: CFLabel {
                color: CFStyle.black06
                width: parent.width
                text:  "• " + modelData

                wrapMode: CFLabel.WordWrap
            }
        }
    }

}
