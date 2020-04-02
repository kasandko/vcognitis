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

import "../SimpleControls"
import "../Singletons"

Popup {
    id: control

    function initAndOpen(titleMessage) {
        control.titleMessage = titleMessage;
        open();
    }

    property string titleMessage

    modal: true
    closePolicy: Popup.NoAutoClose

    CFColumn {
        anchors.centerIn: parent
        spacing: CFStyle.spacing

        BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 96
            height: width
        }

        CFLabel {
            text: titleMessage
            font.pixelSize: 18
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
