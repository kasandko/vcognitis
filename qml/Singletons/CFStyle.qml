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

pragma Singleton

import QtQuick 2.0

Item {
    readonly property int padding: 20
    readonly property int spacing: 20

    readonly property int mainHeaderHeight: 50
    readonly property int dialogHeaderHeight: 60
    readonly property int settingsLeftSectionWidth: 185
    readonly property int controlHeight: 30
    readonly property int roundButtonSize: 40

    readonly property color errorToastColor: "#FFF5F6"
    readonly property color errorRed: "#FF0000"

    readonly property color dividerColor: grey
    readonly property color aboutButtonColor: "#7D7D7D"

    readonly property color colorPrimary: "#4A76A8"

    readonly property color backgroundPrimay: "white"
    readonly property color backgroundSecondary: grey

    readonly property color buttonColor: "#5181B8"
    readonly property color buttonColorSaturated: "#2f7fda"

    readonly property color white: "#FFFFFF"
    readonly property color white08: setColorAlpha(white, 0.8)
    readonly property color white05: setColorAlpha(white, 0.5)
    readonly property color white04: setColorAlpha(white, 0.4)
    readonly property color white01: setColorAlpha(white, 0.1)
    readonly property color black: "black"
    readonly property color black08: setColorAlpha("black", 0.8)
    readonly property color black06: setColorAlpha("black", 0.6)
    readonly property color black05: setColorAlpha("black", 0.5)
    readonly property color black03: setColorAlpha("black", 0.3)


    readonly property color greyScrollBar: "#F2F2F2"
    readonly property color greyScrollBarButtonPressed: "#ACACAC"
    readonly property color greyScrollBarButtonHovered: "#C1C1C1"
    readonly property color greyGroupDelegate: "#FCFDFD"
    readonly property color greyWhite: "#FAFBFC"
    readonly property color grey: "#EDEEF0"
    readonly property color grey5: "#E0E0E0"
    readonly property color lightGrey: "#F9F9F9"
    readonly property color blue: "#2A5885"
    readonly property color blue08: setColorAlpha(blue, 0.8)
    readonly property color blueLight: "#E5EBF1"
    readonly property color blueGray: "#98B1C6"
    readonly property color blueMain: "#4A76A8"
    readonly property color blueTeachToolTip: "#679DFD"
    readonly property color green: "#4BB34B"
    readonly property color red: "#FF3347"
    readonly property color red005: setColorAlpha(red, 0.05)

    // create new color by adjusting alpha channel of existing color
    function setColorAlpha(color, alpha) {
        // Color is transparent then do nothing with it
        if (color.a === 0) {
            return color;
        }

        return Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, alpha)
    }
}
