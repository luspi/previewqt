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

        if(PQCScriptsImages.isLocalURL(PQCConstants.currentSource)) {
            url_top.isWebsite = "file://"+ PQCConstants.currentSource
        } else {
            if(PQCSettings.processUrlWithYtdlp)
                PQCScriptsExternalTools.ytdlpRequestIsSupportedStream(PQCConstants.currentSource)
            else
                url_top.isWebsite = PQCConstants.currentSource
        }

    }

    Component.onDestruction: {
        PQCConstants.mainwindowOverrideTitle = ""
        PQCConstants.currentStreamVideoDirectURL = ""
    }

    Connections {

        target: PQCScriptsExternalTools

        function onYtdlpReceivedStreamSupported(supp : bool) {
            console.log("args: supp =", supp)
            if(supp) {
                PQCScriptsExternalTools.ytdlpRequestStreamTitle(PQCConstants.currentSource)
                PQCScriptsExternalTools.ytdlpRequestStreamURL(PQCConstants.currentSource)
            } else {
                url_top.isVideo = ""
                url_top.isWebsite = PQCConstants.currentSource
            }
        }

        function onYtdlpReceivedStreamURL(url : string) {
            console.log("args: url =", url)
            PQCConstants.currentStreamVideoDirectURL = url
            url_top.isVideo = url
            url_top.isWebsite = ""
        }

        function onYtdlpReceivedStreamTitle(title : string) {
            console.log("args: title =", title)
            PQCConstants.mainwindowOverrideTitle = title
        }

        function onYtdlpReceivedStreamError(err : string) {
            console.log("args: err =", err)
            if(err === "no_stream_found")
                PQCNotify.trayiconShowNotification(qsTr("Stream error"), qsTr("No stream at that URL was found, showing normal website."))
            else if(err === "signin_bot")
                PQCNotify.trayiconShowNotification(qsTr("Stream error"), qsTr("You need to sign in to YouTube. Are you connected to a VPN? Showing normal website."))
            else if(err === "plugin_error")
                PQCNotify.trayiconShowNotification(qsTr("Stream error"), qsTr("The relevant yt-dlp plugin seems to have some issues, showing normal website."))
            else
                PQCNotify.trayiconShowNotification(qsTr("Stream error"), qsTr("The video stream could not be determined, showing normal website."))
            if(err !== "") {
                url_top.isVideo = ""
                url_top.isWebsite = PQCConstants.currentSource
            }
        }

        function onYtdlpFinished() {
            console.log("")
            giveTimeToReactToFinished.restart()
        }

    }

    Timer {
        id: giveTimeToReactToFinished
        interval: 500
        onTriggered: {
            if(url_top.isVideo === "" && url_top.isWebsite === "") {
                PQCNotify.trayiconShowNotification(qsTr("No stream found"), qsTr("No stream at that URL was found, showing normal website."))
                url_top.isVideo = ""
                url_top.isWebsite = PQCConstants.currentSource
            }
        }
    }

    Loader {

        anchors.fill: parent
        active: url_top.isWebsite!==""

        sourceComponent:
        PQWebsiteView {

            anchors.fill: parent

            url: url_top.isWebsite

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

    Image {
        id: musicImage
        property int dim: Math.min(Math.min(200, parent.width*(2/3)), Math.min(200, parent.height*(2/3)))
        x: (parent.width-width)/2
        y: (parent.height-height)/2
        width: dim
        height: dim
        sourceSize: Qt.size(width, height)
        visible: false
        opacity: 0.8
        source: "image://svg/:/musicnote.svg"
    }

    Loader {

        anchors.fill: parent
        active: url_top.isVideo!=="" && PQCScriptsConfig.isQtMultimediaEnabled()

        sourceComponent:
        PQVideoQt {

            imageParent: url_top.imageParent

            overrideSource: url_top.isVideo

            // only audio is present
            onIHaveAudio: {
                musicImage.visible = true
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

            // only audio is present
            onIHaveAudio: {
                musicImage.visible = true
            }

        }
    }

}
