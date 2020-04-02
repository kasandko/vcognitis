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
import QtQuick.Layouts 1.3

import "../Singletons"
import "../SimpleControls"
import "../SpecialControls"

import ConfederateFinder 1.0

CFPopup {
    id: popup

    property alias selectionModel: selectionListModel
    property var selectionSet: new Set()
    
    signal saveSelected(var selectedItems);

    function openWithSelected(selectedObjects){
        clearSelection();
        for (var item of selectedObjects) {
            selectionSet.add(item.uid);
            selectionModel.append(item);
        }

        open();
    }

    function clearSelection() {
        selectionModel.clear();
        selectionSet = new Set();
    }

    function addToSelection(model) {
        var id = model.uid

        if (!selectionSet.has(id)) {
            selectionSet.add(id);
            selectionModel.append({
                name: model.name,
                uid: id
            });
        }
    }

    onClosed: searchTextField.clear()

    onOpened: categoryFlow.correctHeight();

    title: qsTr("Интересы")
    
    modal: true
    
    height: smallResolution ? 580 : 820
    width: 900

    ListModel {
        id: selectionListModel
    }
    
    contentItem: ColumnLayout {
        spacing: CFStyle.padding
        
        CFTextField {
            id: searchTextField
            
            placeholderText: qsTr("Поиск интересов, например автомобили, IT, новости")
            Layout.preferredWidth: parent.width
            iconSource: "qrc:/icons/Icon_search_blue.svg"
            iconSize: Qt.size(24, 24)
            clearButtonEnabled: true

            onTextChanged: {
                groupManager.interestSearchModel.searchString = text.toLowerCase();
                error = groupManager.interestSearchModel.rowCount() === 0;
                categoryFlow.correctHeight();
            }

            CFLabel {
                visible: parent.error
                font.pixelSize: 12
                color: CFStyle.errorRed
                anchors.left: parent.left
                anchors.leftMargin: CFStyle.padding / 2
                text: qsTr("Не найдено. Введите корректное название интереса, например автомобили")
                anchors.top: parent.bottom
                anchors.topMargin: 5
            }
            Layout.bottomMargin: 10
        }


        MouseArea {
            id: hoverMouseArea

            Layout.preferredWidth: parent.width + CFStyle.padding
            Layout.preferredHeight: Math.max(70, Math.min(smallResolution ? 60 : 190, selectedFlow.height))
            hoverEnabled: true

            CFFlickable {
                anchors.fill: parent
                clip: true
                contentHeight: Math.max(70, selectedFlow.height)
                contentWidth: width - CFStyle.padding
                contentY: contentHeight - height
                containsMouse: hoverMouseArea.containsMouse

                Flow {
                    id: selectedFlow

                    spacing: CFStyle.padding / 2
                    width: parent.width

                    Repeater {
                        model: selectionListModel

                        delegate: CFFlowDelegate {
                            text: model.name
                            onIconClicked:{
                                selectionSet.delete(model.uid);
                                selectionModel.remove(model.index);
                            }
                        }
                    }
                }

                CFLabel {
                    visible: selectionListModel.count == 0
                    text: qsTr("Можно выбрать несколько вариантов")
                    anchors.centerIn: parent
                    color: CFStyle.black05
                }
            }
        }
        
        CFHDivider {
            Layout.leftMargin: - parent.spacing
            width: parent.width + parent.spacing * 2
        }

        MouseArea {
            id: hoverMouseArea2

            Layout.preferredWidth: parent.width + 20
            Layout.fillHeight: true
            hoverEnabled: true

            CFFlickable {
                anchors.fill: parent
                clip: true
                contentHeight: categoryFlow.implicitHeight
                containsMouse: hoverMouseArea2.containsMouse

                CFColumn {
                    spacing: CFStyle.padding
                    width: parent.width

                    CFLabel {
                        visible: categoryFlow.search
                        font.weight: Font.Medium
                        text: qsTr("Результаты поиска")
                    }

                    Flow {
                        id: categoryFlow

                        property bool search: searchTextField.text != "" && !searchTextField.error

                        function correctHeight() {
                            if (search) {
                                return height = groupManager.interestSearchModel.rowCount() * 40 / 4 + 20;
                            }

                            var heightArray = [];

                            var topItemHeght = CFStyle.padding / 2 + 40

                            var sum = 0;

                            for (var i = 0; i <  groupManager.categoriesCount; i++) {
                                var count = groupManager.interestByCategoryModel(i).rowCount();
                                if (count !== 0) {
                                    heightArray.push(topItemHeght + 30 * count);
                                    sum += topItemHeght + 30 * count;
                                }
                            }

                            var tempHeight = sum / 3.5;

                            // This part for remove empty space under current flow.
                            var maxHeight = 0;
                            var currentRowHeight = 0;

                            for (var item of heightArray) {
                                if ((currentRowHeight + item) < tempHeight) {
                                    currentRowHeight += item;
                                } else {
                                    maxHeight = Math.max(currentRowHeight, maxHeight);
                                    currentRowHeight = item;
                                }
                            }

                            height = maxHeight;
                        }

                        onSearchChanged: correctHeight()
                        spacing: CFStyle.padding / (search ? 2 : 1)
                        width: parent.width
                        height: 1000
                        flow: search ? Flow.LeftToRight : Flow.TopToBottom

                        // Search elements
                        Repeater {
                            model: groupManager.interestSearchModel
                            width: 200

                            delegate: Rectangle {
                                visible: categoryFlow.search
                                width: 200
                                height: 30

                                CFLabel {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.leftMargin: CFStyle.padding / 2
                                    anchors.right: parent.right
                                    anchors.rightMargin: CFStyle.padding / 2

                                    elide: CFLabel.ElideRight
                                    text: model.name
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onHoveredChanged: parent.color = containsMouse ? CFStyle.blueLight : CFStyle.white

                                    onClicked: addToSelection(model)
                                }
                            }
                        }


                        // usual elements
                        Repeater {
                            model: groupManager.categoriesCount

                            delegate: CFColumn {
                                id: categoryContainer

                                visible: !categoryFlow.search

                                property var currentModel: groupManager.interestByCategoryModel(model.index)

                                width: 200
                                spacing: CFStyle.padding / 2

                                Rectangle {
                                    width: parent.width
                                    height: categoriesLabel.height + 10 * 2

                                    CFLabel {
                                        id: categoriesLabel

                                        font.pixelSize: 18
                                        color: CFStyle.blueMain
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: CFStyle.padding / 2
                                        width: parent.width - CFStyle.padding
                                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                        text: currentModel.parentName
                                        font.weight: Font.DemiBold
                                    }

                                    MouseArea {
                                        enabled: currentModel.rowCount() === 1;
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onHoveredChanged: parent.color = containsMouse ? CFStyle.blueLight : CFStyle.white

                                        onClicked: {
                                            var modelIndex  = currentModel.indexByRow(0);
                                            addToSelection({uid: currentModel.data(modelIndex, CFInterestRoles.Id),
                                                            name: currentModel.data(modelIndex, CFInterestRoles.Name)
                                            })
                                        }
                                    }
                                }

                                CFColumn {
                                    visible: currentModel.rowCount() > 1
                                    spacing: 0
                                    width: parent.width

                                    Repeater {
                                        enabled: currentModel.rowCount() > 1

                                        model: currentModel
                                        width: parent.width

                                        delegate: Rectangle {
                                            width: parent.width
                                            height: interestLabel.height + 8 * 2

                                            CFLabel {
                                                id: interestLabel

                                                anchors.left: parent.left
                                                anchors.verticalCenter: parent.verticalCenter
                                                anchors.leftMargin: CFStyle.padding / 2
                                                anchors.right: parent.right
                                                anchors.rightMargin: CFStyle.padding / 2
                                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                                text: model.name
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                onHoveredChanged: parent.color = containsMouse ? CFStyle.blueLight : CFStyle.white

                                                onClicked: addToSelection(model)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        CFHDivider {
            width: parent.width + parent.spacing * 2
            Layout.leftMargin: - parent.spacing
        }
        
        CFRow {
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: CFStyle.padding
            spacing: CFStyle.padding
            
            CFButton {
                height: 30
                topInset: 0
                bottomInset: 0
                width: 100
                Material.foreground: CFStyle.blue
                Material.background: CFStyle.blueLight
                text: qsTr("Отмена")
                
                onClicked: popup.close();
            }
            
            CFButton {
                height: 30
                topInset: 0
                bottomInset: 0
                width: 150
                text: qsTr("Применить")
                
                onClicked: {
                    var items = [];
                    for (var i = 0; i < selectionModel.count; i++)
                        items.push(selectionModel.get(i));

                    popup.saveSelected(items);
                    // TODO check if there are actually any changes
                    groupManager.interestsDirty = true;
                    popup.close()
                }
            }
        }
    }
}
