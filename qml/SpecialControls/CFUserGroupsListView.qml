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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import "../SimpleControls"
import "../Singletons"

Item {
    id: control

    property bool positive
    property alias model: listView.model

    MouseArea {
        id: hoverMouseArea

        anchors.fill: parent
        hoverEnabled: true

        ListView {
            id: listView

            spacing: CFStyle.spacing

            anchors.fill: parent

            headerPositioning: ListView.OverlayHeader
            boundsBehavior: ListView.StopAtBounds

            header: CFPane {
                z: 3
                width: listView.width
                Item {
                    implicitHeight: headerLabelRow.implicitHeight
                    anchors.left: parent.left
                    anchors.right: parent.right

                    CFRow {
                        id: headerLabelRow

                        spacing: CFStyle.spacing * 0.5

                        Rectangle {
                            width: 5
                            height: width
                            radius: width / 2
                            color: positive ? CFStyle.green : CFStyle.red

                            anchors.verticalCenter: parent.verticalCenter
                        }

                         CFLabel {
                             font.pixelSize: 18

                             text: positive ? qsTr("Желаемые подписки") : qsTr("Нежелаемые подписки")
                             font.weight: Font.DemiBold
                         }
                    }
                }
            }

            delegate: Item {
                implicitHeight: 50
                width: listView.width

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: CFStyle.padding
                    anchors.rightMargin: anchors.leftMargin
                    spacing: CFStyle.spacing * 0.75

                    CFImage {
                        rounded: true

                        source: photo

                        sourceSize: Qt.size(parent.height, parent.height)
                    }

                    CFColumn {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        spacing: CFStyle.spacing * 0.2

                        CFLabel {
                            width: parent.width
                            color: CFStyle.black08
                            elide: Text.ElideRight
                            text: name
                            font.weight: Font.DemiBold
                        }

                        CFLabel {
                            width: parent.width
                            color: CFStyle.black05
                            elide: Text.ElideRight
                            text: screenName
                        }
                    }
                }
            }

            footer: Item { height: CFStyle.padding * 0.5; width: listView.width }

            ScrollBar.vertical: CFScrollBar{ targetContainsMouse: hoverMouseArea.containsMouse }

            CFColumn {
                visible: !parent.count
                anchors.centerIn: parent
                spacing: CFStyle.spacing

                CFImage {
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "qrc:/icons/list96.svg"
                    sourceSize: Qt.size(96, 96)
                }
                CFLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 18
                    font.weight: Font.Medium
                    text: qsTr("Список пуст")
                }
            }
        }
    }
}
