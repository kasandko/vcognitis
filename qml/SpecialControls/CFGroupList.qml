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
import QtGraphicalEffects 1.12
import ConfederateFinder 1.0

import "../SimpleControls"
import "../Singletons"

Item {
    id: control

    property string title
    property var model
    property bool positive

    property bool headLess: false

    property int nameFieldWidth: 330
    property int urlFieldWidth: width - CFStyle.padding * 2.35 - 12 * CFStyle.roundButtonSize - nameFieldWidth + 63

    signal removeRow(int index)
    signal prependRow()

    Rectangle {
        id: controlHeader

        visible: !headLess

        color: CFStyle.greyWhite
        width: parent.width
        height: 70

        CFRow {
            spacing: CFStyle.spacing * 0.5

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: CFStyle.padding * 1.5

            Rectangle {
                color: positive ? CFStyle.green : CFStyle.red
                radius: 3
                width: 5
                height: width
                anchors.verticalCenter: parent.verticalCenter
            }

            CFLabel {
                font.pixelSize: 18
                font.styleName: "Normal"
                font.weight: Font.DemiBold
                text: title
            }
        }

        CFButton {
            text: qsTr("Добавить")

            icon.source: "qrc:/icons/Icon_add.svg"

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: CFStyle.padding * 0.75
            }

            onClicked: prependRow()

            enabled: !userManager.downloadQueue.count
        }

        CFHDivider {
            anchors.bottom: parent.bottom
            width: parent.width
        }
    }

    Item {
        id: listHeader

        width: parent.width
        height: CFStyle.controlHeight

        anchors.left: parent.left
        anchors.top: headLess ? control.top : controlHeader.bottom
        anchors.topMargin: smallResolution ? undefined : CFStyle.padding

        CFLabel {
            id: nameLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: CFStyle.padding * 0.25 + CFStyle.roundButtonSize

            width: nameFieldWidth

            text: qsTr("Название группы")
            color: CFStyle.black05
            leftPadding: 8
        }

        CFLabel {
            id: urlLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: nameLabel.right
            anchors.leftMargin: CFStyle.spacing * 1.35
            width: urlFieldWidth
            text: qsTr("URL")
            color: CFStyle.black05
        }

        CFLabel {
            id: weightLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: urlLabel.right
            anchors.leftMargin: CFStyle.spacing * 0.85
            text: qsTr("Оценка")
            color: CFStyle.black05
        }

        CFHDivider { width: parent.width; anchors.bottom: parent.bottom }
    }

    MouseArea {
        id: hoverMouseArea

        anchors {
            top: listHeader.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        hoverEnabled: true

        CFListView {
            id: list


            anchors.fill: parent
            spacing: 0
            model: control.model

            delegate: CFGroupDelegate {
                id: groupDelegate

                width: list.width

                nameFieldWidth: control.nameFieldWidth
                urlFieldWidth: control.urlFieldWidth
                weight: model.weight
                url: model.screenName
                desired: control.positive
                name: model.name
                queueIndex: userManager.downloadQueue.indexOf(model.uid)

                onUpdateUrl: model.screenName = url
                onUpdateWeight: model.weight = weight
                onRemoveGroup: removeRow(model.index)

                Connections {
                    target: groupManager

                    onErrorAt:  if (control.model.mapFromSource(groupManager.groupModel.index(sourceRow, 0)).row === model.index) groupDelegate.error = true;
                }
            }

            ScrollBar.vertical: CFScrollBar{ targetContainsMouse: hoverMouseArea.containsMouse }

            CFColumn {
                anchors.centerIn: parent
                visible: !parent.count
                spacing: CFStyle.spacing

                CFImage {
                    anchors.horizontalCenter: parent.horizontalCenter

                    source: "qrc:/icons/Icon_list_add.svg"
                    sourceSize: Qt.size(96, 96)
                }

                CFLabel {
                    text: qsTr("Список пуст")
                    font.weight: Font.Medium
                    font.pixelSize: 18
                    color: CFStyle.black08
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                CFLabel {
                    text: qsTr("Нажмите на кнопку \"Добавить\" \nдля добавления подписки")
                    color: CFStyle.black05
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    lineHeight: 20
                    lineHeightMode: Text.FixedHeight
                }

                CFRow {
                    visible: positive
                    spacing: CFStyle.spacing * 0.75
                    anchors.horizontalCenter: parent.horizontalCenter

                    CFButton {
                        text: qsTr("Загрузить")

                        width: 100
                        enabled: !userGroupCountTextField.error

                        onClicked: groupManager.refreshUserGroups(userGroupCountTextField.text)
                    }

                    CFTextField {
                        id: userGroupCountTextField

                        width: 50
                        error: !acceptableInput
                        anchors.verticalCenter: parent.verticalCenter
                        validator: IntValidator {
                            id: userGroupCountTextFieldValidator

                            top: 99
                            bottom: 1
                        }
                        horizontalAlignment: TextInput.AlignHCenter

                        Connections {
                            target: groupManager
                            // Properly set text field and validator upon receiving user group count
                            onUserGroupsCountChanged: {
                                var maxGroupCount = 99;
                                var groupCount = groupManager.userGroupsCount;

                                if (groupCount < maxGroupCount) {
                                    userGroupCountTextFieldValidator.top = groupCount;
                                    userGroupCountTextField.text = groupCount;
                                } else {
                                    userGroupCountTextFieldValidator.top = maxGroupCount;
                                    userGroupCountTextField.text = maxGroupCount;
                                }
                            }
                        }
                    }

                    CFLabel {
                        anchors.verticalCenter: parent.verticalCenter
                        font.weight: Font.Medium
                        color: CFStyle.black
                        text: qsTr("ваших подпис(ки)", "", parseInt(userGroupCountTextField.text))
                    }
                }
            }
        }
    }
}
