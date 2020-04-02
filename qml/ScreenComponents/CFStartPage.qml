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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import ConfederateFinder 1.0

import "../Singletons"
import "../SimpleControls"
import "../SpecialControls"

Rectangle {
    id: control

    // Time in secs
    property int automaticTimeMin: 10 * 60
    property int automaticTimeMax: groupManager.maxTimeToDownloadAllMembers + Math.min(groupManager.maxTimeToDownloadAllMembers * 20 / 100, 10 * 60)
    property int automaticTimeRange: Math.max(automaticTimeMax - automaticTimeMin, 0)

    signal clicked();

    color: CFStyle.grey

    CFImage {
        source: "qrc:/images/greaterBackground.jpg"
        anchors.centerIn: parent
        sourceSize: Qt.size(2234, 1523)
        opacity: 0.25
    }

    MouseArea {
        anchors.fill: parent
    }

    CFClickableLogo {
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: CFStyle.spacing
        }
    }

    RowLayout {
        visible: !(groupManager.loopedChunkDownloader || userManager.rescoringActive || userManager.score)
        anchors.centerIn: parent
        spacing: CFStyle.padding

        CFPane {
            id: autoPane

            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 400
            Layout.preferredHeight: 431

            CFColumn {
                width: parent.width
                spacing: CFStyle.spacing

                CFLabel {
                    text: qsTr("Автоматический режим")
                    font.pixelSize: 24
                    anchors.left: parent.left
                }

                CFHDivider {
                    anchors.left: parent.left
                    anchors.leftMargin: -CFStyle.padding
                    width: parent.width + 2 * CFStyle.padding
                }

                CFBulletedList {
                    width: parent.width
                    title: qsTr("Плюсы:")

                    model: [
                        qsTr("Максимально быстрый и простой")
                    ]
                }

                CFBulletedList {
                    width: parent.width
                    title: qsTr("Минусы:")

                    model: [
                        qsTr("Учитываются только группы, на которые вы подписаны"),
                        qsTr("Нельзя указать группы, которые вам не нравятся"),
                        qsTr("Интересы запрашиваются отдельно")
                    ]
                }
            }

            CFColumn {
                width: parent.width
                anchors.bottom: parent.bottom
                spacing: CFStyle.spacing

                CFHDivider {
                    anchors.left: parent.left
                    anchors.leftMargin: -CFStyle.padding
                    width: parent.width + 2 * CFStyle.padding
                }

                CFColumn {
                    width: parent.width
                    spacing: CFStyle.padding / 2

                    CFLabel {
                        id: timeLabel

                        font.pixelSize: 18
                        font.weight: Font.Medium
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: {
                            var roundedValue = Math.round(automaticTimeRange * timeSlider.value + automaticTimeMin);
                            var resultValue = Math.min(roundedValue, automaticTimeMax);

                            return ( "~" + utils.secs2TimeString(resultValue) );
                        }
                    }

                    Item {
                        id: timeSliderBlock

                        width: parent.width
                        height: 42
                        opacity: 1
                        visible: opacity

                        Behavior on opacity {
                            NumberAnimation { duration: 2000; easing.type: Easing.InOutQuad }
                        }


                        CFLabel {
                            id: leftLabel

                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Так себе")
                            anchors.left: parent.left
                            color: CFStyle.black05
                        }

                        CFSlider {
                            id: timeSlider

                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: leftLabel.right
                            anchors.right: rightLabel.left
                            anchors.margins: CFStyle.padding
                            value: 0.5
                        }

                        CFLabel {
                            id: rightLabel

                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Замечательно")
                            anchors.right: parent.right
                            color: CFStyle.black05
                        }
                    }
                }

                CFButton {
                    width: 180
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Просто сделать")

                    onClicked: {
                        if (automaticTimeMax > automaticTimeMin) {
                            groupManager.startAutomatic(Math.round(automaticTimeRange * timeSlider.value + automaticTimeMin));
                        } else {
                            groupManager.startAutomatic(automaticTimeMax);
                        }
                    }
                }
            }
        }

        CFPane {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredHeight: autoPane.height
            Layout.preferredWidth: 620
            bottomPadding: 0

            CFColumn {
                width: parent.width
                height: parent.height
                spacing: CFStyle.spacing

                CFLabel {
                    id: titleLabel

                    text: qsTr("Ручной режим")
                    font.pixelSize: 24
                }

                CFHDivider {
                    anchors.left: parent.left
                    anchors.leftMargin: -CFStyle.padding
                    width: parent.width + 2 * CFStyle.padding
                }

                CFRow {
                    spacing: CFStyle.spacing
                    width: parent.width
                    height: parent.height - titleLabel.height - CFStyle.padding * 2 - 1

                    Item {
                        width: parent.width / 2 - CFStyle.spacing
                        height: parent.height

                        CFColumn {
                            spacing: CFStyle.spacing * 1.5
                            anchors.fill: parent

                            CFImage {
                                source: "qrc:/icons/Icon_quickMode64.svg"

                                sourceSize: Qt.size(64, 64)
                            }

                            CFLabel {
                                font.weight: Font.Medium
                                font.pixelSize: 18

                                text: qsTr("Быстрый")
                            }

                            CFBulletedList {
                                width: parent.width
                                title: qsTr("Плюсы:")

                                model: [
                                    qsTr("Максимальная скорость")
                                ]
                            }

                            CFBulletedList {
                                width: parent.width
                                title: qsTr("Минусы:")

                                model: [
                                    qsTr("Интересы запрашиваются отдельно")
                                ]
                            }
                        }

                        CFButton {
                            width: 150
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: CFStyle.padding
                            text: qsTr("Начать")

                            onClicked: {
                                utils.appMode = CFAppMode.Quick;
                                currentView = CFMainScreenView.Groups;
                                control.visible = false;
                            }
                        }

                    }

                    CFVDivider {
                        height: parent.height + CFStyle.padding
                        anchors.top: parent.top
                        anchors.topMargin: -CFStyle.padding
                    }

                    Item {
                        width: parent.width / 2 - CFStyle.spacing
                        height: parent.height

                        CFColumn {
                            spacing: CFStyle.spacing * 1.5
                            anchors.fill: parent

                            CFImage {
                                source: "qrc:/icons/fullMode64.svg"

                                sourceSize: Qt.size(64, 64)
                            }

                            CFLabel {
                                font.weight: Font.Medium
                                font.pixelSize: 18

                                text: qsTr("Полный")
                            }

                            CFBulletedList {
                                width: parent.width
                                title: qsTr("Плюсы:")

                                model: [
                                    qsTr("Максимальная точность")
                                ]
                            }

                            CFBulletedList {
                                width: parent.width
                                title: qsTr("Минусы:")

                                model: [
                                    qsTr("Скорость в 1000 раз меньше")
                                ]
                            }
                        }

                        CFButton {
                            width: 150
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: CFStyle.padding
                            text: qsTr("Вперёд")

                            onClicked: {
                                utils.appMode = CFAppMode.Accurate;
                                currentView = CFMainScreenView.Groups;
                                control.visible = false;
                            }
                        }
                    }
                }
            }
        }
    }

    CFPane {
        id: autoProgresPane

        anchors.centerIn: parent
        width: smallResolution ? 400 : 600
        visible: groupManager.loopedChunkDownloader || userManager.rescoringActive || userManager.score

        CFColumn {
            width: parent.width
            spacing: CFStyle.spacing

            CFLabel {
                text: qsTr("Автоматический режим")
                font.pixelSize: 24
                anchors.left: parent.left
            }

            CFImage {
                width: parent.width
                height: width * 0.4578754579
                source: "qrc:/images/Image_automaticProgressMode.svg"
                sourceSize: Qt.size(546, 250)
            }

            CFLabel {
                font.pixelSize: 18
                color: CFStyle.black08
                text: {
                    var time = 0;

                    if (groupManager.loopedChunkDownloader)
                        time = groupManager.loopedChunkDownloader.remainingTimeInSecs + groupManager.allottedTimeForTopAnalysys();
                    if (userManager.rescoringActive)
                        return ""; // We can't predict remaining time here
                    if (userManager.score)
                        time = userManager.score.estimatedDuration;

                    return "~" + utils.secs2TimeString(time);
                }
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                width: parent.width
                height: CFStyle.controlHeight
                border.width: 1
                border.color: CFStyle.blueGray
                color: "transparent"
                radius: 2

                CFProgressFill {
                    radius: parent.radius

                    anchors.fill: parent
                    anchors.margins: 2

                    progress: {
                        if (groupManager.loopedChunkDownloader)
                            return groupManager.loopedChunkDownloader.progress;
                        if (userManager.rescoringActive)
                            return userManager.rescoringPercent
                        if (userManager.score)
                            return userManager.score.progress
                        return 0;
                    }

                    CFLabel {
                        text: {
                            if (groupManager.loopedChunkDownloader)
                                return qsTr("Загрузка подписчиков (%1\%)").arg(Math.round(groupManager.loopedChunkDownloader.progress * 100))
                            if (userManager.rescoringActive)
                                return qsTr("Расчет рейтинга (%1\%)").arg(Math.round(userManager.rescoringPercent * 100))
                            if (userManager.score)
                                return qsTr("Уточнение показателей (%1\%)").arg(Math.round(userManager.score.progress * 100))
                            return ""
                        }

                        leftPadding: CFStyle.padding * 0.5
                        verticalAlignment: Label.AlignVCenter
                        anchors.fill: parent
                        font.weight: Font.Medium
                    }
                }
            }

        }
    }

//    Timer {
//        interval: 2000
//        running: true
//        onTriggered: {
//            timeSliderBlock.opacity = 0;
//        }
//    }

    onAutomaticTimeMaxChanged: {
        timeSliderBlock.opacity = (automaticTimeMax > automaticTimeMin) ? 1 : 0;
    }

    Connections {
        target: groupManager
        onAutomaticFinished: {
            utils.appMode = CFAppMode.Quick;
            currentView = CFMainScreenView.Search;
            control.visible = false;
        }
    }
}
