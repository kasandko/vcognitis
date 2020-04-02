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
import QtWebEngine 1.8

import "../SimpleControls"
import "../Singletons"

WebEngineView {
    url: "https://vk.com/id0"

    // Needed to return user back to where he was at after authorization
    property string prevUrl

    // True if current location is vk
    property bool isVk: url.toString().search("https://vk.com") === 0


    function authorise() {
        // If current url is looks like logout url so remember user page url
        prevUrl = url;
        if (prevUrl.includes("https://vk.com/login") || prevUrl == "https://vk.com/")
            prevUrl = "https://vk.com/id0";

        url = vkAuthManager.authUrl();
    }

    Rectangle {
        id: failedStatusPlaceHolder

        property string failedUrl: ""

        z: 1
        anchors.fill: parent
        visible: true

        CFColumn {
           anchors.centerIn: parent
           spacing: CFStyle.padding

           CFImage {
               source: "qrc:/icons/Icon_error.svg"
               sourceSize: Qt.size(96, 96)

               anchors.horizontalCenter: parent.horizontalCenter
           }

           CFLabel {
               text: qsTr("Не удаётся установить соединение с сайтом")
               color: CFStyle.black08
               anchors.horizontalCenter: parent.horizontalCenter
               font.pixelSize: 18
               font.weight: Font.DemiBold
           }

           CFLabel {
               text: qsTr("Страница по адресу <span style=\"font-weight: 500;\">%1</span> может быть временно недоступна. Попробуйте позже.")
                     .arg(failedStatusPlaceHolder.failedUrl)
               color: CFStyle.black05
               width: 400
               textFormat: Text.RichText
               lineHeight: 20
               lineHeightMode: Text.FixedHeight
               anchors.horizontalCenter: parent.horizontalCenter
               horizontalAlignment: CFLabel.AlignHCenter
               wrapMode: Text.Wrap
           }
        }
    }

    onNewViewRequested: url = request.requestedUrl

    userScripts: [
        WebEngineScript {
            injectionPoint: WebEngineScript.DocumentReady
            sourceCode: "
            var styleElement = document.createElement(\"style\");
            styleElement.appendChild(document.createTextNode(`
                ::-webkit-scrollbar {
                    -webkit-appearance: none;
                    width: 8px;
                }
                ::-webkit-scrollbar-thumb {
                    background: rgba(0,0,0,0.2);
                    border-radius: 3px;
                }
                ::-webkit-scrollbar-thumb:hover {
                    background: rgba(0,0,0,0.35);
                }
                ::-webkit-scrollbar-thumb:active {
                    background: rgba(0,0,0,0.5);
                }
                ::-webkit-scrollbar-track {
                    background: transparent;
                }
                ::-webkit-scrollbar-button {
                    width: 0px;
                    height: 0px;
                }
                ::-webkit-scrollbar-corner {
                    width: 0px;
                    height: 0px;
                }
            `));
            document.getElementsByTagName(\"head\")[0].appendChild(styleElement);"
        }

    ]

    onNavigationRequested: {
        if (String(request.url).search(vkAuthManager.authRedirectUrl()) === 0) {
            vkAuthManager.parseAuthResult(request.url);
            request.action = WebEngineNavigationRequest.IgnoreRequest;


            url = prevUrl;
            prevUrl = "";
        }
    }

    onUrlChanged: {
        // If we see one of that url - it's means that we have logged out from session.
        if ((url.toString() === "https://vk.com/" || url.toString().includes("https://vk.com/login"))
                && vkAuthManager.token != "") {
            vkAuthManager.loginOut();
        }
    }

    onLoadingChanged: {
        if (loadRequest.status === WebEngineLoadRequest.LoadFailedStatus) {
            var url = loadRequest.url.toString().replace("https://", "")

            if (url.length > 50) {
                url = url.slice(0, 49) + "...";
            }

            failedStatusPlaceHolder.failedUrl = url
            failedStatusPlaceHolder.visible = true;
            return;
        }
        failedStatusPlaceHolder.visible = false;
    }

    Connections {
        target: vkAuthManager

        onAuthRequired: authorise()
        onAuthError: globalErrorToast.error(qsTr("Ошибка авторизации"), qsTr("%1: %2").arg(error).arg(errorMessage))
    }

    Connections {
        target: networkConnect
        // Somehow, calling reload() immediately after network is back online
        // will still result in failed reload, so slight delay is required
        onIsOnlineChanged: if (networkConnect.isOnline) reloadTimer.restart()
    }

    Timer {
        id: reloadTimer
        onTriggered: reload();
        interval: 1000
    }
}
