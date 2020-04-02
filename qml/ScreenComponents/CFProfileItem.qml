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
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.12
import ConfederateFinder 1.0

import "../SimpleControls"
import "../SpecialControls"
import "../Singletons"

Item {
    id: control

    property int currentView
    property var currentListView: control.currentView == CFMainScreenView.Favorite ? favoriteListView : userListView

    clip: true

    MouseArea {
        id: hoverMouseArea

        anchors {
            fill: parent
            topMargin: CFStyle.padding
            bottomMargin: anchors.topMargin
        }

        hoverEnabled: true

        SwipeView {
            anchors.fill: parent
            interactive: false
            currentIndex: control.currentView == CFMainScreenView.Favorite ? 1 : 0
            spacing: CFStyle.spacing * 2

            CFProfileList {
                id: userListView

                function setUsersViewed(index) {
                    if (index > userManager.model.lastViewedIndex){
                        userManager.model.lastViewedIndex = index
                    }
                }


                containsMouse: hoverMouseArea.containsMouse
                model: userManager.model
                onAtYEndChanged: atYEnd ? setUsersViewed(count - 1) : null
                onContentYChanged: {
                    var index = indexAt(1, contentY) - 1
                    setUsersViewed(index)
                }

                CFColumn {
                    id: placeholderColumn

                    property bool noUsersCauseOfFilters: false

                    function updatePlaceholder() {
                        var count = userManager.model.usersCountByFilterSettings(-1, 0, [], false, 0, 0,
                                                                                 [], [], false, [], [],
                                                                                 [], [], [], []);

                        noUsersCauseOfFilters = count !== 0 && !userManager.model.viewedCount;
                    }

                    anchors.centerIn: parent
                    visible: !parent.count
                    spacing: CFStyle.spacing

                    onVisibleChanged: if (visible) updatePlaceholder();

                    Connections {
                        target: userManager.model

                        onSelectFinished: placeholderColumn.updatePlaceholder();
                    }

                    CFImage {
                        anchors.horizontalCenter: parent.horizontalCenter

                        source: "qrc:/icons/Icon_person.svg"
                        sourceSize: Qt.size(96, 96)
                    }

                    CFLabel {
                        text: placeholderColumn.noUsersCauseOfFilters ? qsTr("Подходящих людей нет") : qsTr("Список единомышленников пуст")
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    CFLabel {
                        text: placeholderColumn.noUsersCauseOfFilters ?
                                  qsTr("Попробуйте изменить параметры поиска или загрузить\nбольше подписок") :
                                  qsTr("Пожалуйста настройте интересы и список желательных\nи нежелательных подписок")
                        color: CFStyle.black05
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 20
                        lineHeightMode: Text.FixedHeight
                    }

                    CFButton {
                        text: qsTr("Настройка подписок")
                        visible: !userManager.model.viewedCount && !placeholderColumn.noUsersCauseOfFilters
                        width: 170
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: appWindow.currentView = CFMainScreenView.Groups
                    }

                    CFButton {
                        anchors.horizontalCenter: parent.horizontalCenter
                        visible: userManager.model.viewedCount && !placeholderColumn.noUsersCauseOfFilters
                        width: 258
                        text: qsTr("Показать просмотренные (%1)").arg(utils.viewedToString(userManager.model.viewedCount))
                        onClicked: userManager.model.showViewed = !userManager.model.showViewed
                    }

                }

                CFViewedButton {
                    id: viewedButton

                    visible: userManager.model.count && currentView === CFMainScreenView.Search && !(smallResolution && openedWebBrowser) && !greater

                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        leftMargin: 20
                    }
                }

                Connections {
                    target: userManager.model
                    onAskResetPosition: userListView.positionViewAtBeginning()
                    onCountChanged: userListView.count < 6 ? userListView.setUsersViewed(userListView.count - 1) : null
                }
            }

            CFProfileList {
                id: favoriteListView

                containsMouse: hoverMouseArea.containsMouse
                model: userManager.favoriteModel

                CFColumn {
                    anchors.centerIn: parent
                    visible: !parent.count
                    spacing: CFStyle.spacing

                    CFImage {
                        anchors.horizontalCenter: parent.horizontalCenter

                        source: "qrc:/icons/Icon_star_96.svg"
                        sourceSize: Qt.size(96, 96)
                    }

                    CFLabel {
                        text: qsTr("Список избранных пуст")
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    CFLabel {
                        text: qsTr("Нажмите на кнопку \"Добавить в избранные\", чтобы\nдобавить человека в список")
                        color: CFStyle.black05
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 20
                        lineHeightMode: Text.FixedHeight
                    }
                }

                Connections {
                    target: userManager.favoriteModel
                    onAskResetPosition: favoriteListView.positionViewAtBeginning()
                }
            }
        }
    }
}
