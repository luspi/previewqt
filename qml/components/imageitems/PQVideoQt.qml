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
import QtMultimedia
import PreviewQt

Video {

    id: video

    parent: image_top

    // earlier versions of Qt6 seem to struggle if only one slash is used
    source: image_top.imageSource!=="" ? ((PQCScriptsConfig.isQtAtLeast6_5() ? "file:/" : "file://") + image_top.imageSource) : ""
    onSourceChanged: {
        video.play()
    }

    // dummy item
    property bool asynchronous

    property size sourceSize: Qt.size(video.width, video.height)
    property int paintedWidth: video.width
    property int paintedHeight: video.height

    property var volumeList: [1.0, 0.8, 0.45, 0]
    property int volumeIndex: 0
    property var volumeIcon: ["high", "medium", "low", "mute"]

    volume: volumeList[volumeIndex]

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    scale: Math.min(image_top.width/width, image_top.height/height)

    onPositionChanged: {
        if(position >= duration-100) {
            video.seek(0)
        }
    }

    onPlaybackStateChanged: {

        if(playbackState === MediaPlayer.StoppedState) {

            // earlier versions of Qt6 seem to struggle if only one slash is used
            if(PQCScriptsConfig.isQtAtLeast6_5())
                video.source = "file:/" + image_top.imageSource
            else
                video.source = "file://" + image_top.imageSource

            video.play()
        }

    }

    onWidthChanged: {
        if(width > 15 && height > 15) {
            image.status = Image.Ready
        }
    }

    onHeightChanged: {
        if(width > 15 && height > 15) {
            image.status = Image.Ready
        }
    }

    property int videoPosition: 0
    onVideoPositionChanged:
        video.position = videoPosition*1000

    Rectangle {

        parent: image_top

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        width: controlrow.width+10
        height: 30
        radius: 5
        z: 1

        color: "#88000000"
        opacity: controlsmouse.containsMouse||playpausemouse.containsMouse||slider.hovered||volumemouse.containsMouse ? 1 : 0.4
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
                source: video.playbackState===MediaPlayer.PlayingState ? "image://svg/:/pause.svg" : "image://svg/:/play.svg"
                MouseArea {
                    id: playpausemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(video.playbackState===MediaPlayer.PlayingState)
                            video.pause()
                        else
                            video.play()
                    }
                }
            }
            Text {
                y: (parent.height-height)/2
                color: "white"
                text: Math.round(video.position/1000) + "s"
            }

            Slider {
                id: slider
                y: (parent.height-height)/2
                orientation: Qt.Horizontal
                width: 100
                from: 0
                value: Math.round(video.position/1000)
                to: Math.round(video.duration/1000)
                onValueChanged: {
                    if(pressed)
                        videoPosition = value
                }
            }
            Text {
                y: (parent.height-height)/2
                color: "white"
                text: Math.round(video.duration/1000) + "s"
            }
            Image {
                y: (parent.height-height)/2
                visible: video.hasAudio
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
                    }
                }
            }


        }

    }

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Space) {

                if(video.playbackState===MediaPlayer.PlayingState)
                    video.pause()
                else
                    video.play()

            } else if(keycode === Qt.Key_Left) {

                video.seek(video.position-5000)

            } else if(keycode === Qt.Key_Right) {

                video.seek(video.position+5000)

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_M) {

                if(volumeIndex != 3)
                    volumeIndex = 3
                else
                    volumeIndex = 0

            } else if(keycode === Qt.Key_Home) {

                video.seek(0)

            } else if(keycode === Qt.Key_End) {

                video.seek(video.duration)

            }

        }

    }

}
