/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
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

    id: ani_top

    x: (PQCConstants.imageAvailableSize.width-width)/2
    y: (PQCConstants.imageAvailableSize.height-height)/2

    width: imageitem.width
    height: imageitem.height

    property Item imageParent

    Component.onCompleted: {
        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(imageitem.paintedWidth, imageitem.paintedHeight) })
        PQCConstants.imageAsynchronous = Qt.binding(function() { return imageitem.asynchronous })
    }

    AnimatedImage {

        id: imageitem

        source: PQCConstants.currentSource!=="" ? ("file:/" + PQCScriptsFilesPaths.toPercentEncoding(PQCConstants.currentSource)) : ""

        asynchronous: true

        fillMode: Image.PreserveAspectFit

        smooth: Math.abs(sourceSize.width-width) > 100
        mipmap: smooth

        rotation: PQCConstants.imageRotation

        width: rotation%180===0 ? PQCConstants.imageAvailableSize.width : PQCConstants.imageAvailableSize.height
        height: rotation%180===0 ? PQCConstants.imageAvailableSize.height : PQCConstants.imageAvailableSize.width

        onStatusChanged: {
            PQCConstants.imageStatus = status
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
        }

    }

    Rectangle {

        parent: ani_top.imageParent

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        width: controlrow.width+10
        height: 30
        radius: 5

        color: "#88000000"
        opacity: controlsmouse.containsMouse||playpausemouse.containsMouse||slider.hovered ? 1 : 0.4
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
                source: imageitem.playing ? "image://svg/:/pause.svg" : "image://svg/:/play.svg"
                MouseArea {
                    id: playpausemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked:
                        imageitem.playing = !imageitem.playing
                }
            }

            Slider {
                id: slider
                y: (parent.height-height)/2
                orientation: Qt.Horizontal
                width: 100
                from: 0
                value: imageitem.currentFrame
                to: imageitem.frameCount-1
                onValueChanged: {
                    if(pressed)
                        imageitem.currentFrame = value
                    PQCNotify.resetFocus()
                }
            }


        }

    }

    Connections {

        target: PQCNotify

        function onMainwindowKeyPress(modifiers : int, keycode : int) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Space) {

                imageitem.playing = !imageitem.playing

            } else if(keycode === Qt.Key_Left) {

                imageitem.currentFrame = (imageitem.currentFrame+imageitem.frameCount-1)%imageitem.frameCount

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                imageitem.currentFrame = (imageitem.currentFrame+1)%imageitem.frameCount

            } else if(keycode === Qt.Key_Home) {

                imageitem.currentFrame = 0

            } else if(keycode === Qt.Key_End) {

                imageitem.currentFrame = imageitem.frameCount-1

            }

        }

        function onSetImageAsync(async : bool) {
            imageitem.asynchronous = async
        }

    }

}
