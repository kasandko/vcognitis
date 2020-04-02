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

Item {
    id: control

    property int spacing: 5
    property bool running: true
    property int animationDuration: 300

    implicitHeight: 4
    implicitWidth: recRow.implicitWidth

    Row {
        id: recRow

        height: parent.height
        spacing: control.spacing

        Rectangle {
            id: rec1

            width: parent.height
            height: width
            radius: width / 2
        }

        Rectangle {
            id: rec2

            width: parent.height
            height: width
            radius: width / 2
        }

        Rectangle {
            id: rec3

            width: parent.height
            height: width
            radius: width / 2
        }
    }

    SequentialAnimation {
        loops: Animation.Infinite
        running: control.running

        ParallelAnimation {
            NumberAnimation { target: rec1; property: "opacity"; from: 1; to: 0.5; duration: animationDuration }
            NumberAnimation { target: rec2; property: "opacity"; from: 0.8; to: 1; duration: animationDuration }
            NumberAnimation { target: rec3; property: "opacity"; from: 0.5; to: 0.8; duration: animationDuration }
        }

        ParallelAnimation {
            NumberAnimation { target: rec1; property: "opacity"; from: 0.5; to: 0.8; duration: animationDuration }
            NumberAnimation { target: rec2; property: "opacity"; from: 1; to: 0.5; duration: animationDuration }
            NumberAnimation { target: rec3; property: "opacity"; from: 0.8; to: 1; duration: animationDuration }
        }

        ParallelAnimation {
            NumberAnimation { target: rec1; property: "opacity"; from: 0.8; to: 1; duration: animationDuration }
            NumberAnimation { target: rec2; property: "opacity"; from: 0.5; to: 0.8; duration: animationDuration }
            NumberAnimation { target: rec3; property: "opacity"; from: 1; to: 0.5; duration: animationDuration }
        }
    }
}
