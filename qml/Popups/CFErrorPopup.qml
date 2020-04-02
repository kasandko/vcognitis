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
import QtQuick.Controls.Material.impl 2.12

import "../Singletons"
import "../SimpleControls"

Popup {
    id: control

    property int autoHideInterval: 5000

    function error(title, description) {
        autoHideTimer.start();
        titleLabel.text = title;
        descriptionLabel.text = description;

        control.open();
    }


    padding: CFStyle.padding
    Material.background: CFStyle.errorToastColor
    Material.elevation: 1

    contentWidth: rootColumn.implicitWidth
    contentHeight: rootColumn.implicitHeight

    background: Rectangle {
        radius: 4
        color: control.Material.dialogColor

        layer.enabled: control.Material.elevation > 0
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }
    }

    CFColumn {
        id: rootColumn

        opacity: 0.8
        spacing: CFStyle.padding

        CFLabel {
            id: titleLabel

            color: CFStyle.errorRed
            font.pixelSize: 18
        }

        CFLabel {
            id: descriptionLabel

            color: CFStyle.errorRed

            font.pixelSize: 14
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            autoHideTimer.stop();
            control.close();
        }
    }

    Timer {
        id: autoHideTimer

        repeat: false
        interval: autoHideInterval

        onTriggered: control.close()
    }
}
