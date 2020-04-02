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
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtQuick.Layouts 1.12

import "../Singletons"
import "../SimpleControls"

Popup {
    id: control

    width: 400
    modal: true
    padding: 0

    contentItem: Item {
        implicitHeight: contentColumn.implicitHeight
        implicitWidth: contentColumn.implicitWidth

        ColumnLayout {
            id: contentColumn

            spacing: 0

            height: parent.height
            width: parent.width

            CFColumn {
                spacing: CFStyle.spacing * 1.5
                padding: CFStyle.padding

                Layout.fillWidth: true

                CFRow {
                    spacing: CFStyle.spacing

                    CFImage {
                        source: "qrc:/icons/vcognitis.svg"
                        sourceSize: Qt.size(48, 48)
                    }

                    CFLabel {
                        id: vcognitisLabel

                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("VCognitis")
                        font.pixelSize: 36
                    }
                }

                CFColumn {
                    spacing: CFStyle.spacing * 0.9

                    CFLabel {
                        text: qsTr("Version %1 (commit %2)").arg(utils.appVersion).arg(utils.appCommit)
                    }

                    CFLabel {
                        text: qsTr("Barbatum™ %1").arg((new Date()).getFullYear())
                    }
                }
            }

            CFHDivider { Layout.fillWidth: true }

            CFColumn {
                spacing: CFStyle.padding * 1.4
                padding: CFStyle.padding

                Layout.fillWidth: true

                CFClickableLabel {
                    text: qsTr("Icons provided by Google Material Design")
                    onClicked: utils.openUrlInDesktopBrowser("https://material.io/resources/icons/?style=baseline")
                }

                CFClickableLabel {
                    text: qsTr("Font provided by Google Fonts")
                    onClicked: utils.openUrlInDesktopBrowser("https://fonts.google.com/")
                }

                CFColumn {
                    spacing: CFStyle.padding * 0.2

                    CFClickableLabel {
                        text: qsTr("Photo by Christian Perner on Unsplash")
                        onClicked: utils.openUrlInDesktopBrowser("https://unsplash.com/@christianperner")
                    }

                    CFClickableLabel {
                        text: qsTr("Photo by Toa Heftiba on Unsplash")
                        onClicked: utils.openUrlInDesktopBrowser("https://unsplash.com/@heftiba")
                    }
                }
            }

            CFHDivider { Layout.fillWidth: true }

            Item {
                Layout.fillWidth: true

                implicitHeight: siteButton.implicitHeight + CFStyle.padding * 2

                CFButton {
                    id: siteButton

                    text: qsTr("Сайт")
                    width: 150
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: CFStyle.padding
                    onClicked: utils.openUrlInDesktopBrowser("http://barbatum.ru")
                }
            }
        }

        CFRoundButton {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: CFStyle.padding
            anchors.topMargin: CFStyle.padding + (vcognitisLabel.height - height) / 2
            icon.source: "qrc:/icons/closePopup.svg"
            onClicked: control.close()
            flat: true
        }
    }
}
