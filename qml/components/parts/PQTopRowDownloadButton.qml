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

PQIconButton {
    id: downbut
    visible: ["url"].indexOf(PQCConstants.currentType)>-1
    active: PQCConstants.currentStreamVideoDirectURL !== ""
    y: (parent.height-height)/2
    height: toprow.height-2
    tooltip: qsTr("Download video")
    source: "image://svg/:/download.svg"
    onClicked: {
        PQCScriptsOther.startDownloadOfFile(PQCConstants.currentStreamVideoDirectURL, PQCConstants.mainwindowOverrideTitle+".mp4")
    }
    MouseArea {
        enabled: !downbut.active && !PQCConstants.downloadInProgress
        anchors.fill: parent
        acceptedButtons: Qt.RightButton|Qt.LeftButton
        onClicked: {}
    }

    Rectangle {
        id: downloadProgress
        x: 0
        height: parent.height
        width: 0
        color: "#8800ff00"
        property bool downloadCancelled: false
        property bool showCheckmark: false
        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        Image {
            opacity: (downloadProgress.showCheckmark&&!downloadProgress.downloadCancelled ? 1 : 0)
            Behavior on opacity { NumberAnimation { duration: 200 } }
            anchors.fill: parent
            anchors.margins: 5
            sourceSize: Qt.size(width, height)
            source: "image://svg/:/checkmark.svg"
        }
        Image {
            parent: downbut
            visible: cancelmouse.containsMouse
            anchors.fill: parent
            anchors.margins: 8
            sourceSize: Qt.size(width, height)
            source: "image://svg/:/redx.svg"
        }
        MouseArea {
            id: cancelmouse
            parent: downbut
            enabled: PQCConstants.downloadInProgress
            anchors.fill: parent
            hoverEnabled: true
            onClicked: PQCScriptsOther.cancelDownloadOfFile()
        }
    }

    Timer {
        id: resetStatus
        interval: 2000
        onTriggered: {
            downloadProgress.opacity = 0
        }
    }

    Connections {

        target: PQCScriptsOther

        function onDownloadCancelled() {
            timeoutChecker.stop()
            downloadProgress.downloadCancelled = true
            resetStatus.triggered()
        }

        function onDownloadStarted() {
            PQCConstants.downloadInProgress = true
            downloadProgress.width = 0
            downloadProgress.showCheckmark = false
            downloadProgress.downloadCancelled = false
            downloadProgress.opacity = 1
            timeoutChecker.restart()
        }

        function onDownloadFinished() {
            timeoutChecker.stop()
            PQCConstants.downloadInProgress = false
            downloadProgress.showCheckmark = true
            resetStatus.restart()
        }

        function onDownloadProgress(bytesReceived : int, bytesTotal : int) {
            downloadProgress.width = (downbut.width-1)*(bytesReceived/bytesTotal)
            timeoutChecker.restart()
        }

    }

    // this is to make sure we don't stall out
    Timer {
        id: timeoutChecker
        interval: 5000
        onTriggered: {
            PQCScriptsOther.cancelDownloadOfFile()
            PQCNotify.trayiconShowNotification("Timeout", "No content received for a little while. Cancelling download.")
            downloadProgress.downloadCancelled = true
            resetStatus.restart()
        }
    }

}

