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

import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

import ConfederateFinder 1.0

import "../Singletons"
import "../SimpleControls"
import "../SpecialControls"

Rectangle {
    id: control

    function changeView(view){
        if (groupManager.groupModel.dirtyStatus || groupManager.interestsDirty) {
            tabBar.currentIndex = currentView;
            return unsavedChangesDialog.changeView(view, false)
        }

        if (filterDialog.opened && !openFilters)
            filterDialog.close();
        if ((openFilters && !userManager.downloadQueue.count)) {
            filterDialog.open();
            openFilters = false;
        }

        currentView = view;
    }

    implicitHeight: CFStyle.mainHeaderHeight

    color: CFStyle.colorPrimary

    layer.enabled: control.visible
    layer.effect: ElevationEffect {
        elevation: 3
    }

    TabBar {
        id: tabBar

        height: parent.height
        anchors {
            left: parent.left
            leftMargin: CFStyle.padding
        }

        Material.background: CFStyle.colorPrimary
        Material.accent: "white"

        CFHeaderTabButton {
            width: implicitWidth
            height: parent.height
            text: qsTr("Поиск")
            icon.source: "qrc:/icons/Icon_search.svg"
            onClicked: changeView(CFMainScreenView.Search)
        }

        CFHeaderTabButton {
            width: implicitWidth
            height: parent.height
            text: qsTr("Избранные")
            icon.source: "qrc:/icons/Icon_headerstar.svg"
            onClicked: changeView(CFMainScreenView.Favorite)
        }

        CFHeaderTabButton {
            width: implicitWidth
            height: parent.height
            text: qsTr("Настройка подписок")
            icon.source: "qrc:/icons/Icon_Sub.svg"
            onClicked: changeView(CFMainScreenView.Groups)
        }

        Connections {
            target: appWindow
            onCurrentViewChanged: tabBar.currentIndex = currentView
        }
    }

    Row {
        anchors {
            left: tabBar.right
            leftMargin: CFStyle.padding
        }

        spacing: 20
        height: parent.height

        CFSearchParamsButton {
            id: searchParamsButton

            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            id: separatorRect

            height: parent.height
            width: 1
            color: CFStyle.white
            opacity: 0.15
        }

        CFModesSwitchButton {
            id: changeModesButton

            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        spacing: CFStyle.spacing * 0.5
        visible: userManager.score || userManager.downloadQueue.count || userManager.ageSearch || userManager.rescoringActive

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: CFStyle.padding
        }

        CFLabel {
            visible: !userManager.rescoringActive

            text: {
                if (userManager.downloadQueue.count) {
                    return qsTr("Осталось: ") + utils.secs2TimeString(userManager.downloadQueue.estimatedDuration)
                }

                if (userManager.score) {
                    return qsTr("Осталось: ") + utils.secs2TimeString(userManager.score.estimatedDuration)
                }

                if (userManager.ageSearch) {
                    return qsTr("Осталось: ") + utils.secs2TimeString(userManager.ageSearch.estimatedDuration)
                }

                return "";
            }

            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            font.weight: Font.Medium
        }

        CFRoundButton {
            flat: true

            Material.foreground: "white"
            visible: !userManager.ageSearch && !userManager.rescoringActive

            icon {
                source: {
                    if (userManager.downloadQueue.count) {
                        return userManager.downloadQueue.paused ? "qrc:/icons/Icon_play.svg" : "qrc:/icons/Icon_pause.svg";
                    }

                    if (userManager.score) {
                        return userManager.score.paused ? "qrc:/icons/Icon_play.svg" : "qrc:/icons/Icon_pause.svg";
                    }

                    return "";
                }
                width: 24
                height: 24
            }

            onClicked: userManager.downloadQueue.count ? userManager.downloadQueue.togglePaused() : userManager.score.togglePause()

            CFToolTip {
                visible: parent.hovered
                text: qsTr(parent.icon.source == "qrc:/icons/Icon_pause.svg" ? "Приостановить" : "Продолжить")
            }
        }

        CFProgressFill {
            radius: 4
            width: 224
            height: CFStyle.controlHeight

            anchors.verticalCenter: parent.verticalCenter

            progress: {
                if (userManager.rescoringActive) {
                    return userManager.rescoringPercent;
                }

                if (userManager.downloadQueue.count) {
                    return userManager.downloadQueue.progress;
                }

                if (userManager.ageSearch) {
                    return userManager.ageSearch.progress;
                }

                if (userManager.score) {
                    return userManager.score.progress;
                }

                return 0;
            }

            CFLabel {
                text: {
                    if (userManager.rescoringActive)
                        return qsTr("Переоценка (%1\%)").arg(Math.round(userManager.rescoringPercent * 100));
                    if (userManager.downloadQueue.count)
                        return qsTr("Получение списков (%1\%)").arg(Math.round(userManager.downloadQueue.progress * 100));
                    if (userManager.ageSearch)
                        return qsTr("Уточнение возраста (%1\%)").arg(Math.round(userManager.ageSearch.progress * 100));
                    if (userManager.score)
                        return qsTr("Анализ профилей (%1\%)").arg(Math.round(userManager.score.progress * 100));
                    return ""
                }

                leftPadding: CFStyle.padding * 0.5
                verticalAlignment: Label.AlignVCenter
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: cancelButton.left
                font.weight: Font.Medium
            }

            CFRoundButton {
                id: cancelButton

                width: CFStyle.controlHeight
                height: width
                anchors.right: parent.right
                icon.source: "qrc:/icons/close14.svg"
                icon.width: 14
                icon.height: 14
                flat: true
                Material.foreground: CFStyle.blueGray
                visible: !userManager.rescoringActive

                onClicked: {
                    userManager.score ? userManager.cancelScoring() : userManager.downloadQueue.cancel()
                }
            }
        }
    }
}
