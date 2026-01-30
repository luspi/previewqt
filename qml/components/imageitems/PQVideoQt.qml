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

    parent: imageParent

    // earlier versions of Qt6 seem to struggle if only one slash is used
    source: overrideSource==="" ?
                (PQCConstants.currentSource!=="" ? ((PQCScriptsConfig.isQtAtLeast6_5() ? "file:/" : "file://") + PQCConstants.currentSource) : "") :
                overrideSource
    onSourceChanged: {
        video.play()
    }

    Component.onCompleted: {
        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(video.width, video.height) })
        PQCConstants.imageAsynchronous = false

        PQCConstants.mediainfoIsAudio = false
        PQCConstants.mediainfoIsVideo = true
        PQCConstants.mediainfoTitle = ""
        PQCConstants.mediainfoArtist = ""
        PQCConstants.mediainfoAlbum = ""
        PQCConstants.mediainfoDate = ""
        PQCConstants.mediainfoGenre = ""
        PQCConstants.mediainfoCopyright = ""
        PQCConstants.mediainfoAudioTrack = ""
        PQCConstants.mediainfoAudioBitRate = ""
        PQCConstants.mediainfoAudioCodec = ""
        PQCConstants.mediainfoVideoFrameRate = ""
        PQCConstants.mediainfoVideoBitRate = ""
        PQCConstants.mediainfoVideoCodec = ""
        PQCConstants.mediainfoVideoHdr = ""

    }

    Component.onDestruction: {
        PQCConstants.mediainfoIsAudio = false
        PQCConstants.mediainfoIsVideo = false
        PQCConstants.mediainfoTitle = ""
        PQCConstants.mediainfoArtist = ""
        PQCConstants.mediainfoAlbum = ""
        PQCConstants.mediainfoDate = ""
        PQCConstants.mediainfoGenre = ""
        PQCConstants.mediainfoCopyright = ""
        PQCConstants.mediainfoAudioTrack = ""
        PQCConstants.mediainfoAudioBitRate = ""
        PQCConstants.mediainfoAudioCodec = ""
        PQCConstants.mediainfoVideoFrameRate = ""
        PQCConstants.mediainfoVideoBitRate = ""
        PQCConstants.mediainfoVideoCodec = ""
        PQCConstants.mediainfoVideoHdr = ""
    }

    property Item imageParent

    property string overrideSource: ""

    property list<double> volumeList: [1.0, 0.8, 0.45, 0]
    property int volumeIndex: 0
    property list<string> volumeIcon: ["high", "medium", "low", "mute"]

    volume: volumeList[volumeIndex]

    x: (PQCConstants.imageAvailableSize.width-width)/2
    y: (PQCConstants.imageAvailableSize.height-height)/2

    scale: Math.min(PQCConstants.imageAvailableSize.width/width, PQCConstants.imageAvailableSize.height/height)

    onPositionChanged: {
        if(position >= duration-100) {
            video.seek(0)
        }
    }

    onPlaybackStateChanged: {

        if(playbackState === MediaPlayer.StoppedState) {

            // earlier versions of Qt6 seem to struggle if only one slash is used
            if(PQCScriptsConfig.isQtAtLeast6_5())
                video.source = "file:/" + PQCConstants.currentSource
            else
                video.source = "file://" + PQCConstants.currentSource

            video.play()
        }

    }

    onMetaDataChanged: {

        PQCConstants.mediainfoTitle = metaData.stringValue(MediaMetaData.Title)
        if(PQCConstants.mediainfoTitle === "") PQCConstants.mediainfoTitle = PQCScriptsFilesPaths.getBasename(PQCConstants.currentSource)

        PQCConstants.mediainfoArtist = metaData.stringValue(MediaMetaData.AlbumArtist)
        if(PQCConstants.mediainfoArtist === "") PQCConstants.mediainfoArtist = metaData.stringValue(MediaMetaData.ContributingArtist)

        if(PQCConstants.mediainfoTitle !== "" && PQCConstants.mediainfoArtist !== "")
            PQCConstants.mainwindowOverrideTitle = PQCConstants.mediainfoTitle + " | " + PQCConstants.mediainfoArtist
        else if(PQCConstants.mediainfoTitle !== "" || PQCConstants.mediainfoArtist !== "")
            PQCConstants.mainwindowOverrideTitle = PQCConstants.mediainfoTitle + PQCConstants.mediainfoArtist

        PQCConstants.mediainfoDate = metaData.stringValue(MediaMetaData.Date)
        PQCConstants.mediainfoAlbum = metaData.stringValue(MediaMetaData.AlbumTitle)
        PQCConstants.mediainfoAudioTrack = metaData.stringValue(MediaMetaData.TrackNumber)
        PQCConstants.mediainfoGenre = metaData.stringValue(MediaMetaData.Genre)
        PQCConstants.mediainfoCopyright = metaData.stringValue(MediaMetaData.Copyright)

        PQCConstants.mediainfoVideoFrameRate = metaData.stringValue(MediaMetaData.VideoFrameRate)
        PQCConstants.mediainfoVideoBitRate = metaData.stringValue(MediaMetaData.VideoBitRate)
        PQCConstants.mediainfoVideoCodec = metaData.stringValue(MediaMetaData.VideoCodec)
        PQCConstants.mediainfoVideoHdr = metaData.stringValue(MediaMetaData.HasHdrContent)

    }

    onWidthChanged: {
        PQCConstants.imagePaintedSize.width = width
        if(width > 15 && height > 15) {
            PQCConstants.imageStatus = Image.Ready
        }
    }

    onHeightChanged: {
        PQCConstants.imagePaintedSize.height = height
        if(width > 15 && height > 15) {
            PQCConstants.imageStatus = Image.Ready
        }
    }

    property int videoPosition: 0
    onVideoPositionChanged:
        video.position = videoPosition*1000

    Rectangle {

        parent: video.imageParent

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
                text: PQCScriptsOther.convertSecondsToHumandFriendly(Math.round(video.position/1000), Math.round(video.duration/1000))
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
                    PQCNotify.resetFocus()
                }
            }
            Text {
                y: (parent.height-height)/2
                color: "white"
                text: PQCScriptsOther.convertSecondsToHumandFriendly(Math.round(video.duration/1000))
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

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if(video.playbackState===MediaPlayer.PlayingState)
                video.pause()
            else
                video.play()
        }
        onDoubleClicked: (mouse) => {
            if(mouse.button === Qt.RightButton) return
            if(PQCConstants.mainwindowIsFullscreen)
                PQCNotify.mainwindowShowNormal()
            else
                PQCNotify.mainwindowShowFullscreen()
        }
    }

    Connections {

        target: PQCNotify

        function onMainwindowKeyPress(modifiers : int, keycode : int) {

            if(keycode === Qt.Key_Space && modifiers === Qt.NoModifier) {

                if(video.playbackState===MediaPlayer.PlayingState)
                    video.pause()
                else
                    video.play()

            } else if(keycode === Qt.Key_Left && modifiers === Qt.NoModifier) {

                video.seek(video.position-5000)

            } else if(keycode === Qt.Key_Right && modifiers === Qt.NoModifier) {

                video.seek(video.position+5000)

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_M) {

                if(volumeIndex != 3)
                    volumeIndex = 3
                else
                    volumeIndex = 0

            } else if(keycode === Qt.Key_Home && modifiers === Qt.NoModifier) {

                video.seek(0)

            } else if(keycode === Qt.Key_End && modifiers === Qt.NoModifier) {

                video.seek(video.duration)

            }

        }

    }

}
