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

import ConfederateFinder 1.0

import "../Singletons"
import "../SimpleControls"
import "../SpecialControls"

CFButton {
    id: changeModesButton

    property bool waitAnimation: false

    text: utils.appMode == CFAppMode.Quick ? qsTr("Быстрый режим") : qsTr("Полный режим")
    flat: true
    Material.background:  "transparent"
    
    contentItem: CFRow {
        spacing: 5

        CFImage {
            anchors.verticalCenter: parent.verticalCenter
            sourceSize: Qt.size(24, 24)
            source: "qrc:/icons/Icon_accurateMode.svg"
            opacity: utils.appMode != CFAppMode.Quick && enabled ? 1 : 0.4;
        }

        CFLabel {
            anchors.verticalCenter: parent.verticalCenter
            width: 110
            text: changeModesButton.text
            horizontalAlignment: Text.AlignHCenter
            color: CFStyle.white
            opacity: enabled ? 1 : 0.4

            CFAnimatedDots {anchors.centerIn: parent; visible: waitAnimation}
        }

        CFImage {
            anchors.verticalCenter: parent.verticalCenter
            sourceSize: Qt.size(24, 24)
            source: "qrc:/icons/Icon_quickMode.svg"
            opacity: utils.appMode == CFAppMode.Quick && enabled ? 1 : 0.4;
        }

    }

    backgroundRect.border.width: enabled ? 1 : 0
    backgroundRect.border.color: "#809FC2"

    onPressed: {
        text = ""
        waitAnimation = true
    }
    
    onClicked: {
        let error = false;
        if (userManager.downloadQueue.count) {
            globalErrorToast.error(qsTr("Изменение режима во время загрузки подписчиков запрещено"), qsTr("Пожалуйста, дождитесь окончания операции"));
            error = true;
        }
        
        if (userManager.score) {
            globalErrorToast.error(qsTr("Изменение режима во время применения фильтров запрещено"), qsTr("Пожалуйста, дождитесь окончания операции"));
            error = true;
        }
        
        if (userManager.rescoringActive) {
            globalErrorToast.error(qsTr("Изменение режима во время перерасчета рейтинга запрещено"), qsTr("Пожалуйста, дождитесь окончания операции"));
            error = true;
        }
        
        if (groupManager.groupModel.dirtyStatus || groupManager.interestsDirty) {
            unsavedChangesDialog.changeMode();
            error = true;
        }
        
        if (!error) {
            if (utils.appMode === CFAppMode.Accurate) {
                utils.appMode = CFAppMode.Quick;
            } else {
                utils.appMode = CFAppMode.Accurate;
            }
        }

        text = Qt.binding(function() {
            return utils.appMode == CFAppMode.Quick ? qsTr("Быстрый режим") : qsTr("Полный режим");
        })
        waitAnimation = false;
    }
}
