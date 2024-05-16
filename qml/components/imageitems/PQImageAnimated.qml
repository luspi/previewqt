/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2024 Lukas Spies                                       **
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
import PQCScripts

Item {

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    width: imageitem.width
    height: imageitem.height

    property alias sourceSize: imageitem.sourceSize
    property alias asynchronous: imageitem.asynchronous
    property alias paintedWidth: imageitem.paintedWidth
    property alias paintedHeight: imageitem.paintedHeight

    AnimatedImage {

        id: imageitem

        source: image_top.imageSource!=="" ? ("file:/" + PQCScripts.toPercentEncoding(image_top.imageSource)) : ""

        asynchronous: true

        fillMode: Image.PreserveAspectFit

        smooth: Math.abs(sourceSize.width-width) > 100
        mipmap: smooth

        rotation: image_top.setRotation

        width: rotation%180===0 ? image_top.width : image_top.height
        height: rotation%180===0 ? image_top.height : image_top.width

        onStatusChanged: {
            image.status = status
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
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
                    focusitem.forceActiveFocus()
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

    }

}
