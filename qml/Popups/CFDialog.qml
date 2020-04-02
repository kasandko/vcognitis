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

import "qrc:/qml/Singletons"
import "qrc:/qml/SimpleControls"

Dialog {
    padding: 0
    topPadding: 0

    header: Item {
        implicitHeight: CFStyle.dialogHeaderHeight

        CFPane {
            anchors.fill: parent

            Item {
                anchors.fill: parent

                CFLabel {
                    text: title

                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 18
                    font.weight: Font.DemiBold
                }

                CFRoundButton {
                    flat: true

                    anchors {
                        right: parent.right
                        rightMargin: -((width - icon.width) / 2)
                        verticalCenter: parent.verticalCenter
                    }

                    icon {
                        source: "qrc:/icons/close.svg"
                        width: 24
                        height: 24
                    }

                    onClicked: reject()
                }
            }
        }

        CFHDivider {
            width: parent.width

            anchors {
                bottom: parent.bottom
                bottomMargin: -height
            }
        }
    }
}
