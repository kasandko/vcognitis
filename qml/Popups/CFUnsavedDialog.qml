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
import QtQuick.Controls.Material 2.12
import ConfederateFinder 1.0

import "../SimpleControls"
import "../Singletons"

CFConfirmDialog {
    id: dialog

    property bool isExit: false
    property bool isFilter: false
    property bool isChangeMode: false
    property int targetScreenView
    property bool save
    property bool haveErrors

    function exit() {
        isExit = true;
        open();
    }

    function changeView(view, filter) {
        isChangeMode = false;
        targetScreenView = view;
        isFilter = filter;
        isExit = false;
        open();
    }

    function changeMode() {
        isChangeMode = true;
        isFilter = false;
        isExit = false;
        open();
    }

    title: qsTr("Сохранить изменения?")
    rightPadding: 40

    onRejected: {
        // Do nothing
    }

    onAccepted: {
        if (save) {
            rootItem.enabled = false;
            groupManager.assertModelValidity()
        } else {
            groupManager.groupModel.select();
            groupItem.resetSelectors();
            groupManager.interestsDirty = false;
        }
    }

    footer: Item {
        implicitHeight: footerRow.implicitHeight + CFStyle.padding * 2
        implicitWidth: rejectButton.width + acceptButton.width + cancelButton.width + CFStyle.spacing * 2 + CFStyle.padding * 2

        CFRow {
            id: footerRow

            spacing: CFStyle.spacing
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: CFStyle.padding

            CFButton {
                id: cancelButton

                text: qsTr("Отмена")
                width: 100
                Material.background: CFStyle.blueLight
                Material.foreground: CFStyle.blue08
                flat: true
                onClicked: reject()
            }

            CFButton {
                id: rejectButton

                text: qsTr("Не сохранять")
                width: 150
                Material.background: CFStyle.blueLight
                Material.foreground: CFStyle.blue08
                flat: true
                onClicked: {
                    if (isExit) {
                        appWindow.closeAccepted = true;
                        appWindow.close();
                        return;
                    }

                    save = false;

                    if (isChangeMode) {
                        isChangeMode = false;
                        if (utils.appMode === CFAppMode.Accurate) {
                            utils.appMode = CFAppMode.Quick;
                        } else {
                            utils.appMode = CFAppMode.Accurate;
                        }
                        accept();
                        return
                    }

                    currentView = targetScreenView;

                    if (isFilter || (openFilters && !userManager.downloadQueue.count)) {
                        openFilters = false;
                        filterDialog.open();
                    }

                    accept();
                }
            }

            CFButton {
                id: acceptButton

                width: 150
                text: qsTr("Сохранить")
                onClicked: { save = true; accept(); }
            }
        }
    }

    Connections {
        target: groupManager

        enabled: dialog.save

        onModelValidityAssertionFinished: {
            haveErrors = error;
        }
    }

    Connections {
        target: userManager

        enabled: dialog.save

        onRescoreFinished: {
            dialog.save = false;

            if (haveErrors) {
                return;
            }

            if (isExit) {
                appWindow.closeAccepted = true;
                appWindow.close();
                return;
            }

            if (isChangeMode) {
                isChangeMode = false;
                if (utils.appMode === CFAppMode.Accurate) {
                    utils.appMode = CFAppMode.Quick;
                } else {
                    utils.appMode = CFAppMode.Accurate;
                }
                return;
            }

            currentView = targetScreenView;

            if (isFilter || (openFilters && !userManager.downloadQueue.count)) {
                openFilters = false;
                filterDialog.open();
            }
        }
    }
}
