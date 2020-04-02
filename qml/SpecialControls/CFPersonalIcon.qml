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

IconLabel {
    property var personalList
    property int value

    property string toolTipText: ""

    display:  AbstractButton.IconOnly
    visible: (userManager.model.political.length === 0 // If no personal filters are set than show all icons
              && userManager.model.religion.length === 0
              && userManager.model.lifeMain.length === 0
              && userManager.model.peopleMain.length === 0
              && userManager.model.smoking.length === 0
              && userManager.model.alcohol.length === 0) || personalList.length > 0
    icon.width: 24
    icon.height: 24
    icon.color: {
        if (value === CFConstants.invalidValue || value === CFPersonalPolitical.Undefined) {
            return CFStyle.blueLight
        }

        switch(userManager.model.checkPersonal(personalList, value)) {
        case 1: CFStyle.colorPrimary; break;
        case 0: CFStyle.blueLight; break;
        case -1: CFStyle.red; break;
        }
    }

    MouseArea {
        anchors.fill: parent
        anchors.margins: -1


        hoverEnabled: true

        CFToolTip {
            visible: parent.containsMouse && toolTipText != ""
            text: toolTipText
        }
    }
}
