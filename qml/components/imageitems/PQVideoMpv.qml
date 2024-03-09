/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of PhotoQt.                                        **
 **                                                                      **
 ** PhotoQt is free software: you can redistribute it and/or modify      **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PhotoQt is distributed in the hope that it will be useful,           **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PhotoQt. If not, see <http://www.gnu.org/licenses/>.      **
 **                                                                      **
 **************************************************************************/

import QtQuick
import QtQuick.Controls

import PQCScripts
import PQCMPVObject

// The MPV object needs to be wrapped in an item
// and be as empty as shown below
// otherwise a separate mpv window will open up
Item {

    id: videotop

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    scale: Math.min(image_top.width/width, image_top.height/height)

    width: 100
    height: 100

    property int videoDuration: 0
    property int videoPosition: 0
    property bool videoPlaying: false

    property var volumeList: [100, 80, 45, 0]
    property int volumeIndex: 0
    property var volumeIcon: ["high", "medium", "low", "mute"]

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
            video.command(["loadfile", image_top.imageSource])
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
                    video.command(["loadfile", image_top.imageSource])
                    restarting = true
                }
            } else {
                videoPosition = video.getProperty("time-pos")
                restarting = false
            }

        }
    }

    Rectangle {

        parent: image_top

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
                source: videotop.videoPlaying ? "/pause.svg" : "/play.svg"
                MouseArea {
                    id: playpausemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(video.getProperty("eof-reached")) {
                            video.command(["loadfile", image_top.imageSource])
                            videoPlaying = true
                        } else {
                            videoPlaying = !videoPlaying
                            video.command(["cycle", "pause"])
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
                            video.command(["loadfile", image_top.imageSource])
                            video.command(["cycle", "pause"])
                            videoPlaying = false
                            setPosTimeout.pos = value
                            setPosTimeout.restart()
                        } else
                            video.command(["seek", value, "absolute"])
                    }
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
                source: "/volume_" + volumeIcon[volumeIndex] + ".svg"
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

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Space) {

                videoPlaying = !videoPlaying
                video.command(["cycle", "pause"])

            } else if(keycode === Qt.Key_Left) {

                video.command(["seek", Math.max(0, videoPosition-5), "absolute"])

            } else if(keycode === Qt.Key_Right) {

                video.command(["seek", Math.min(videoDuration, videoPosition+5), "absolute"])

            } else if(keycode === Qt.Key_M) {

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
