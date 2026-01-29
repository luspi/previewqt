/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
 ** Contact: https://previewqt.org                                       **
 **                                                                      **
 ** This file is part of PreviewQt.                                      **
 **                                                                      **
 ** PreviewQt is free software: you can redistribute it and/or modify    **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PreviewQt is distributed in the hope that it will be useful,         **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PreviewQt. If not, see <http://www.gnu.org/licenses/>.    **
 **                                                                      **
 **************************************************************************/

import QtQuick
import QtQuick.Controls
import QtWebEngine
import PreviewQt

Item {

    id: url_top

    x: 0
    y: 0

    width: PQCConstants.imageAvailableSize.width
    height: PQCConstants.imageAvailableSize.height

    property Item imageParent

    property string isWebsite: ""
    property string isVideo: ""

    Component.onCompleted: {

        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(width, height) })
        PQCConstants.imageAsynchronous = Qt.binding(function() { return true })

        PQCScriptsImages.requestIsSupportedStream(PQCConstants.currentSource)

    }

    Component.onDestruction: {
        PQCConstants.mainwindowOverrideTitle = ""
        PQCConstants.currentStreamVideoDirectURL = ""
    }

    Connections {

        target: PQCScriptsImages

        function onReceivedStreamSupported(supp : bool) {
            if(supp) {
                PQCScriptsImages.requestStreamTitle(PQCConstants.currentSource)
                PQCScriptsImages.requestStreamURL(PQCConstants.currentSource)
            } else
                url_top.isWebsite = PQCConstants.currentSource
        }

        function onReceivedStreamURL(url : string) {
            PQCConstants.currentStreamVideoDirectURL = url
            url_top.isVideo = url
        }

        function onReceivedStreamTitle(title : string) {
            PQCConstants.mainwindowOverrideTitle = title
        }

        function onReceivedStreamError(err : string) {
            if(err === "signin_bot")
                PQCNotify.trayiconShowNotification(qsTr("Stream error"), qsTr("You need to sign in to YouTube. Are you connected to a VPN? Showing normal website."))
            else if(err === "plugin_error")
                PQCNotify.trayiconShowNotification(qsTr("Stream error"), qsTr("The relevant yt-dlp plugin seems to have some issues, showing normal website."))
            else
                PQCNotify.trayiconShowNotification(qsTr("Stream error"), qsTr("The video stream could not be determined, showing normal website."))
            if(err !== "")
                url_top.isWebsite = PQCConstants.currentSource
        }

    }

    Loader {

        anchors.fill: parent
        active: url_top.isWebsite!==""

        sourceComponent:
        WebEngineView {

            anchors.fill: parent

            url: url_top.isWebsite

            Component.onCompleted: {

            }

        }
    }

    Text {
        id: loading
        anchors.centerIn: parent
        color: "white"
        text: qsTr("Loading...")
        font.pointSize: 12
        font.bold: true
        visible: url_top.isVideo==="" && url_top.isWebsite===""
    }

    Loader {

        anchors.fill: parent
        active: url_top.isVideo!=="" && PQCScriptsConfig.isQtVideoEnabled()

        sourceComponent:
        PQVideoQt {

            imageParent: url_top.imageParent

            overrideSource: url_top.isVideo

            Component.onCompleted: {
                play()
            }

        }
    }

    Loader {

        anchors.fill: parent
        active: url_top.isVideo!=="" && PQCScriptsConfig.isMPVEnabled()

        sourceComponent:
        PQVideoMpv {

            imageParent: url_top.imageParent

            overrideSource: url_top.isVideo

            Component.onCompleted: {
                play()
            }

        }
    }

}
