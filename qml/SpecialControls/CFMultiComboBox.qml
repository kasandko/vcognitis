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
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Layouts 1.3

import "../Singletons"
import "../SimpleControls"


CFComboBox {
    id: control

    property string placeholder: ""
    property string hintPlaceHodler: qsTr("Выберите значение")

    property bool wasDelete: false

    property alias selectionModel: selectionListModel

    function uids() {
        var items = [];
        for (var i = 0; i < selectionModel.count; i++)
            items.push(selectionModel.get(i).uid);
        return items;
    }

    function reset(incomingArray) {
        selectionModel.clear();
        for (var i of incomingArray) {
            var item = null;
            for (var j = 0;  j < model.count; j++) {
                item = model.get(j);
                if (item.value === i) {
                    break;
                }
            }

            if (item !== null) {
                selectionModel.append({
                    name: item.name,
                    uid: i
                });
            }
        }
    }

    function findInSelection(uid) {
        for (var i = 0;  i < selectionModel.count; i++) {
            var item = selectionModel.get(i);
            if (item.uid === uid) {
                return item;
            }
        }

        return null;
    }

    signal contentChanged()

    ListModel {
        id: selectionListModel

        onCountChanged: control.contentChanged();
    }

    text: placeholder
    height: background.implicitHeight

    indicator: null
    contentItem: null

    background: Item {
        implicitHeight: backgroundColumn.implicitHeight

        CFColumn {
            id: backgroundColumn
            spacing: CFStyle.padding / 2 + 1
            width: parent.width
            CFLabel {
                text: placeholder
                font.weight: Font.Medium
            }

            Flow {
                id: flow

                width: parent.width
                spacing: 10

                CFLabel {
                    height: CFStyle.controlHeight
                    visible: selectionModel.count === 0
                    text: hintPlaceHodler
                    color: CFStyle.black05
                    verticalAlignment: Text.AlignVCenter
                }

                Repeater {
                    id: selectedRepeater

                    model: selectionModel

                    delegate: CFFlowDelegate {
                        onIconClicked: selectionModel.remove(index)
                        text: model.name
                    }
                }

                Item {
                    width: 24; height: CFStyle.controlHeight;

                    CFImage {
                        anchors.centerIn: parent
                        source: "qrc:/icons/Icon_down.svg"
                        sourceSize: Qt.size(24, 24)
                    }

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

            }

            CFHDivider {
                height: 1
                width: parent.width
            }
        }
    }

    popup.y: -popup.height + CFStyle.controlHeight - CFStyle.padding / 2

    delegate: Item {
        width: parent.width
        height: checkDelegate.height

        CFComboBoxDelegate {
            id: checkDelegate

            width: control.width
            height: CFStyle.controlHeight

            onClicked: {
                if (findInSelection(model.value) === null)
                    selectionModel.append({name: model.name, uid: model.value})

                control.popup.close();
            }
        }
    }
}
