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

import "../SimpleControls"
import "../SpecialControls"
import "../Singletons"

CFPopup {
    id: popup

    property bool placeholderActive: false

    title: qsTr("Полный анализ для лучших")
    width: 470
    height:492
    modal: true
    closePolicy: CFPopup.NoAutoClose

    onOpened: {
        timeLabel.timeAnalyzeInProgress = true;
        placeholderActive = true;
        userManager.topAnalyzeTimeScoreQml(120); // 2min
    }

    CFColumn {
        spacing: CFStyle.spacing
        anchors.fill: parent
        padding: CFStyle.padding * 0.75

        CFImage {
            source: "qrc:/icons/adjust96.svg"
            sourceSize: Qt.size(96, 96)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CFLabel {
            text: qsTr("• Можно узнать интересы пользователей\n• Учесть даже не скачанные группы\n• Исключить совпадения, найденные слишком низко")
            color: CFStyle.black05
        }

        CFColumn {
            width: parent.width - CFStyle.padding * 1.5
            spacing: CFStyle.spacing * 0.75

            CFLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Анализировать с рейтингом от:")
                font.weight: Font.Medium
                horizontalAlignment: CFLabel.AlignHCenter
            }

            CFProgressFill {
                visible: placeholderActive
                radius: 4
                indeterminate: true
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
            }

            CFRow {
                visible: !placeholderActive
                spacing: CFStyle.spacing / 2
                anchors.horizontalCenter: parent.horizontalCenter

                CFSlider {
                    id: scoreSlider

                    anchors.verticalCenter: parent.verticalCenter
                    from: 5
                    stepSize: 1
                    to: 100
                    width: 310



                    onMoved: {
                        timeLabel.timeAnalyzeInProgress = true;
                        userManager.topAnalyzeTime(value);
                    }
                }

                CFLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    text: scoreSlider.value
                    font.pixelSize: 18
                    font.weight: Font.Medium
                }
            }

            CFLabel {
                id: timeLabel

                property int seconds: 0
                property bool timeAnalyzeInProgress: false

                anchors.horizontalCenter: parent.horizontalCenter
                text: timeAnalyzeInProgress ? qsTr("Идет расчет...") : qsTr("Это займет %1").arg(utils.secs2TimeString(seconds))
                color: CFStyle.black05
            }

            Connections {
                target: userManager

                onTopAnalyzeTimeFinished: {
                    timeLabel.timeAnalyzeInProgress = false;
                    timeLabel.seconds = time;
                }
                onTopAnalyzeTimeScoreFinished: {
                    timeLabel.timeAnalyzeInProgress = false;
                    placeholderActive = false;
                    timeLabel.seconds = time;
                    scoreSlider.value = score;
                }
            }
        }
    }

    CFHDivider {
        width: parent.width + CFStyle.spacing * 2

        anchors.left: parent.left
        anchors.leftMargin: -CFStyle.spacing
        anchors.bottom: buttonRow.top
        anchors.bottomMargin: CFStyle.spacing
    }

    CFRow {
        id: buttonRow

        spacing: CFStyle.spacing
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        CFButton {
            text: qsTr("Пропустить")
            Material.foreground: CFStyle.blue
            Material.background: CFStyle.blueLight
            width: 180

            onClicked: close()
        }

        CFButton {
            text: qsTr("Уточнить")
            width: 180

            enabled: !placeholderActive

            onClicked: {
                userManager.topAnalyze(scoreSlider.value)
                close();
            }
        }
    }
}
