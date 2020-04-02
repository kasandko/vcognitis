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
import ConfederateFinder 1.0

import "../SimpleControls"

Rectangle {
    id: viewedRectangle
    
    color: "white"
    radius: 4
    width: 308
    height: 36

    Material.elevation: 1
    
    MouseArea {
        anchors.fill: parent // block propagating mouse clicks
    }
    
    CFImage {
        anchors {
            left: parent.left
            leftMargin: 16
            verticalCenter: parent.verticalCenter
        }
        
        sourceSize: Qt.size(24, 24)
        source: "qrc:/icons/Icon_viewed.svg"
    }
    
    CFLabel {
        anchors {
            left: parent.left
            leftMargin: 49
            verticalCenter: parent.verticalCenter
        }
        
        text: "%1%".arg(userManager.model.viewedPercent)
    }
    
    Rectangle {
        id: viewedSeparator
        
        anchors {
            left: parent.left
            leftMargin: 92
        }
        
        height: parent.height
        width: 1
        color: "#EDEEF0"
    }
    
    CFButton {
        anchors {
            right: parent.right
            left: viewedSeparator.right
        }
        text: userManager.model.showViewed ? qsTr("Скрывать просмотренные") : qsTr("Показывать просмотренные")
        height: parent.height
        Material.background: "white"
        Material.foreground: "#4A76A8"
        background.layer.effect: null
        onClicked: userManager.model.showViewed = !userManager.model.showViewed
    }

    layer.enabled: viewedRectangle.visible
    layer.effect: ElevationEffect {
        elevation: viewedRectangle.Material.elevation
    }
}
