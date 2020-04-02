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

import QtQuick 2.9
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import ConfederateFinder 1.0

import "../Singletons"
import "../SimpleControls"

Item {
    id: control

    property string url
    property string name
    property int weight
    property bool desired: true
    property bool error: false
    property int queueIndex: CFConstants.invalidIndex // Group's index in download queue
    property int relativeIndex: queueIndex - userManager.downloadQueue.currentIndex

    property int nameFieldWidth: 330
    property int urlFieldWidth: 140

    signal updateUrl(string url)
    signal updateWeight(int weight)
    signal removeGroup()

    implicitHeight: 60

    onUrlChanged: if (!urlTextField.focus) urlTextField.text = url

    onErrorChanged: urlTextField.error = error

    Rectangle {
        anchors.fill: parent
        visible: downloaded || subHidden
        color: CFStyle.greyGroupDelegate

        Rectangle {
            color: CFStyle.buttonColor
            width: 2
            height: parent.height
        }
    }

    Item {
        id: downloadButtonItem

        height: parent.height
        width: downloadButton.implicitWidth

        anchors {
            left: parent.left
            leftMargin: CFStyle.padding * 0.5
        }

        CFRoundButton {
            id: downloadButton

            flat: true
            enabled: model.validated && !userManager.scoring && !(downloaded || subHidden)

            Material.foreground: CFStyle.colorPrimary

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            visible: queueIndex === CFConstants.invalidIndex || relativeIndex < 0

            icon {
                source: {
                    if (subHidden) {
                        return "qrc:/icons/Icon_block.svg";
                    }

                    if (downloaded)
                        return "qrc:/icons/mdi_done.svg"

                    return "qrc:/icons/Icon_dowload.svg"
                }
                width: 24
                height: 24
                color: enabled ? CFStyle.buttonColor : CFStyle.setColorAlpha(CFStyle.buttonColor, 0.5)
            }

            onClicked: {
                userManager.downloadQueue.add(uid);
                queueIndex = userManager.downloadQueue.count - 1;
            }

            CFToolTip {
                visible: parent && parent.hovered

                // Dirty hack to correct work with K or M suffix

                text: qsTr(" %n подписчик(ов)", "", model.subCount >= 1000 ? 5/*any plural*/: model.subCount).arg(utils.subsToString(subCount))
            }
        }

        Rectangle {
            anchors.centerIn: parent

            color: CFStyle.buttonColor
            width: 24
            height: width
            radius: width / 2

            visible: queueIndex !== CFConstants.invalidIndex && relativeIndex >= 0

            CFLabel {
                anchors.centerIn: parent

                font.weight: Font.DemiBold
                text: relativeIndex + 1
                color: "white"
                lineHeight: 16
                lineHeightMode: Text.FixedHeight
            }
        }
    }

    CFLabel {
        id: nameTextField


        anchors.left: downloadButtonItem.right
        anchors.leftMargin: CFStyle.padding * 0.25
        anchors.verticalCenter: parent.verticalCenter

        width: nameFieldWidth
        elide: Text.ElideRight

        color: name ? enabled ? CFStyle.black08 : CFStyle.black05 : CFStyle.black03

        text: name ? name :  qsTr("Появится автоматически")
    }

    Item {
        id: urlTextFieldItem

        width: urlFieldWidth
        height: parent.height

        anchors {
            left: nameTextField.right
            leftMargin: CFStyle.padding * 0.75
        }

        CFTextFieldWithProgress {
            id: urlTextField

            withContextMenu: true

            backgroundColor: (downloaded || subHidden) ? CFStyle.greyGroupDelegate : CFStyle.backgroundPrimay
            inProgress: queueIndex !== CFConstants.invalidIndex && relativeIndex === 0
            placeholderText: qsTr("Введите URL")
            progress: (!(downloaded || subHidden) && offset) ? model.progress : 0
            width: urlFieldWidth
            readOnly: uid != CFConstants.invalidValue
            activeFocusOnPress: !readOnly
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: (queueIndex === CFConstants.invalidIndex) || relativeIndex <= 0

            onTextChanged: {
                error = control.error = false;
                if (urlTextField.focus) {
                    updateUrl(text);
                }
            }

            onInProgressChanged: {
                if (inProgress) {
                    text = Qt.binding(function() {
                        return qsTr("Загрузка %1\%").arg(Math.round(progress * 100));
                    });
                } else {
                    text = url;
                }
            }

            Component.onCompleted:  {
                if (text == "") {
                    urlTextField.focus = true
                    urlTextField.forceActiveFocus();
                }
            }

            CFRoundButton {
                width: height
                height: parent.height
                anchors.right: parent.right
                icon.source: "qrc:/icons/Icon_stop.svg"
                icon.width: 24
                icon.height: 24
                topPadding: (height - icon.height) / 2
                bottomPadding: topPadding
                leftPadding: (width - icon.width) / 2
                rightPadding: leftPadding
                flat: true
                Material.foreground: CFStyle.blueGray
                visible: queueIndex !== CFConstants.invalidIndex && relativeIndex === 0

                onClicked: {
                    userManager.downloadQueue.remove(uid);
                    queueIndex = CFConstants.invalidIndex;
                }
            }
        }

        CFLabel {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            color: CFStyle.colorPrimary
            text: qsTr("Убрать из очереди")
            visible: relativeIndex > 0

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    userManager.downloadQueue.remove(uid);
                    queueIndex = CFConstants.invalidIndex;
                }
            }
        }
    }



    CFScoreSelector {
        id: scoreSelector

        anchors.left: urlTextFieldItem.right
        anchors.leftMargin: CFStyle.padding * 0.5
        anchors.verticalCenter: parent.verticalCenter

        desired: control.desired
        weight: Math.abs(control.weight)
        enabled: !userManager.downloadQueue.count


        onUpdateWeight: control.updateWeight(weight * (desired ? 1 : -1))
    }

    CFRoundButton {
        id: removeButton

        flat: true
        enabled: !userManager.downloadQueue.count

        Material.foreground: CFStyle.blueGray

        anchors {
            right: parent.right
            rightMargin: CFStyle.padding * 0.35
            verticalCenter: parent.verticalCenter
        }

        icon {
            source: "qrc:/icons/closeDelegate.svg"
            width: 14
            height: 14
        }

        onClicked: removeGroup()

        CFToolTip {
            visible: parent.hovered
            text: qsTr("Удалить подписку")
        }
    }

    CFHDivider { width: parent.width; anchors.bottom: parent.bottom }

    Connections {
        target: groupManager

        enabled: !control.error && !model.validated

        onModelValidityAssertionFinished: {
            if (!error)
                model.validated = true;
        }
    }

    Connections {
        target: userManager.downloadQueue
        onCancelled: queueIndex = CFConstants.invalidIndex;
        onCountChanged: queueIndex = userManager.downloadQueue.indexOf(uid)
    }

    Component.onCompleted: {

        // We can't force focus from here(maybe something steal focus)
        if (model.uid === -1 && model.index === 0)
            focusTimer.start();
    }

    // Used only for force focus on url field.
    Timer {
        id: focusTimer

        interval: 1

        onTriggered: {
            urlTextField.forceActiveFocus()
        }
    }
}
