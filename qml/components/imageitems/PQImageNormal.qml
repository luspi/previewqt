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
import QtMultimedia
import PreviewQt

Item {

    id: img_top

    x: (PQCConstants.imageAvailableSize.width-width)/2
    y: (PQCConstants.imageAvailableSize.height-height)/2

    width: imageitem.width
    height: imageitem.height

    property Item imageParent

    Component.onCompleted: {
        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(imageitem.paintedWidth, imageitem.paintedHeight) })
        PQCConstants.imageAsynchronous = Qt.binding(function() { return imageitem.asynchronous })
    }

    Image {

        id: imageitem

        source: PQCConstants.currentSource!=="" ? ("image://full/" + PQCScriptsFilesPaths.toPercentEncoding(PQCConstants.currentSource)) : ""

        asynchronous: true

        fillMode: Image.PreserveAspectFit

        smooth: false
        mipmap: false

        rotation: PQCConstants.imageRotation

        property int defw: Math.max(50, PQCSettings.defaultWindowWidth)
        property int defh: Math.max(50, PQCSettings.defaultWindowHeight)

        width: rotation%180===0 ? PQCConstants.imageAvailableSize.width : PQCConstants.imageAvailableSize.height
        height: rotation%180===0 ? PQCConstants.imageAvailableSize.height : PQCConstants.imageAvailableSize.width
        sourceSize: (PQCSettings.maximizeImageSizeAndAdjustWindow && !PQCConstants.mainwindowIsMaximized &&
                     !PQCConstants.mainwindowIsFullscreen && !PQCConstants.mainwindowManuallyResized) ?
                        (rotation%180===0 ? Qt.size(defw, defh) : Qt.size(defh, defw)) :
                        (rotation%180===0 ? Qt.size(PQCConstants.imageAvailableSizeDelay.width,
                                                    PQCConstants.imageAvailableSizeDelay.height) : Qt.size(PQCConstants.imageAvailableSizeDelay.height,
                                                                                                      PQCConstants.imageAvailableSizeDelay.width))

        onStatusChanged: {
            PQCConstants.imageStatus = status
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
            else if(status == Image.Ready) {
                asynchronous = false
            }
        }

    }

    // The code below handles motion photos

    Timer {

        interval: 200
        running: true

        onTriggered: {

            if(PQCScriptsConfig.isMotionPhotoSupportEnabled()) {

                var what = PQCScriptsImages.isMotionPhoto(PQCConstants.currentSource)

                if(what > 0) {

                    var src = ""

                    if(what === 1)
                        src = PQCScriptsFilesPaths.getDir(PQCConstants.currentSource) + "/" + PQCScriptsFilesPaths.getBasename(PQCConstants.currentSource) + ".mov"
                    else if(what === 2 || what === 3)
                        src = PQCScriptsImages.extractMotionPhoto(PQCConstants.currentSource)

                    if(src != "") {

                        // HEIF/HEIC images are a little trickier with their orientation handling
                        // We need to ignore this value as the Exif orientation might not be correct
                        // See also: https://github.com/Exiv2/exiv2/issues/2958
                        var suf = PQCScriptsFilesPaths.getSuffix(PQCConstants.currentSource).toLowerCase()
                        if(suf !== "heic" && suf !== "heif") {

                            var orientation = PQCScriptsImages.getExifOrientation(PQCConstants.currentSource)
                            switch(orientation) {

                            case 1:
                                // no rotation, no mirror
                                videoloader.forceRotation = 0
                                videoloader.forceMirror = false
                                break;
                            case 2:
                                // no rotation, horizontal mirror
                                videoloader.forceRotation = 0
                                videoloader.forceMirror = true
                                break;
                            case 3:
                                // 180 degree rotation, no mirror
                                videoloader.forceRotation = 180
                                videoloader.forceMirror = false
                                break;
                            case 4:
                                // 180 degree rotation, horizontal mirror
                                videoloader.forceRotation = 180
                                videoloader.forceMirror = true
                                break;
                            case 5:
                                // 90 degree rotation, horizontal mirror
                                videoloader.forceRotation = 90
                                videoloader.forceMirror = true
                                break;
                            case 6:
                                // 90 degree rotation, no mirror
                                videoloader.forceRotation = 90
                                videoloader.forceMirror = false
                                break;
                            case 7:
                                // 270 degree rotation, horizontal mirror
                                videoloader.forceRotation = 270
                                videoloader.forceMirror = true
                                break;
                            case 8:
                                // 270 degree rotation, no mirror
                                videoloader.forceRotation = 270
                                videoloader.forceMirror = false
                                break;
                            default:
                                console.warn("Unexpected orientation value received:", orientation)
                                break;

                            }

                        }

                        videoloader.active = false
                        // earlier versions of Qt6 seem to struggle if only one slash is used
                        if(PQCScriptsConfigQML.isQtAtLeast6_5())
                            videoloader.mediaSrc = "file:/" + src
                        else
                            videoloader.mediaSrc = "file://" + src
                        videoloader.active = true
                        return
                    }

                }

            } else
                videoloader.mediaSrc = ""

        }

    }

    // we hide the video element behind a loader so that we don't even have to set it up if no video is found

    Loader {
        id: videoloader

        active: false
        property string mediaSrc: ""

        property int forceRotation: 0
        property bool forceMirror: false

        asynchronous: true
        sourceComponent: motionphoto
    }

    Component {

        id: motionphoto

        Item {

            width: imageitem.width
            height: imageitem.height
            transform:
                Rotation {
                    origin.x: width / 2
                    axis { x: 0; y: 1; z: 0 }
                    angle: videoloader.forceMirror ? 180 : 0
                }

            Video {
                id: mediaplayer
                rotation: videoloader.forceRotation
                anchors.fill: parent
                anchors.margins: rotation%180==0 ? 0 : -(imageitem.height-imageitem.width)/2
                source: videoloader.mediaSrc
                Component.onCompleted: {
                    play()
                }
            }

            Rectangle {

                parent: img_top.imageParent

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

    }

    Connections {

        target: PQCNotify

        function onSetImageAsync(async : bool) {
            imageitem.asynchronous = async
        }

    }

}
