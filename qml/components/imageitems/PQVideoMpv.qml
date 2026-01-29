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

// The MPV object needs to be wrapped in an item
// and be as empty as shown below
// otherwise a separate mpv window will open up
Item {

    id: videotop

    x: (PQCConstants.imageAvailableSize.width-width)/2
    y: (PQCConstants.imageAvailableSize.height-height)/2

    scale: Math.min(PQCConstants.imageAvailableSize.width/width, PQCConstants.imageAvailableSize.height/height)

    width: 10
    height: 10

    property Item imageParent

    property string overrideSource: ""

    property int videoDuration: 0
    property int videoPosition: 0
    property bool videoPlaying: false

    Component.onCompleted: {
        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(video.width, video.height) })
        PQCConstants.imageAsynchronous = false
    }

    onVideoPlayingChanged: {
        video.command(["set", "pause", (videoPlaying ? "no" : "yes")])
    }

    property var volumeList: [100, 80, 45, 0]
    property int volumeIndex: 0
    property var volumeIcon: ["high", "medium", "low", "mute"]

    onWidthChanged: {
        if(width > 15 && height > 15) {
            PQCConstants.imageStatus = Image.Ready
        }
    }

    onHeightChanged: {
        if(width > 15 && height > 15) {
            PQCConstants.imageStatus = Image.Ready
        }
    }

    PQCMPVObject {

        id: video

        transformOrigin: Item.Center

        width: parent.width
        height: parent.height

    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            // then this is (likely) an internet stream
            if(videotop.overrideSource !== "") {
                video.command(["set", "user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"])
                video.command(["set", "referrer", PQCScriptsOther.getDomainOfUrl(videotop.overrideSource)]);
            }
            video.command(["loadfile", (overrideSource==="" ? PQCConstants.currentSource : overrideSource)])
            getProps.restart()
        }
    }

    Timer {
        id: getProps
        interval: 100
        repeat: false
        running: false
        onTriggered: {
            // check whether the file has fully loaded yet
            // depending on the Qt version there will be a comma at the end of the error message string
            var tmp = video.getProperty("width")+""
            if(tmp == "QVariant(mpv::qt::ErrorReturn)" || tmp == "QVariant(mpv::qt::ErrorReturn, )") {
                getProps.restart()
                return
            }
            videotop.width = video.getProperty("width")
            videotop.height = video.getProperty("height")
            videoDuration = video.getProperty("duration")
            video.setProperty("volume", volumeList[volumeIndex])
            getPosition.restart()
            PQCNotify.resetFocus()
        }
    }

    Timer {
        id: getPosition
        interval: videoPlaying ? 250 : 500
        repeat: true
        running: false
        property bool restarting: false
        onTriggered: {
            videoPlaying = !video.getProperty("core-idle")
            if(video.getProperty("eof-reached")) {
                if(!restarting) {
                    video.command(["loadfile", (overrideSource==="" ? PQCConstants.currentSource : overrideSource)])
                    restarting = true
                }
            } else {
                videoPosition = video.getProperty("time-pos")
                restarting = false
            }

        }
    }

    Rectangle {

        parent: videotop.imageParent

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        width: controlrow.width+10
        height: 30
        radius: 5

        color: "#88000000"
        opacity: controlsmouse.containsMouse||playpausemouse.containsMouse||slider.hovered/*||volumemouse.containsMouse*/ ? 1 : 0.4
        Behavior on opacity { NumberAnimation { duration: 200 } }

        MouseArea {
            id: controlsmouse
            anchors.fill: parent
            hoverEnabled: true
        }

        Row {
            id: controlrow
            x: 5
            y: (parent.height-height)/2
            spacing: 5
            Image {
                y: (parent.height-height)/2
                width: height
                height: 20
                sourceSize: Qt.size(width, height)
                source: videotop.videoPlaying ? "image://svg/:/pause.svg" : "image://svg/:/play.svg"
                MouseArea {
                    id: playpausemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(video.getProperty("eof-reached")) {
                            video.command(["loadfile", (overrideSource==="" ? PQCConstants.currentSource : overrideSource)])
                            videoPlaying = true
                        } else {
                            videoPlaying = !videoPlaying
                        }
                    }
                }
            }
            Text {
                y: (parent.height-height)/2
                color: "white"
                text: videoPosition + "s"
            }

            Slider {
                id: slider
                y: (parent.height-height)/2
                orientation: Qt.Horizontal
                width: 100
                from: 0
                value: videoPosition
                to: videoDuration
                onValueChanged: {
                    if(pressed) {
                        if(video.getProperty("eof-reached")) {
                            video.command(["loadfile", (overrideSource==="" ? PQCConstants.currentSource : overrideSource)])
                            videoPlaying = false
                            setPosTimeout.pos = value
                            setPosTimeout.restart()
                        } else
                            video.command(["seek", value, "absolute"])
                    }
                    PQCNotify.resetFocus()
                }
            }
            Text {
                y: (parent.height-height)/2
                color: "white"
                text: videoDuration + "s"
            }
            Image {
                y: (parent.height-height)/2
                width: height
                height: 20
                sourceSize: Qt.size(width, height)
                source: "image://svg/:/volume_" + volumeIcon[volumeIndex] + ".svg"
                MouseArea {
                    id: volumemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        volumeIndex = (volumeIndex+1)%4
                        video.setProperty("volume", volumeList[volumeIndex])
                    }
                }
            }


        }

    }

    Timer {
        id: setPosTimeout
        interval: 500
        property int pos
        onTriggered:
            video.command(["seek", pos, "absolute"])
    }

    Connections {

        target: PQCNotify

        function onMainwindowKeyPress(modifiers : int, keycode : int) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Space) {

                videoPlaying = !videoPlaying

            } else if(keycode === Qt.Key_Left) {

                video.command(["seek", Math.max(0, videoPosition-5), "absolute"])

            } else if(keycode === Qt.Key_Right) {

                video.command(["seek", Math.min(videoDuration, videoPosition+5), "absolute"])

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_M) {

                if(volumeIndex != 3)
                    volumeIndex = 3
                else
                    volumeIndex = 0
                video.setProperty("volume", volumeList[volumeIndex])

            } else if(keycode === Qt.Key_Home) {

                video.command(["seek", 0, "absolute"])

            } else if(keycode === Qt.Key_End) {

                video.command(["seek", videoDuration, "absolute"])

            }

        }

    }

}
