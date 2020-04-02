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

import "../Singletons"
import "../SimpleControls"
import "../Popups"

import ConfederateFinder 1.0

Item {
    id: control

    property alias selectionModel: selectionListModel
    property int shortIndex: -1

    function openPopup() {
        var items = [];
        for (var i = 0; i < selectionModel.count; i++)
            items.push(selectionModel.get(i));
        downloadedGroupSelectorPopup.openWithSelected(items);
    }

    function uids() {
        var items = [];
        for (var i = 0; i < downloadedGroupSelector.selectionModel.count; i++)
            items.push(selectionModel.get(i).uid);
        return items;
    }

    function reset(incomingGroups) {
        selectionModel.clear();

        for (var uid of incomingGroups) {
            var group = groupManager.downloadedGroupProxy.find(uid);
            if (group !== null) {

                downloadedGroupSelector.selectionModel.append(
                    {
                        name: group.name,
                        uid: group.uid
                    }
                );
            }
        }
    }

    function clear() {
        selectionModel.clear();
    }

    function correctVisibleElements() {
        const spacing = flow.spacing;

        var rowWidth = addButton.width + spacing;
        var rowCount = 0;
        const maxRowWidth = flow.width;
        for (var i = 0; i < selectedRepeater.count; i++) {
            var item = selectedRepeater.itemAt(i);

            if (rowWidth + item.width + spacing > maxRowWidth) {
                rowCount++;
                if (rowCount > 1) {
                    shortIndex = i;
                    return;
                }

                rowWidth = item.width + spacing;
            } else {
                if (rowCount === 1 && rowWidth + item.width + spacing  + hidedButton.width > maxRowWidth) {
                    shortIndex = i;
                    return;
                }

                rowWidth += item.width + spacing;
            }
        }
        shortIndex = -1;
    }

    signal contentChanged()

    height: flow.height

    ListModel {
        id: selectionListModel

        onCountChanged: {
            control.contentChanged()
            correctVisibleElements();
        }
    }

    Connections {
        target: downloadedGroupSelectorPopup

        onSaveSelected: {
            selectionModel.clear();
            for (var item of selectedItems) {
                selectionModel.append(item);
            }
        }
    }

    Flow {
        id: flow

        property bool haveHided: false

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 4
        anchors.leftMargin: 0
        width: parent.width
        spacing: 10

        CFButton {
            id: addButton

            contentItem: Row {
                spacing: 5

                CFLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 12
                    text: qsTr("Выбрать группы")
                    color: CFStyle.white
                }

                CFImage {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/icons/Icon_add_20px.svg"
                    sourceSize: Qt.size(20, 20)
                }
            }

            onClicked: openPopup()
        }

        Repeater {
            id: selectedRepeater

            model: selectionModel

            delegate: CFFlowDelegate {
                visible: shortIndex === -1 || model.index < shortIndex
                text: model.name
                onIconClicked: selectionModel.remove(model.index)
            }
        }

        CFButton {
            id: hidedButton

            width: 40
            display: AbstractButton.IconOnly
            visible: shortIndex != -1 && !smallResolution
            icon.source: "qrc:/icons/more_horiz_24px_white.svg"

            onClicked: openPopup()
        }
    }
}
