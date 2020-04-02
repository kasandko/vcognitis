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
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Layouts 1.3

import "../Singletons"

ComboBox {
    id: control

    property string placeholder: ""
    property string text: ""

    property bool needPlaceholder: !control.displayText

    font.pixelSize: 14

    height: 30

    indicator: CFImage {
        source: "qrc:/icons/Icon_down.svg"
        sourceSize: Qt.size(24, 24)
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter

        Ripple {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 20
            height: 20
            pressed: control.pressed
            anchor: parent
            active: control.down || control.visualFocus || control.hovered
            color: control.flat && control.highlighted ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }
    }

    background: Rectangle {
        anchors.fill: parent
        radius: 2
        border.color: CFStyle.grey
        border.width: 1
    }

    contentItem: CFLabel {

        leftPadding: CFStyle.padding / 2
        rightPadding: control.indicator.width + CFStyle.padding / 2

        text: needPlaceholder ?  placeholder : control.displayText
        font: control.font
        color: needPlaceholder ? CFStyle.black05 : CFStyle.black08
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    delegate: CFComboBoxDelegate {
        width: parent.width
    }

    popup: Popup {
        width: control.width
        implicitHeight: contentItem.implicitHeight

        background: CFPane {
            Material.elevation: 2
        }

        y: 0
        padding: 0

        contentItem: ColumnLayout {
            implicitHeight: Math.min(CFStyle.controlHeight + listView.contentHeight + parent.padding * 2, 400)
            spacing: 0

            Item {
                height: CFStyle.controlHeight
                Layout.fillWidth: true

                CFLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: CFStyle.padding / 2
                    text: control.text
                    color: CFStyle.black05
                }
            }

            ListView {
                id: listView

                Layout.fillHeight: true
                Layout.fillWidth: true
                clip: true
                model: control.popup.visible ? control.delegateModel : null
                currentIndex: control.highlightedIndex
            }
        }
    }
}
