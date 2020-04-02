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

import "../Singletons"
import "../SimpleControls"

CFPopup {
    id: control

    title: qsTr("Журнал ошибок")

    width: 600
    height: 500

    padding: CFStyle.padding
    modal: true

    MouseArea {
        id: hoverMouseArea

        anchors.fill: parent
        hoverEnabled: true

        CFListView {
            id: listView

            anchors.fill: parent
            spacing: CFStyle.spacing * 0.7
            model: errorModel
            delegate: Item {
                width: listView.width
                implicitWidth: delegateLabel.implicitWidth
                implicitHeight: delegateLabel.implicitHeight

                CFLabel {
                    id: delegateLabel

                    width: parent.width

                    lineHeight: 20
                    lineHeightMode: Text.FixedHeight
                    wrapMode: Text.Wrap

                    text: "<font color=\"%1\">[%2]</font> %3. %4".arg(CFStyle.red)
                                                                 .arg(timestamp.toLocaleTimeString(
                                                                          Qt.locale(), "hh.mm"
                                                                  ))
                                                                 .arg(source)
                                                                 .arg(userfriendlyMessage)
                }

                CFToolTip {
                    id: toolTip

                    visible: mouseArea.containsMouse
                    text: technicalMessage
                    width: parent.width

                    // To set wrapping
                    contentItem: CFLabel {
                        text: toolTip.text
                        font: toolTip.font
                        color: CFStyle.white
                        wrapMode: Label.Wrap
                    }
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            ScrollBar.vertical: CFScrollBar { targetContainsMouse: hoverMouseArea.containsMouse }

            CFLabel {
                anchors.centerIn: parent
                color: CFStyle.black05
                text: qsTr("Ошибок нет")
                visible: !listView.count
            }
        }
    }
}
