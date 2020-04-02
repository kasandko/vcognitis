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
import ConfederateFinder 1.0

import "../Singletons"
import "../SimpleControls"

CFButton {
    id: control

    text: qsTr("Параметры поиска")

    flat: true
    Material.background: enabled ? CFStyle.white01 : "transparent"

    icon.source: "qrc:/icons/icon_tune.svg"

    icon.color: !enabled ? CFStyle.white05 :
            flat && highlighted ? Material.accentColor :
            highlighted ? Material.primaryHighlightedTextColor : Material.foreground

    contentIconLabel.color: !control.enabled ? CFStyle.white05 :
            control.flat && control.highlighted ? control.Material.accentColor :
            control.highlighted ? control.Material.primaryHighlightedTextColor : control.Material.foreground

    onClicked: {
        if(filterDialog.visible) {
            filterDialog.close();
        } else {
            if (userManager.downloadQueue.count) {
                globalErrorToast.error(qsTr("Применение фильтров во время загрузки подписчиков запрещено"), qsTr("Пожалуйста, дождитесь окончания операции"));
                return;
            }

            if (userManager.score) {
                globalErrorToast.error(qsTr("Изменение фильтров во время во время их пременения запрещено"), qsTr("Пожалуйста, дождитесь окончания операции"));
                return;
            }

            if (userManager.rescoringActive) {
                globalErrorToast.error(qsTr("Изменение фильтров во время перерасчета рейтинга запрещено"), qsTr("Пожалуйста, дождитесь окончания операции"));
                return;
            }

            if (groupManager.groupModel.dirtyStatus || groupManager.interestsDirty) {
                return unsavedChangesDialog.changeView(CFMainScreenView.Search, true)
            }

            currentView = CFMainScreenView.Search;
            filterDialog.open();
        }
    }
}
