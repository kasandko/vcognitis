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
import QtQuick.Layouts 1.12

import "../SimpleControls"
import "../Singletons"
import "../SpecialControls"

CFDialog {
    id: control

    function initAndOpen(fullName, positives, negatives) {
        control.title = qsTr("Подписки пользователя: %1").arg(fullName);
        positiveList.model = positives;
        negativeList.model = negatives;
        open();
    }

    modal: true

    RowLayout {
        spacing: 0

        anchors.fill: parent

        CFUserGroupsListView {
            id: positiveList

            positive: true

            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        CFVDivider { Layout.fillHeight: true }

        CFUserGroupsListView {
            id: negativeList

            positive: false

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    footer: Item {
        implicitHeight: CFStyle.dialogHeaderHeight

        CFPane {
            anchors.fill: parent

            Item {
                anchors.fill: parent

                CFButton {
                    width: 150

                    text: qsTr("Ок")

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }

                    onClicked: reject()
                }
            }
        }

        CFHDivider {
            width: parent.width

            anchors {
                top: parent.top
                topMargin: -height
            }
        }
    }
}
