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

AnimatedImage {

    id: image

    source: image_top.imageSource!=="" ? ("file:/" + image_top.imageSource) : ""

    asynchronous: true

    fillMode: Image.PreserveAspectFit

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    scale: Math.min(image_top.width/width, image_top.height/height)

    smooth: scale<1
    mipmap: scale<1

    onStatusChanged: {
        if(status == Image.Error)
            source = "image://svg/:/other/errorimage.svg"
        else if(status == Image.Ready) {
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
                source: image.playing ? "/pause.svg" : "/play.svg"
                MouseArea {
                    id: playpausemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked:
                        image.playing = !image.playing
                }
            }

            Slider {
                id: slider
                y: (parent.height-height)/2
                orientation: Qt.Horizontal
                width: 100
                from: 0
                value: image.currentFrame
                to: image.frameCount-1
                onValueChanged: {
                    if(pressed)
                        image.currentFrame = value
                }
            }


        }

    }

    Connections {

        target: image_top

        function onKeyPress(modifiers, keycode) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Space) {

                playing = !playing

            } else if(keycode === Qt.Key_Left) {

                currentFrame = (currentFrame+frameCount-1)%frameCount

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                currentFrame = (currentFrame+1)%frameCount

            } else if(keycode === Qt.Key_Home) {

                currentFrame = 0

            } else if(keycode === Qt.Key_End) {

                currentFrame = frameCount-1

            }

        }

    }

}
