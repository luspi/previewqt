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
import QtMultimedia
import PQCScripts

Image {

    id: image

    source: image_top.imageSource!=="" ? ("image://full/" + PQCScripts.toPercentEncoding(image_top.imageSource)) : ""

    asynchronous: true

    fillMode: Image.PreserveAspectFit

    smooth: false
    mipmap: false

    width: image_top.width
    height: image_top.height
    sourceSize: Qt.size(image_top.windowWidth, image_top.windowHeight)

    onStatusChanged: {
        if(status == Image.Error)
            source = "image://svg/:/errorimage.svg"
        else if(status == Image.Ready) {
            asynchronous = false
            if(extrasCheckedFor !== image_top.imageSource)
                checkForExtras.restart()
        }
    }

    property string extrasCheckedFor: ""

    Timer {
        id: checkForExtras
        interval: 50
        onTriggered: {

            extrasCheckedFor = image_top.imageSource

            var motion = PQCScripts.isMotionPhoto(image_top.imageSource)

            if(motion === 1) {

                mediaplayer_wrapper.forceRotation = 0
                mediaplayer_wrapper.forceMirror = false
                mediaplayer.source = "file:/" + PQCScripts.getDir(image_top.imageSource) + "/" + PQCScripts.getBasename(image_top.imageSource) + ".mov"
                mediaplayer.play()

            } else if(motion > 1) {

                var src = PQCScripts.extractMotionPhoto(image_top.imageSource)
                mediaplayer.source = "file:/" + src

                var orientation = PQCScripts.getExifOrientation(image_top.imageSource)
                switch(orientation) {

                case 1:
                    // no rotation, no mirror
                    mediaplayer_wrapper.forceRotation = 0
                    mediaplayer_wrapper.forceMirror = false
                    break;
                case 2:
                    // no rotation, horizontal mirror
                    mediaplayer_wrapper.forceRotation = 0
                    mediaplayer_wrapper.forceMirror = true
                    break;
                case 3:
                    // 180 degree rotation, no mirror
                    mediaplayer_wrapper.forceRotation = 180
                    mediaplayer_wrapper.forceMirror = false
                    break;
                case 4:
                    // 180 degree rotation, horizontal mirror
                    mediaplayer_wrapper.forceRotation = 180
                    mediaplayer_wrapper.forceMirror = true
                    break;
                case 5:
                    // 90 degree rotation, horizontal mirror
                    mediaplayer_wrapper.forceRotation = 90
                    mediaplayer_wrapper.forceMirror = true
                    break;
                case 6:
                    // 90 degree rotation, no mirror
                    mediaplayer_wrapper.forceRotation = 90
                    mediaplayer_wrapper.forceMirror = false
                    break;
                case 7:
                    // 270 degree rotation, horizontal mirror
                    mediaplayer_wrapper.forceRotation = 270
                    mediaplayer_wrapper.forceMirror = true
                    break;
                case 8:
                    // 270 degree rotation, no mirror
                    mediaplayer_wrapper.forceRotation = 270
                    mediaplayer_wrapper.forceMirror = false
                    break;
                default:
                    console.warn("Unexpected orientation value received:", orientation)
                    break;

                }

                mediaplayer.play()

            } else
                mediaplayer.source = ""

        }
    }

    Item {

        id: mediaplayer_wrapper

        property bool forceMirror: false
        property int forceRotation: 0

        width: image.width
        height: image.height

        transform:
            Rotation {
                origin.x: width / 2
                axis { x: 0; y: 1; z: 0 }
                angle: mediaplayer_wrapper.forceMirror ? 180 : 0
            }

        Video {
            id: mediaplayer
            rotation: mediaplayer_wrapper.forceRotation
            anchors.fill: parent
            anchors.margins: rotation%180==0 ? 0 : (image.paintedHeight-image.paintedWidth)/2
            source: ""
        }

    }

    Rectangle {

        x: parent.width-width-10
        y: parent.height-height-10

        width: 30
        height: 30
        color: "#88000000"
        radius: 5

        visible: mediaplayer.hasVideo

        opacity: playpausemouse.containsMouse ? 1 : 0.2
        Behavior on opacity { NumberAnimation { duration: 200 } }

        Image {
            anchors.fill: parent
            anchors.margins: 5
            sourceSize: Qt.size(width, height)
            source: mediaplayer.playbackState == MediaPlayer.PlayingState ? "image://svg/:/pause.svg" : "image://svg/:/play.svg"
        }

        MouseArea {
            id: playpausemouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if(mediaplayer.playbackState == MediaPlayer.PlayingState)
                    mediaplayer.pause()
                else
                    mediaplayer.play()
            }
        }

    }

}
