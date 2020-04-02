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

import "../Popups"
import "../SimpleControls"
import "../Singletons"

import ConfederateFinder 1.0

CFPane {
    id: control

    property int shortIndex: -1;
    property var selectionModel: interestSelectionListModel

    function remove(index) {
        selectionModel.remove(index);
        groupManager.interestsDirty = true;
    }

    function openPopup() {
        var items = [];
        for (var i = 0; i < selectionModel.count; i++)
            items.push(selectionModel.get(i));
        interestPopup.openWithSelected(items);
    }

    function interestUids() {
        var items = [];
        for (var i = 0; i < selectionModel.count; i++)
            items.push(selectionModel.get(i).uid);
        return items;
    }

    function reset(incomingArray) {
        selectionModel.clear();
        for (var i of incomingArray) {
            var row = groupManager.interestModel.find(i);
            var name = groupManager.interestModel.get(i.toString()).name;

            selectionModel.append({
                                      name: name,
                                      uid: i
                                  });
        }
        updateShortIndex()
    }

    function updateShortIndex() {
        const spacing = interestFlow.spacing;
        const maxRowWidth = interestFlow.width
        const maxHeight = interestFlow.height

        // First button width and spacing
        let currentRowWidth = openButton.implicitWidth + spacing; // Width of current row
        let totalHeight = CFStyle.controlHeight; // Total height of flow

        for (let i = 0; i < selectionModel.count; i++) {
            // Directly get width of item
            const flowButtonWidth = flowItems.itemAt(i).width;
            currentRowWidth += flowButtonWidth + spacing;

            // If i-th button does not fit into control horizonatlly
            if (currentRowWidth - spacing > maxRowWidth) {
                // Then it will go into new row and so add to total height
                totalHeight += spacing + CFStyle.controlHeight;
                // Then check whether it fits vertically
                if (totalHeight > maxHeight) {
                    // If it does not fit vertically then "..." must be shown
                    // at the place of i-th or (i - 1)-th flow button
                    // Test if "..." button will fit in place of i-th flow button

                    if (currentRowWidth - flowButtonWidth + hidedButton.width <= maxRowWidth) {
                        // Fits
                        return shortIndex = i;
                    } else {
                        // Does not fit, so start hiding from (i - 1)
                        return shortIndex = i - 1;
                    }
                } else {
                    // Otherwise there still enough space vertically and so loop procedes to next row
                    // which starts at i-th button that was just checked
                    currentRowWidth = flowButtonWidth + spacing;
                }
            }
        }

        shortIndex = -1;
    }

    padding: CFStyle.padding

    Connections {
        target: groupManager

        onInterestsChanged: reset(groupManager.interests);
    }

    height: 30

    Item {
        anchors.fill: parent

        Flow {
            id: interestFlow

            anchors.fill: parent
            spacing: CFStyle.spacing / 2

            onWidthChanged: updateShortIndex()
            onHeightChanged: updateShortIndex();

            CFButton {
                id: openButton

                text: qsTr("Интересы")
                icon.source: "qrc:/icons/icon_tune.svg"
                onClicked: openPopup()
            }

            Repeater {
                id: flowItems

                model: selectionModel

                delegate: CFFlowDelegate {
                    visible: shortIndex === -1 || model.index < shortIndex
                    color: enabled ? CFStyle.blueLight : CFStyle.setColorAlpha(CFStyle.blueLight, 0.5)
                    textColor: enabled ? CFStyle.blueMain : CFStyle.setColorAlpha(CFStyle.blueMain, 0.5)
                    text: model.name
                    onIconClicked: remove(model.index)
                }
            }

            CFButton {
                id: hidedButton

                width: 40
                display: AbstractButton.IconOnly
                visible: shortIndex != -1
                icon.source: "qrc:/icons/more_horiz_24px_white.svg"

                onClicked: openPopup()
            }
        }
    }


    CFRow {
        anchors.centerIn: parent
        visible: selectionModel.count == 0
        spacing: CFStyle.padding / 2

        CFImage {
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/icons/list-48px.svg"
            sourceSize: Qt.size(48, 48)
        }

        CFLabel {
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Список интересов пуст")
            color: CFStyle.blueGray
        }
    }

    Connections {
        target: interestPopup

        onSaveSelected: {
            selectionModel.clear();
            for (var item of selectedItems) {
                selectionModel.append(item);
            }
        }
    }
}



