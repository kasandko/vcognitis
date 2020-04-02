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

    function clearSelection() {
        selectionModel.clear();
        selectionSet = new Set();
    }

    function openWithSelected(selectedObjects){
        clearSelection();
        for (var item of selectedObjects) {
            selectionSet.add(item.id);
            selectionModel.append(item);
        }
        open();
    }

    function doSearch() {
        searchListModel.clear();
        if (searchTextField.text == "") {
            return;
        }
        userManager.citySearch(searchTextField.text,
                               filterDialog.getCurrentCountryId());
    }

    title: qsTr("Выбор города")
    width: 900
    height: smallResolution ? 580 : 820
    modal: true

    onClosed: {
        searchTextField.clear();
        searchListModel.clear();
    }

    ListModel {
        id: selectionListModel
    }

    ListModel {
        id: searchListModel
    }

    Connections {
        target: userManager

        onNoCitiesFound: searchTextField.error = true
        onCitySearchFinished: {
            var items = JSON.parse(json);

            searchListModel.clear();

            for (var item of items) {
                searchListModel.append({
                   id: item.id,
                   title: item.title,
                   area: item.area ? item.area : "",
                   region: item.region ? item.region : "",
                })
            }
        }
    }

    contentItem: ColumnLayout {
        spacing: CFStyle.padding

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: searchTextField.height

            CFTextField {
                id: searchTextField

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: searchButton.left
                anchors.rightMargin: CFStyle.padding

                placeholderText: qsTr("Введите город")
                iconSource: "qrc:/icons/Icon_search_blue.svg"
                iconSize: Qt.size(24, 24)
                clearButtonEnabled: true
                onClearClicked: searchListModel.clear()
                onTextChanged: error = false;
            }

            CFButton {
                id: searchButton

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: CFStyle.padding
                }

                text: qsTr("Найти")
                onClicked: doSearch()
            }

            CFLabel {
                visible: searchTextField.error
                font.pixelSize: 12
                color: CFStyle.errorRed
                anchors.left: searchTextField.left
                anchors.leftMargin: CFStyle.padding / 2
                text: qsTr("Не найдено ни одного города")
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
                        id: selectedRepeater

                        model: selectionModel

                        delegate: CFFlowDelegate {
                            text: model.name
                            onIconClicked: {
                                selectionSet.delete(model.id);
                                selectionModel.remove(model.index);
                            }
                        }
                    }
                }

                CFLabel {
                    visible: selectionListModel.count == 0
                    text: qsTr("Вы ещё не добавили ни одного города")
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

            Layout.preferredWidth: parent.width + CFStyle.padding
            Layout.fillHeight: true
            hoverEnabled: true

            CFFlickable {
                anchors.fill: parent
                contentWidth: width - CFStyle.padding
                contentHeight: Math.max(cityFlow.implicitHeight, height)
                clip: true
                containsMouse: hoverMouseArea2.containsMouse

                CFColumn {
                    visible: !searchListModel.count
                    spacing: CFStyle.spacing

                    anchors.centerIn: parent

                    CFImage {
                        source: "qrc:/icons/Icon_search96.svg"
                        sourceSize: Qt.size(96, 96)

                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    CFLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Можно выбрать несколько вариантов")
                        font.weight: Font.Medium
                        font.pixelSize: 18
                    }

                    CFLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        horizontalAlignment: CFLabel.AlignHCenter
                        text: qsTr("Пожалуйста введите город и выберите нужный из\nпредложенного списка")
                        color: CFStyle.black05
                    }
                }

                Flow {
                    id: cityFlow

                    width: parent.width
                    spacing: CFStyle.padding / 2
                    flow: Flow.LeftToRight

                    Repeater {
                        model: searchListModel

                        delegate: CFPane {

                            Material.background: delegateMouseArea.containsMouse ? CFStyle.blueGray : CFStyle.white
                            padding: CFStyle.padding * 0.75
                            Material.elevation: 1
                            width: 200
                            height: 90

                            CFColumn {
                                id: cityColumn

                                anchors.fill: parent
                                spacing: CFStyle.padding * 0.25

                                CFLabel {
                                    width: parent.width

                                    text: model.title
                                    elide: CFLabel.ElideRight
                                }

                                CFLabel {
                                    visible: text != ""
                                    width: parent.width
                                    font.pixelSize: 12
                                    color: CFStyle.black05
                                    elide: CFLabel.ElideRight
                                    text: model.area
                                }

                                CFLabel {
                                    visible: text != ""
                                    font.pixelSize: 12
                                    color: CFStyle.black05
                                    width: parent.width
                                    text: model.region
                                    elide: CFLabel.ElideRight
                                }
                            }

                            MouseArea {
                                id: delegateMouseArea

                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked: {
                                    var id = model.id

                                    if (!selectionSet.has(id)) {
                                        selectionSet.add(id);
                                        selectionModel.append({
                                          name: model.title,
                                          id: id
                                        });
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
                    popup.close();
                }
            }
        }
    }

    Shortcut {
        enabled: popup.opened
        sequences: ["Enter", "Return"]
        onActivated: doSearch()
    }
}
