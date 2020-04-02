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
import QtQuick.Layouts 1.3

import "../Singletons"
import "../SimpleControls"


CFComboBox {
    id: control

    property alias filterText: filterTextField.text

    popup: Popup {
        id: popup

        width: control.width

        onOpened: filterTextField.clear()

        y: 0
        padding: 0

        background: CFPane {
            Material.elevation: 2
        }

        contentItem: ColumnLayout {
            implicitHeight: Math.min(CFStyle.controlHeight + listView.contentHeight + parent.padding * 2, 400)

            Item {
                height: CFStyle.controlHeight
                Layout.fillWidth: true

                CFTextField {
                    id: filterTextField

                    placeholderText: control.placeholder
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    width: parent.width - 35
                    font.pixelSize: 14
                    background: null
                }

                CFImage {
                    source: "qrc:/icons/Icon_down.svg"
                    sourceSize: Qt.size(24, 24)
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter

                    MouseArea {
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            bottom: parent.bottom
                        }

                        onClicked: popup.close
                    }
                }
            }

            MouseArea {
                id: hoverMouseArea

                Layout.fillHeight: true
                Layout.fillWidth: true
                hoverEnabled: true

                ListView {
                    id: listView

                    anchors.fill: parent
                    clip: true
                    model: control.popup.visible ? control.delegateModel : null
                    currentIndex: control.highlightedIndex

                    ScrollBar.vertical: CFScrollBar { targetContainsMouse: hoverMouseArea.containsMouse }
                }
            }
        }
    }
}
