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
import QtQuick.Controls.Material 2.12
import ConfederateFinder 1.0

import "../SimpleControls"
import "../Singletons"
import "../SpecialControls"
import "../Popups"

CFPane {
    id: component

    property var deleteGroup
    property bool headLess: false
    property int currentIndex: 0

    // Enable connections(needs for small resolution)
    property bool enableStatus: true

    topPadding: 0
    bottomPadding: 0

    function revert() {
        groupManager.groupModel.select();
        groupManager.positiveGroups.invalidate();
        groupManager.negativeGroups.invalidate();
        interestSelector.reset(groupManager.interests);
        groupManager.interestsDirty = false;
    }

    /* Disable screen components
       Needed to block screen while saving */
    function disable() {
        rootItem.enabled = false;
    }

    /* Revert disable() */
    function enable() {
        rootItem.enabled = true;
    }

    /* Prepend empty row to model */
    function prependRow(positive) {
        groupManager.groupModel.addEmpty(positive);

        if (positive) {
            groupManager.positiveGroups.invalidate();
        } else {
            groupManager.negativeGroups.invalidate();
        }
    }

    /* Remove row by given idx */
    function removeRow(positive, idx) {
        var group = positive ? groupManager.positiveGroups.get(idx) : groupManager.negativeGroups.get(idx);

        if (group.downloaded) {
            deleteGroup = group;
            confirmDialog.acceptText = qsTr("Удалить");
            confirmDialog.title = qsTr("Удалить подписку?");
            confirmDialogConnection.enabled = true;
            confirmDialog.open();
            return;
        }

        groupManager.groupModel.remove(group);
    }

    function showDownloadSuccessMessage(groupId) {
        var message = qsTr("Подписчики группы <b>%1</b> успешно загружены").arg(groupManager.groupModel.get(String(groupId)).name);

        globalToast.show(message);
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: smallResolution ? CFStyle.padding : 0

        Rectangle {
            visible: smallResolution
            color: CFStyle.greyWhite

            Layout.fillWidth: true
            Layout.preferredHeight: 70

            TabBar {
                id: tabBar

                height: parent.height
                anchors {
                    left: parent.left
                    leftMargin: CFStyle.padding
                }

                Material.background: CFStyle.greyWhite
                Material.accent: CFStyle.blueMain

                CFGroupsListHeaderTabButton {
                    id: desiredTabButton

                    anchors.top: parent.top
                    count: groupManager.positiveGroups.rowCount()
                    width: implicitWidth
                    height: parent.height
                    text: qsTr("Желаемые подписки")
                    onClicked: component.currentIndex = 0
                }

                CFGroupsListHeaderTabButton {
                    id: undesiredTabButton

                    anchors.top: parent.top
                    count: groupManager.negativeGroups.rowCount()
                    width: implicitWidth
                    height: parent.height
                    text: qsTr("Нежелаемые подписки")
                    onClicked: component.currentIndex = 1
                }

                Connections {
                    target: groupManager.groupModel

                    onCountChanged: {
                        desiredTabButton.count = groupManager.positiveGroups.rowCount();
                        undesiredTabButton.count = groupManager.negativeGroups.rowCount();
                    }
                }
            }

            CFButton {
                text: qsTr("Добавить")

                icon.source: "qrc:/icons/Icon_add.svg"

                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: CFStyle.padding
                }

                onClicked: prependRow(currentIndex == 0)

                enabled: !userManager.downloadQueue.count
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            SwipeView {
                anchors.fill: parent
                interactive: false
                clip: true
                currentIndex: smallResolution ? component.currentIndex : 0

                RowLayout {
                    spacing: 0

                    CFGroupList {
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        headLess: component.headLess
                        title: qsTr("Желаемые подписки")
                        model: groupManager.positiveGroups
                        positive: true

                        onRemoveRow: component.removeRow(positive, index)
                        onPrependRow: component.prependRow(true)
                    }

                    CFVDivider {
                        visible: !smallResolution
                        Layout.fillHeight: true
                    }

                    CFGroupList {
                        visible: !smallResolution
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        title: qsTr("Нежелаемые подписки")
                        model: visible ? groupManager.negativeGroups : null
                        positive: false

                        onRemoveRow: component.removeRow(positive, index)
                        onPrependRow: component.prependRow(false)
                    }
                }

                CFGroupList {
                    visible: smallResolution
                    headLess: true
                    model: visible ? groupManager.negativeGroups : null
                    positive: false

                    onRemoveRow: component.removeRow(positive, index)
                    onPrependRow: component.prependRow(false)
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 70

            CFHDivider {
                width: parent.width
                anchors.top: parent.top
            }

            CFButton {
                enabled:!userManager.downloadQueue.count && !groupManager.interestsDirty && !groupManager.groupModel.dirtyStatus

                text: qsTr("Скачать все группы")
                anchors.left: parent.left
                anchors.leftMargin: CFStyle.padding
                anchors.verticalCenter: parent.verticalCenter
                width: 200

                onClicked: {
                    let groupModel = groupManager.groupModel;
                    for (let i = 0; i < groupModel.count; i++) {
                        let group = groupModel.get(i);
                        if (!group.downloaded && !group.subHidden) {
                            userManager.downloadQueue.add(group.uid);
                        }
                    }
                }
            }

            CFLabel {
                id: usersDownloadedLabel

                text: qsTr("Всего загружено: <font color=\"#000000\">%1</font> подписчик(ов)",
                           "",
                           userManager.model.usersDownloaded >= 1000 ? 5/*any plural*/: userManager.model.usersDownloaded)
                .arg(utils.subsToString(userManager.model.usersDownloaded));

                anchors.centerIn: parent

                color: CFStyle.black05
            }

            Row {
                spacing: CFStyle.spacing
                enabled: !userManager.downloadQueue.count

                anchors {
                    right: parent.right
                    rightMargin: CFStyle.padding
                    verticalCenter: parent.verticalCenter
                }

                CFButton {
                    id: cancelButton

                    text: qsTr("Отмена")

                    Material.background: CFStyle.blueLight
                    Material.foreground: CFStyle.blue
                    width: 100

                    onClicked: revert()
                }

                CFAnimatedDotsButton {
                    id: saveButton

                    flicker: groupManager.groupModel.dirtyStatus || groupManager.interestsDirty

                    text: qsTr("Сохранить")
                    width: 150

                    animation: !enabled && !userManager.downloadQueue.count

                    onClicked: {
                        disable();
                        groupManager.assertModelValidity();
                    }
                }
            }
        }
    }

    Connections {
        target: appWindow

        enabled: component.enableStatus

        onCurrentViewChanged: {
            if (currentView !== CFMainScreenView.Groups) {
                revert();
            }
        }
    }

    Connections {
        target: groupManager.groupModel

        enabled: component.enableStatus

        onCantAddEmpty: globalErrorToast.error(qsTr("Невозможно добавить группу"),
                                         qsTr("В списке присутствует незаполненная группа"))
    }
    Connections {
        target: groupManager

        enabled: component.enableStatus

        onErrorAt: globalErrorToast.error(qsTr("При работе с группами возникла ошибка"), message)

        onUserInterestDownloadFinished: {
            groupManager.interestsDirty = true;
            interestSelector.reset(items);
        }

        onModelValidityAssertionFinished: {
            if (!error) {
                groupManager.groupModel.submitAll();
                groupManager.groupModel.select();
                userManager.rescore(interestSelector.interestUids());
            } else {
                enable();
            }
        }

        onError: if (currentView === CFMainScreenView.Groups) globalErrorToast.error(qsTr("При работе с группами возникла ошибка"), message)
    }

    Connections {
        id: confirmDialogConnection

        enabled: component.enableStatus

        target: confirmDialog

        onAccepted: {
            confirmDialogConnection.enabled = false;
            groupManager.groupModel.remove(deleteGroup);
        }
    }

    Connections {
        target: userManager

        enabled: component.enableStatus

        onRescoreFinished: {
            enable();
            if (success) {
                userManager.select();
            } else {
                console.log("[qrc::CFGroupManager::onRescoreFinished] Rescore failed");
            }
        }
    }

    Connections {
        target: userManager.downloadQueue
        enabled: component.enableStatus
        onFinished: {
            disable();
            userManager.rescore(interestSelector.interestUids());
        }
        onGroupFinished: {
            if (success) {
                showDownloadSuccessMessage(groupId);
            } else {
                let groupName = groupManager.groupModel.get(String(groupId)).name;
                let title = qsTr("Ошибка при загрузке подписчиков группы <b>%1</b>").arg(groupName);
                let message = errorModel.getLast().userfriendlyMessage;
                globalErrorToast.error(title, message);
            }
        }
        onCancelled: {
            disable();
            userManager.rescore(interestSelector.interestUids());
        }
    }
}
